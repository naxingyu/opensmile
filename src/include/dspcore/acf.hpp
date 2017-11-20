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
