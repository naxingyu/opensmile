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

a single statistical functional or the like....
(template class)

*/


#ifndef __CFUNCTIONAL_HPP
#define __CFUNCTIONAL_HPP

#include <core/smileCommon.hpp>
#include <core/dataMemory.hpp>
#include <core/dataWriter.hpp>

#define COMPONENT_DESCRIPTION_CFUNCTIONALCOMPONENT "This is an abstract functional class, it is the base for all functional extractor classes."
#define COMPONENT_NAME_CFUNCTIONALCOMPONENT "cFunctionalComponent"

#define TIMENORM_UNDEFINED   -1
#define TIMENORM_SEGMENT   0
#define TIMENORM_SECOND    1
#define TIMENORM_SECONDS   1
#define TIMENORM_FRAME     2
#define TIMENORM_FRAMES    2
#define TIMENORM_SAMPLE    3  // currently not implemented
#define TIMENORM_SAMPLES   3  // currently not implemented
#define _TIMENORM_MAX      3

// obsolete: please use TIMENORM_XXX constants in new code..!
#define NORM_TURN     0
#define NORM_SECOND   1
#define NORM_FRAME    2
#define NORM_SAMPLES  3


#undef class
class DLLEXPORT cFunctionalComponent : public cSmileComponent {
  private:
    double T;
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    int nEnab, nTotal;
    int *enab;
    int timeNorm, timeNormIsSet;
    const char **functNames;
    
    virtual void fetchConfig();
    // get size of one input element/frame in seconds
    double getInputPeriod() {
      return T;
    }
    // parse a custom 'norm' (timeNorm) option if set (call this from fetchConfig)
    void parseTimeNormOption();

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cFunctionalComponent(const char *_name, int _nTotal=0, const char *_names[]=NULL);
    void setInputPeriod(double _T) { T=_T; }

    void setTimeNorm(int _norm) { 
      if (!timeNormIsSet) {
        if ((_norm >= 0)&&(_norm <= _TIMENORM_MAX)&&(_norm!=TIMENORM_UNDEFINED)) {
          timeNorm = _norm;
        }
      }
    }

    // inputs: sorted and unsorted array of values, out: pointer to space in output array,
    // You may not return MORE than Nout elements, please return as return value the number
    // of actually computed elements (usually Nout)
    virtual long process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM *out, long Nin, long Nout);
    virtual long process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM min,
        FLOAT_DMEM max, FLOAT_DMEM mean, FLOAT_DMEM *out, long Nin, long Nout)
    {
      return process(in,inSorted,out,Nin,Nout);
    }
    
    virtual long process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout);
    virtual long process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM min,
        INT_DMEM max, INT_DMEM mean, INT_DMEM *out, long Nin, long Nout)
    {
      return process(in,inSorted,out,Nin,Nout);
    }

    virtual void setFieldMetaData(cDataWriter *writer,
        const FrameMetaInfo *fmeta, int idxi, long nEl);
    virtual long getNoutputValues() { return nEnab; }
    virtual long getNumberOfElements(long j) { return 1; }
    virtual const char* getValueName(long i);
    virtual int getRequireSorted() { return 0; }

    virtual ~cFunctionalComponent();
};




#endif // __CFUNCTIONAL_HPP
