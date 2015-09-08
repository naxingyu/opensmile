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

    int periodLengths, periodStarts;
    long F0fieldIdx;
    long lastIdx, lastMis;
    int refinedF0;
    int usePeakToPeakPeriodLength_;
    FLOAT_DMEM minCC;
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
