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
    int lastVIdx;
    double lastVTime;
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
    double minTurnLength_;
    double minTurnLengthTurnFrameTimeMessage_;

    double turnFrameTimePreRollSec_;
    long turnFrameTimePreRollFrames_;
    double turnFrameTimePostRollSec_;
    long turnFrameTimePostRollFrames_;
    double msgPeriodicMaxLengthSec_;
    long msgPeriodicMaxLengthFrames_;
    int sendTurnFrameTimeMessageAtEnd_;
  
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
