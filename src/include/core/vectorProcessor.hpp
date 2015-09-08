/*F***************************************************************************
 * openSMILE - the open-Source Multimedia Interpretation by Large-scale
 * feature Extraction toolkit
 * 
 * (c) 2008-2011, Florian Eyben, Martin Woellmer, Bjoern Schuller: TUM-MMK
 * 
 * (c) 2012-2013, Florian Eyben, Felix Weninger, Bjoern Schuller: TUM-MMK
 * 
 * (c) 2013-2014 audEERING UG, haftungsbeschränkt. All rights reserved.
 * 
 * Any form of commercial use and redistribution is prohibited, unless another
 * agreement between you and audEERING exists. See the file LICENSE.txt in the
 * top level source directory for details on your usage rights, copying, and
 * licensing conditions.
 * 
 * See the file CREDITS in the top level directory for information on authors
 * and contributors. 
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
