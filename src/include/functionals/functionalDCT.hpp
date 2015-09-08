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

number of segments based on delta thresholding

*/


#ifndef __CFUNCTIONALDCT_HPP
#define __CFUNCTIONALDCT_HPP

#include <core/smileCommon.hpp>
#include <core/dataMemory.hpp>
#include <functionals/functionalComponent.hpp>

#define COMPONENT_DESCRIPTION_CFUNCTIONALDCT "  DCT coefficients"
#define COMPONENT_NAME_CFUNCTIONALDCT "cFunctionalDCT"

#undef class
class DLLEXPORT cFunctionalDCT : public cFunctionalComponent {
private:
  int firstCoeff, lastCoeff;
  int nCo;
  long N;
  long costableNin;
  FLOAT_DMEM * costable;
  FLOAT_DMEM factor;
  char *tmpstr;

protected:
  SMILECOMPONENT_STATIC_DECL_PR
    virtual void fetchConfig();
  virtual void initCostable(long Nin, long Nout);

public:
  SMILECOMPONENT_STATIC_DECL

    cFunctionalDCT(const char *_name);
  // inputs: sorted and unsorted array of values, out: pointer to space in output array, You may not return MORE than Nout elements, please return as return value the number of actually computed elements (usually Nout)
  virtual long process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM *out, long Nin, long Nout);
  //virtual long process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout);
  virtual const char * getValueName(long i);

  virtual long getNoutputValues() { return nEnab; }
  virtual int getRequireSorted() { return 0; }

  virtual ~cFunctionalDCT();
};




#endif // __CFUNCTIONALDCT_HPP
