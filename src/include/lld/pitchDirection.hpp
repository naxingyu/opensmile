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


#ifndef __CPITCHDIRECTION_HPP
#define __CPITCHDIRECTION_HPP

#include <core/smileCommon.hpp>
#include <core/dataProcessor.hpp>

#define BUILD_COMPONENT_PitchDirection
#define COMPONENT_DESCRIPTION_CPITCHDIRECTION "This component reads pitch data, detects pseudo syllables, and computes pitch direction estimates per syllable. Thereby the classes falling, flat, and rising are distinguished. \n    Required input fields: F0, F0env, and 'loudness' or 'RMSenergy'."
#define COMPONENT_NAME_CPITCHDIRECTION "cPitchDirection"

#undef class
class DLLEXPORT cPitchDirection : public cDataProcessor {
  private:
    cVector *myVec;
    long F0field, F0envField, LoudnessField, RMSField;
    double stbs, ltbs;
    long stbsFrames, ltbsFrames;
    FLOAT_DMEM * stbuf, * ltbuf;
    FLOAT_DMEM F0non0, lastF0non0, f0s;
    long stbufPtr, ltbufPtr; /* ring-buffer pointers */
    long bufInit; /* indicates wether buffer has been filled and valid values are to be expected */
    double ltSum, stSum;
    FLOAT_DMEM longF0Avg;
    long nFall,nRise,nFlat;

    int insyl;
    int f0cnt;
    FLOAT_DMEM lastE;
    FLOAT_DMEM startE, maxE, minE, endE;
    long sylen, maxPos, minPos, sylenLast;
    long sylCnt;
    double inpPeriod, timeCnt, lastSyl;

    FLOAT_DMEM startF0, lastF0, maxF0, minF0;
    long maxF0Pos, minF0Pos;

    const char *directionMsgRecp;
    int speakingRateBsize;

    int F0directionOutp, directionScoreOutp;
    int speakingRateOutp;
    int F0avgOutp, F0smoothOutp;

    /* speaking rate variables (buf0 is always the first half of buf1) */
    int nBuf0,nBuf1; /* number of frames collected in bufferA and bufferB */
    int nSyl0,nSyl1; /* number of syllable starts in bufferA and bufferB */
    double curSpkRate;

    int nEnabled;
    
    int isTurn, onlyTurn, invertTurn;
    int resetTurnData;
    const char * turnStartMessage, * turnEndMessage;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    virtual void sendPitchDirectionResult(int result, double _smileTime, const char * _directionMsgRecp);

    virtual int processComponentMessage( cComponentMessage *_msg );

    // virtual int dataProcessorCustomFinalise();
     virtual int setupNewNames(long nEl);
    // virtual int setupNamesForField();
    virtual int configureWriter(sDmLevelConfig &c);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cPitchDirection(const char *_name);

    virtual ~cPitchDirection();
};




#endif // __CPITCHDIRECTION_HPP
