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

LPC, compute LPC coefficients from wave data (PCM) frames

*/


#ifndef __CFORMANTLPC_HPP
#define __CFORMANTLPC_HPP

#include <core/smileCommon.hpp>
#include <core/vectorProcessor.hpp>

#define BUILD_COMPONENT_FormantLpc
#define COMPONENT_DESCRIPTION_CFORMANTLPC "This component computes formant frequencies and bandwidths by solving for the roots of the LPC polynomial. The formant trajectories can and should be smoothed by the cFormantSmoother component."
#define COMPONENT_NAME_CFORMANTLPC "cFormantLpc"

#undef class
class DLLEXPORT cFormantLpc : public cVectorProcessor {
  private:
    int nFormants;
    int saveFormants;
    int saveIntensity;
    int saveBandwidths;
    int saveNumberOfValidFormants;
    int useLpSpec;
    int nSmooth;
    int medianFilter, octaveCorrection;

    int nLpc;
    long lpcCoeffIdx;
    long lpcGainIdx;
    long lpSpecIdx, lpSpecN;

    double minF, maxF;
    
    double T;
    double *lpc, *roots;
    double *formant, *bandwidth;

    FLOAT_DMEM calcFormantLpc(const FLOAT_DMEM *x, long Nsrc, FLOAT_DMEM * lpc, long nCoeff, FLOAT_DMEM *refl);
  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    virtual int setupNewNames(long nEl);
    virtual void findInputFields();

    virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cFormantLpc(const char *_name);

    virtual ~cFormantLpc();
};

#endif // __CFORMANTLPC_HPP
