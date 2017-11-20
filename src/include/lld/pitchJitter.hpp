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

example for dataProcessor descendant

*/


#ifndef __CPITCHJITTER_HPP
#define __CPITCHJITTER_HPP

#include <core/smileCommon.hpp>
#include <smileutil/smileUtil.h>
#include <core/dataProcessor.hpp>

#define BUILD_COMPONENT_PitchJitter
#define COMPONENT_DESCRIPTION_CPITCHJITTER "This component computes Voice Quality parameters Jitter (pitch period deviations) and Shimmer (pitch period amplitude deviations). It requires the raw PCM frames and the corresponding fundamental frequency (F0) as inputs."
#define COMPONENT_NAME_CPITCHJITTER "cPitchJitter"

#undef class
class DLLEXPORT cPitchJitter : public cDataProcessor {
  private:
    int onlyVoiced;
    long savedMaxDebugPeriod;
    int jitterLocal, jitterDDP, shimmerLocal, shimmerLocalDB;
    int jitterLocalEnv, jitterDDPEnv, shimmerLocalEnv, shimmerLocalDBEnv;
    int shimmerUseRmsAmplitude;
    int harmonicERMS, noiseERMS, linearHNR, logHNR;
    int sourceQualityRange, sourceQualityMean;
    int useBrokenJitterThresh_;

    int periodLengths, periodStarts;
    long F0fieldIdx;
    long lastIdx, lastMis;
    int refinedF0;
    int usePeakToPeakPeriodLength_;
    FLOAT_DMEM threshCC_;
    int minNumPeriods;

    FILE *filehandle;
    long input_max_delay_;
    double searchRangeRel;
    double minF0;
    const char * F0field;
    cDataReader *F0reader;
    cVector *out;
    long Nout;

    // jitter data memory:
    FLOAT_DMEM lastT0;
    FLOAT_DMEM lastDiff;
    FLOAT_DMEM lastJitterDDP, lastJitterLocal;
    FLOAT_DMEM lastJitterDDP_b, lastJitterLocal_b;

    // shimmer data memory:
    FLOAT_DMEM lastShimmerLocal;
    FLOAT_DMEM lastShimmerLocal_b;

    FLOAT_DMEM lgHNRfloor;

    double crossCorr(FLOAT_DMEM *x, long Nx, FLOAT_DMEM *y, long Ny);
    FLOAT_DMEM amplitudeDiff(FLOAT_DMEM *x, long Nx, FLOAT_DMEM *y, long Ny,
        double *maxI0, double *maxI1, FLOAT_DMEM *_A0, FLOAT_DMEM *_A1);
    FLOAT_DMEM rmsAmplitudeDiff(FLOAT_DMEM *x, long Nx, FLOAT_DMEM *y, long Ny,
        double *maxI0, double *maxI1, FLOAT_DMEM *_A0, FLOAT_DMEM *_A1);
    void saveDebugPeriod(long sample, double sampleInterp);

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    virtual void mySetEnvironment();
    virtual int myRegisterInstance(int *runMe=NULL);
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    // virtual int dataProcessorCustomFinalise();
    virtual int setupNewNames(long nEl);
    // virtual int setupNamesForField();
    virtual int configureReader(const sDmLevelConfig &c);
    virtual int configureWriter(sDmLevelConfig &c);


  public:
    SMILECOMPONENT_STATIC_DECL
    
    cPitchJitter(const char *_name);

    virtual ~cPitchJitter();
};




#endif // __CPITCHJITTER_HPP
