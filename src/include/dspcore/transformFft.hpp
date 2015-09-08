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

fast fourier transform using fft4g library
output: complex values of fft or real signal values (for iFFT)

*/


#ifndef __TRANSFORM_FFT_HPP
#define __TRANSFORM_FFT_HPP

#include <core/smileCommon.hpp>
#include <core/vectorProcessor.hpp>
#include <dspcore/fftXg.h>

#define COMPONENT_DESCRIPTION_CTRANSFORMFFT "This component performs an FFT on a sequence of real values (one frame), the output is the complex domain result of the transform. Use the cFFTmagphase component to compute magnitudes and phases from the complex output."
#define COMPONENT_NAME_CTRANSFORMFFT "cTransformFFT"

#undef class
class DLLEXPORT cTransformFFT : public cVectorProcessor {
  private:
    int inverse;
//    int zeroPad;
    int **ip;
    FLOAT_TYPE_FFT **w;
    int newFsSet;
    double frameSizeSec_out;

    // generate "frequency axis information", i.e. the frequency in Hz for each spectral bin
    // which is to be saved as meta-data in the dataMemory level field (FrameMetaInfo->FieldMetaInfo->info)
    // &infosize is initialized with the number of fft bins x 2 (= number of input samples)
    //   and should contain the number of complex bins at the end of this function
    void * generateSpectralVectorInfo(long &infosize);

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    virtual int myFinaliseInstance();

    virtual int configureWriter(sDmLevelConfig &c);
    virtual int setupNamesForField(int i, const char*name, long nEl);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);


  public:
    SMILECOMPONENT_STATIC_DECL
    
    cTransformFFT(const char *_name);

    virtual ~cTransformFFT();
};




#endif // __TRANSFORM_FFT_HPP
