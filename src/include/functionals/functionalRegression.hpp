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

functionals: linear and quadratic regression coefficients

*/


#ifndef __CFUNCTIONALREGRESSION_HPP
#define __CFUNCTIONALREGRESSION_HPP

#include <core/smileCommon.hpp>
#include <core/dataMemory.hpp>
#include <functionals/functionalComponent.hpp>

#define COMPONENT_DESCRIPTION_CFUNCTIONALREGRESSION "  linear and quadratic regression coefficients and corresponding linear and quadratic regression errors. Linear regression line: y = m*x + t ; quadratic regression parabola: y = a*x^2 + b*x + c . Algorithm used: Minimum mean square error, direct analytic solution. This component also computes the centroid of the contour."
#define COMPONENT_NAME_CFUNCTIONALREGRESSION "cFunctionalRegression"

#undef class
class DLLEXPORT cFunctionalRegression : public cFunctionalComponent {
  private:
    int enQreg;
    int oldBuggyQerr;
    int normRegCoeff;
    int normInputs;
    int centroidNorm;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    virtual void fetchConfig();

    int doRatioLimit_;
    int centroidRatioLimit_;
    int centroidUseAbsValues_;

    // soft limit to [-10;10] (linear) and non-linear hard-limit
    // to [-20;20] for inputs [-inf;+inf]
    FLOAT_DMEM ratioLimit(FLOAT_DMEM x) {
      if (doRatioLimit_) {
        return smileMath_ratioLimit(x, 10.0, 10.0);
      }
      return x;
    }
    FLOAT_DMEM ratioLimitMax(FLOAT_DMEM altValue) {
      if (doRatioLimit_) {
        return 20.0;  // sum of limit1 + limit2 in above function
      }
      return altValue;
    }

    // limit to -1..+1
    FLOAT_DMEM ratioLimitUnity(FLOAT_DMEM x) {
      if (doRatioLimit_) {
        if (x > (FLOAT_DMEM)1.0)
          return (FLOAT_DMEM)1.0;
        if (x < (FLOAT_DMEM)-1.0)
          return (FLOAT_DMEM)-1.0;
      } 
      return x;
    }

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cFunctionalRegression(const char *_name);
    // inputs: sorted and unsorted array of values, out: pointer to space in output array, You may not return MORE than Nout elements, please return as return value the number of actually computed elements (usually Nout)
    virtual long process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM min, FLOAT_DMEM max, FLOAT_DMEM mean, FLOAT_DMEM *out, long Nin, long Nout);
    //virtual long process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout);

    virtual long getNoutputValues() { return nEnab; }
    virtual int getRequireSorted() { return 0; }

    virtual ~cFunctionalRegression();
};




#endif // __CFUNCTIONALREGRESSION_HPP
