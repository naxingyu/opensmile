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

simple preemphasis : x(t) = x(t) - k*x(t-1)

*/


#ifndef __CSMILERESAMPLE_HPP
#define __CSMILERESAMPLE_HPP

#include <core/smileCommon.hpp>
#include <core/dataProcessor.hpp>

#define BUILD_COMPONENT_SmileResample
#define COMPONENT_DESCRIPTION_CSMILERESAMPLE "This component implements a spectral domain resampling component. Input frames are transferred to the spectral domain, then the spectra are shifted, and a modified DFT is performed to synthesize samples at the new rate."
#define COMPONENT_NAME_CSMILERESAMPLE "cSmileResample"


#undef class
class DLLEXPORT cSmileResample : public cDataProcessor {
  private:
    cMatrix *matnew; cMatrix *rowout;
    cMatrix *row;
    int flushed;
    int useQuickAlgo;

    double ND;
    double resampleRatio, targetFs, pitchRatio;
    double winSize, winSizeTarget;
    long winSizeFramesTarget, winSizeFrames;

    FLOAT_DMEM *outputBuf, *lastOutputBuf;
    FLOAT_TYPE_FFT *inputBuf;
    sResampleWork *resampleWork;
    long Ni;

    int getOutput(FLOAT_DMEM *cur, FLOAT_DMEM *last, long N, FLOAT_DMEM *out, long Nout);

  protected:
    SMILECOMPONENT_STATIC_DECL_PR


    virtual void fetchConfig();
/*
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    
*/

    virtual int myTick(long long t);
    virtual int configureWriter(sDmLevelConfig &c);

   // buffer must include all (# order) past samples
    //virtual int processBuffer(cMatrix *_in, cMatrix *_out, int _pre, int _post );
    virtual int dataProcessorCustomFinalise();
/*
    virtual int setupNamesForField(int i, const char*name, long nEl);
    virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
*/
    //virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

    
  public:
    SMILECOMPONENT_STATIC_DECL
    
    cSmileResample(const char *_name);

    virtual ~cSmileResample();
};




#endif // __CSMILERESAMPLE_HPP
