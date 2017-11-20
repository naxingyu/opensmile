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
    int inverse_;
    int **ip_;
    FLOAT_TYPE_FFT **w_;
    FLOAT_TYPE_FFT **xconv_;
    int newFsSet_;
    double frameSizeSecOut_;
    int zeroPadSymmetric_;

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
