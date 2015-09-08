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

functionals: percentiles and quartiles, and inter-percentile/quartile ranges

*/


#ifndef __CFUNCTIONALPERCENTILES_HPP
#define __CFUNCTIONALPERCENTILES_HPP

#include <core/smileCommon.hpp>
#include <core/dataMemory.hpp>
#include <functionals/functionalComponent.hpp>

#define COMPONENT_DESCRIPTION_CFUNCTIONALPERCENTILES "  percentile values and inter-percentile ranges (including quartiles, etc.). This component sorts the input array and then chooses the value at the index closest to p*buffer_len for the p-th percentile (p=0..1)."
#define COMPONENT_NAME_CFUNCTIONALPERCENTILES "cFunctionalPercentiles"

#undef class
class DLLEXPORT cFunctionalPercentiles : public cFunctionalComponent {
  private:
    int nPctl, nPctlRange;
    double *pctl;
    int *pctlr1, *pctlr2;
    char *tmpstr;
    int quickAlgo, interp;
    long varFctIdx;

    long getPctlIdx(double p, long N);
    FLOAT_DMEM getInterpPctl(double p, FLOAT_DMEM *sorted, long N);
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    virtual void fetchConfig();

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cFunctionalPercentiles(const char *_name);
    // inputs: sorted and unsorted array of values, out: pointer to space in output array, You may not return MORE than Nout elements, please return as return value the number of actually computed elements (usually Nout)
    virtual long process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM *out, long Nin, long Nout);
    //virtual long process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout);

    //virtual long getNoutputValues() { return nEnab; }
    virtual const char* getValueName(long i);
    virtual int getRequireSorted() { if (quickAlgo) return 0; else return 1; }

    virtual ~cFunctionalPercentiles();
};




#endif // __CFUNCTIONALPERCENTILES_HPP
