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

simple silence threshold based turn detector

*/


#ifndef __CTURNDETECTOR_HPP
#define __CTURNDETECTOR_HPP

#include <core/smileCommon.hpp>
#include <core/dataProcessor.hpp>

#define COMPONENT_DESCRIPTION_CTURNDETECTOR "Speaker turn detector using data from cVadV1 component or cSemaineSpeakerID1 (adaptive VAD) to determine speaker turns and identify continuous segments of voice activity."
#define COMPONENT_NAME_CTURNDETECTOR "cTurnDetector"


#undef class
class DLLEXPORT cTurnDetector : public cDataProcessor {
  private:
    FLOAT_DMEM threshold, threshold2;
    int nPost, nPre;
    int useRMS;
    int turnState, actState;
    long startP, startP0, endP0;
    double endSmileTime, startSmileTime, startSmileTimeS;
    double turnTime, turnStep, msgInterval;
    double maxTurnLengthS, graceS;
    long maxTurnLength, grace;
    long minTurnLengthFrames_, minTurnLengthFrameTimeFrames_;
    double minTurnLength_, minTurnLengthTurnFrameTimeMessage_;

    int blockAll, blockStatus;
    int blockTurn, unblockTurnCntdn, unblockTimeout;
    double initialBlockTime;
    long initialBlockFrames;

    int terminateAfterTurns, terminatePostSil, nTurns;
    int exitFlag;
    long nSilForExit;

    long eoiMis;
    int forceEnd;
    int timeout; double lastDataTime;
    double timeoutSec;

    int debug;
    int cnt1, cnt2, cnt1s, cnt2s;

    long rmsIdx, autoRmsIdx, readVad;
    int autoThreshold;
    int invert_;

    // variables for auto threshold statistics:
    int nmin, nmax;
    FLOAT_DMEM minmaxDecay;
    FLOAT_DMEM rmin, rmax, rmaxlast;
    FLOAT_DMEM tmpmin, tmpmax;
    FLOAT_DMEM dtMeanO, dtMeanT, dtMeanS;
    FLOAT_DMEM alphaM;
    FLOAT_DMEM nE, dtMeanAll;
    long nTurn, nSil;
    int tmpCnt;
    int calCnt;

    const char *recFramer, *recComp, *statusRecp;
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual int isVoice(FLOAT_DMEM *src, int state=0);  /* state 0: silence/noise, state 1: voice */
    virtual void updateThreshold(FLOAT_DMEM eRmsCurrent);

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    virtual int myTick(long long t);
    virtual int processComponentMessage( cComponentMessage *_msg );

    //virtual int configureWriter(sDmLevelConfig &c);
    virtual int setupNewNames(long nEl);
    virtual void resumeEvent() {
      lastDataTime = getSmileTime();
    }
    //virtual void configureField(int idxi, long __N, long nOut);
    //virtual int setupNamesForField(int i, const char*name, long nEl);
//    virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
//    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cTurnDetector(const char *_name);

    virtual ~cTurnDetector();
};




#endif // __CTURNDETECTOR_HPP
