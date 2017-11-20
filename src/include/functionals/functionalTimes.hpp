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

functionals: rise/fall times, up/down-level times

*/

#ifndef __CFUNCTIONALTIMES_HPP
#define __CFUNCTIONALTIMES_HPP

#include <core/smileCommon.hpp>
#include <core/dataMemory.hpp>
#include <functionals/functionalComponent.hpp>

#define COMPONENT_DESCRIPTION_CFUNCTIONALTIMES "  up- and down-level times + rise and fall, left- and right-curve times, duration, etc."
#define COMPONENT_NAME_CFUNCTIONALTIMES "cFunctionalTimes"

#undef class
class DLLEXPORT cFunctionalTimes : public cFunctionalComponent {
  private:
    int nUltime, nDltime;
    double *ultime, *dltime;
    char *tmpstr;
    int varFctIdx;
    int buggySecNorm;
    int useRobustPercentileRange_;
    FLOAT_DMEM pctlRangeMargin_;
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    virtual void fetchConfig();
    FLOAT_DMEM getPctlMin(FLOAT_DMEM *sorted, long N);
    FLOAT_DMEM getPctlMax(FLOAT_DMEM *sorted, long N);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cFunctionalTimes(const char *_name);
    // inputs: sorted and unsorted array of values, out: pointer to space in output array, You may not return MORE than Nout elements, please return as return value the number of actually computed elements (usually Nout)
    virtual long process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM min, FLOAT_DMEM max, FLOAT_DMEM mean, FLOAT_DMEM *out, long Nin, long Nout);
    //virtual long process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout);

    //virtual long getNoutputValues() { return nEnab; }
    virtual const char* getValueName(long i);
    virtual int getRequireSorted() {
      if (useRobustPercentileRange_)
        return 1;
      return 0;
    }

    virtual ~cFunctionalTimes();
};




#endif // __CFUNCTIONALTIMES_HPP
