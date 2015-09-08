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


#ifndef __CLPC_HPP
#define __CLPC_HPP


#include <core/smileCommon.hpp>
#include <core/vectorProcessor.hpp>

#include <dspcore/fftXg.h>

#define BUILD_COMPONENT_Lpc
#define COMPONENT_DESCRIPTION_CLPC "This component computes linear predictive coding (LPC) coefficients from PCM frames. Burg's algorithm and the standard ACF/Durbin based method are implemented for LPC coefficient computation. The output of LPC filter coefficients, reflection coefficients, residual signal, and LP spectrum is supported."

#define COMPONENT_NAME_CLPC "cLpc"

#define LPC_METHOD_ACF   0
#define LPC_METHOD_BURG   5


#undef class
class DLLEXPORT cLpc : public cVectorProcessor {
  private:
    int p;
    int saveLPCoeff, saveRefCoeff;
    int residual;
    int residualGainScale;
    int method;
    int lpGain;
    int forwardRes;
    int lpSpectrum;
    int forwardLPspec;
    double lpSpecDeltaF;
    int lpSpecBins;

    FLOAT_DMEM forwardLPspecFloor;
    FLOAT_DMEM *latB;
    FLOAT_DMEM lastGain;
    FLOAT_TYPE_FFT *lSpec;

    int *_ip;
    FLOAT_TYPE_FFT *_w;

    FLOAT_DMEM *acf;
    FLOAT_DMEM *lpCoeff, *lastLpCoeff, *refCoeff;

    FLOAT_DMEM *gbb, *gb2, *gaa;

    FLOAT_DMEM calcLpc(const FLOAT_DMEM *x, long Nsrc, FLOAT_DMEM * lpc, long nCoeff, FLOAT_DMEM *refl);

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    virtual int setupNamesForField(int i, const char*name, long nEl);

    virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cLpc(const char *_name);

    virtual ~cLpc();
};


#endif // __CLPC_HPP
