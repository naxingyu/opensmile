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
