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

Autocorrelation Function (ACF)

*/


#ifndef __CACF_HPP
#define __CACF_HPP

#include <core/smileCommon.hpp>
#include <core/vectorProcessor.hpp>
#include <dspcore/fftXg.h>
#include <math.h>

#define COMPONENT_DESCRIPTION_CACF "This component computes the autocorrelation function (ACF) by sqauring a magnitude spectrum and applying an inverse Fast Fourier Transform. This component mus read from a level containing *only* FFT magnitudes in a single field. Use the 'cTransformFFT' and 'cFFTmagphase' components to compute the magnitude spectrum from PCM frames. Computation of the Cepstrum is also supported (this applies a log() function to the magnitude spectra)."
#define COMPONENT_NAME_CACF "cAcf"

#undef class
class DLLEXPORT cAcf : public cVectorProcessor {
  private:
    int absCepstrum_;
    int oldCompatCepstrum_;
    int acfCepsNormOutput_;
    int symmetricData;
    int expBeforeAbs;
    int cosLifterCepstrum;
    int usePower, cepstrum, inverse;
    FLOAT_TYPE_FFT **data;
    FLOAT_TYPE_FFT **w;
    FLOAT_DMEM **winFunc;
    int **ip;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
//    virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    //virtual int myTick(long long t);

    virtual int dataProcessorCustomFinalise();

//    virtual void configureField(int idxi, long __N, long nOut);
    virtual int setupNamesForField(int i, const char*name, long nEl);
    //virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cAcf(const char *_name);

    virtual ~cAcf();
};




#endif // __CACF_HPP
