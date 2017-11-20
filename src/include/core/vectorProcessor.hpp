/*F***************************************************************************
 * 
 * openSMILE - the Munich open source Multimedia Interpretation by 
 * Large-scale Extraction toolkit
 * 
 * This file is part of openSMILE.
 * 
 * openSMILE is copyright (c) by audEERING GmbH. All rights reserved.
 * 
 * See file "COPYING" for details on usage rights and licensing terms.
 * By using, copying, editing, compiling, modifying, reading, etc. this
 * file, you agree to the licensing terms in the file COPYING.
 * If you do not agree to the licensing terms,
 * you must immediately destroy all copies of this file.
 * 
 * THIS SOFTWARE COMES "AS IS", WITH NO WARRANTIES. THIS MEANS NO EXPRESS,
 * IMPLIED OR STATUTORY WARRANTY, INCLUDING WITHOUT LIMITATION, WARRANTIES OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ANY WARRANTY AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE SOFTWARE OR ANY WARRANTY OF TITLE
 * OR NON-INFRINGEMENT. THERE IS NO WARRANTY THAT THIS SOFTWARE WILL FULFILL
 * ANY OF YOUR PARTICULAR PURPOSES OR NEEDS. ALSO, YOU MUST PASS THIS
 * DISCLAIMER ON WHENEVER YOU DISTRIBUTE THE SOFTWARE OR DERIVATIVE WORKS.
 * NEITHER TUM NOR ANY CONTRIBUTOR TO THE SOFTWARE WILL BE LIABLE FOR ANY
 * DAMAGES RELATED TO THE SOFTWARE OR THIS LICENSE AGREEMENT, INCLUDING
 * DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL OR INCIDENTAL DAMAGES, TO THE
 * MAXIMUM EXTENT THE LAW PERMITS, NO MATTER WHAT LEGAL THEORY IT IS BASED ON.
 * ALSO, YOU MUST PASS THIS LIMITATION OF LIABILITY ON WHENEVER YOU DISTRIBUTE
 * THE SOFTWARE OR DERIVATIVE WORKS.
 * 
 * Main authors: Florian Eyben, Felix Weninger, 
 * 	      Martin Woellmer, Bjoern Schuller
 * 
 * Copyright (c) 2008-2013, 
 *   Institute for Human-Machine Communication,
 *   Technische Universitaet Muenchen, Germany
 * 
 * Copyright (c) 2013-2015, 
 *   audEERING UG (haftungsbeschraenkt),
 *   Gilching, Germany
 * 
 * Copyright (c) 2016,	 
 *   audEERING GmbH,
 *   Gilching Germany
 ***************************************************************************E*/


/*  openSMILE component:

vector Processor :  (abstract class only)
       specialised dataProcessor, which takes one frame as input an produces one frame as output
       however, each array field is processed individually as a vector
       for each field the output dimension can be set in derived components

*/


#ifndef __VECTOR_PROCESSOR_HPP
#define __VECTOR_PROCESSOR_HPP

#include <core/smileCommon.hpp>
#include <core/dataProcessor.hpp>

#define COMPONENT_DESCRIPTION_CVECTORPROCESSOR "dataProcessor, where each array field is processed individually as a vector"
#define COMPONENT_NAME_CVECTORPROCESSOR "cVectorProcessor"

#undef class
class DLLEXPORT cVectorProcessor : public cDataProcessor {
  private:
    long Nfi, Nfo, Ni, No;
    long *fNi, *fNo;
    cVector * vecO;

    int includeSingleElementFields;
	  int processArrayFields;
    int preserveFieldNames;
    //mapping of field indicies to config indicies: (size of these array is maximum possible size: Nfi)
    int Nfconf;
    int *fconf, *fconfInv;
    long *confBs;  // blocksize for configurations

    int addFconf(long bs, int field); // return value is index of assigned configuration

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    int getProcessArrayFields() { return processArrayFields; }

    /*
      since a vector processor can process individual fields of a vector seperately,
      these fields might require different configuration and thus different internal variables/parameters.
      this is especially true for things like state variables, histories, etc.
      THUS, you must use the following functions to allocate arrays, holding multiple variants
      of your configuration/state/etc. variables, one for each field.
    */

    // (input) field configuration, may be used in setupNamesForField
    int getFconf(int field) { return fconf[field]; } // caller must check for return value -1 (= no conf available for this field)
    /* get number of fields of input vector (matched number of fields in output vector) */
    int getNf() { 
      if (!processArrayFields) return 1;
      else return reader_->getLevelNf(); // return Nfi; ??? 
    }

    /* free an array of pointers to configuration variables */
    void multiConfFree( void * x );
    /* free only a single value array allocated by multiConfAllocLong/Float() */
    void multiConfFree1D( void * x ) {
      if (x != NULL) free(x);
    }


    /* allocate an array of N (=num. fields=) pointers for storing multiple configurations */
    void *multiConfAlloc() {
      return calloc(1,sizeof(void*)*getNf());
    }
    /* allocate only single value arrays for type long for float */
    long *multiConfAllocLong() {
      return (long*)calloc(1,sizeof(long)*getNf());
    }
    FLOAT_DMEM *multiConfAllocFloat() {
      return (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*getNf());
    }
    
    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    virtual int dataProcessorCustomFinalise();

    //virtual int configureWriter(sDmLevelConfig &c);

    /* cVectorProcessor::myTick calls customVecProcess at the beginning, giving the vector processor a
       chance to access and process the raw vector data.  */
    virtual int customVecProcess(cVector *vecIn, cVector* vecOut) {
      if (vecIn == NULL) {
        // do flushing here if no new input
        // ...
        // return 1;
        // or return that we've done nothing:
        return 0;
      }
      // return value of 1 means that we have processed data,
      // process vector float will not be called in this case!
      // return 1;
      // A return value of 0 means that we have done nothing, or only modified incoming data
      // processVectorFloat on fields etc. will still be called!
      return 0;
    }
    
    //virtual void configureField(int idxi, long __N, long nOut);
    //virtual int setupNamesForField(int i, const char*name, long nEl);

    /* these functions process an input vector and produce an output vector
       by default for each field in the input vector processVectorX will be called
       with the field index in idxi, the field data in *src, the number of elements in the field in Nsrc 
       in this case for each input field a corresponding output field exists
       IF the option processArrayFields is set to 0, then the full input and output vector is passed
       to processVectorX and processVectorX is only called once per vector (idxi = 0)*/
    virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

    /* these methods are called at the end of processing (end-of-input) to allow the component
       to flush data, save final results to files, etc. 
       you should also use these functions for components that collect data/statistics over the
       full input, instead of doing incremental processing */
    virtual int flushVectorInt(INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int flushVectorFloat(FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    
  public:
    SMILECOMPONENT_STATIC_DECL
    
    cVectorProcessor(const char *_name);

    virtual ~cVectorProcessor();
};




#endif // __VECTOR_PROCESSOR_HPP
