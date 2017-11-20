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
