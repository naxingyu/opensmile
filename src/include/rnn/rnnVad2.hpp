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

BLSTM RNN processor

*/


#ifndef __CRNNVAD2_HPP
#define __CRNNVAD2_HPP

#include <core/smileCommon.hpp>
#include <core/dataProcessor.hpp>
#include <rnn/rnn.hpp>

#define COMPONENT_DESCRIPTION_CRNNVAD2 "BLSTM RNN processor."
#define COMPONENT_NAME_CRNNVAD2 "cRnnVad2"

#undef class


/* ring buffer average computation over holdTime + decayTime segments */
/* hold of max over holdTime, decay over decayTime */
class cEavgHold 
{
private:
  /* moving average */
  FLOAT_DMEM *Ebuf;
  long EbufSize, nAvg;
  long EbufPtr, EbufPtrOld;
  double EavgSumCur;
  FLOAT_DMEM EmaxRiseAlpha; /* alpha for exponential rise smoothing of max energy: 1= rise immediately, 0=do not follow input */

  /* envelope */
  FLOAT_DMEM Emax;
  long EmaxAge;
  FLOAT_DMEM EmaxDecayStep;

  int holdTime;
  int decayTime;

public:
  /* holdTime and decayTime in frames/samples */
  cEavgHold(int _holdTime, int _decayTime, int _decayFunct=0, FLOAT_DMEM _EmaxRiseAlpha=0.2) :
      holdTime(_holdTime),
      decayTime(_decayTime),
      EmaxRiseAlpha(_EmaxRiseAlpha)
  {
    if (decayTime <= 0) decayTime = 1;
    if (holdTime <= 0) holdTime = 1;

    EbufSize = holdTime + decayTime;
    EbufPtr = 0;
    EbufPtrOld = 0;
    Ebuf = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*EbufSize);
    EavgSumCur = 0.0; nAvg = 0;
    Emax = 0.0;
    EmaxAge = 0;
    EmaxDecayStep = 0.0;
  }

  /* add next input to buffer */
  void nextE(FLOAT_DMEM E)
  {
    /* average: */
    EavgSumCur -= Ebuf[EbufPtrOld];
    Ebuf[EbufPtr] = E;
    EbufPtrOld = EbufPtr++;
    EavgSumCur += E;
    if (EbufPtr >= EbufSize) EbufPtr %= EbufSize;
    if (nAvg < EbufSize) nAvg++;
    /* envelope: */
    if (E > Emax) {
      Emax = Emax*((FLOAT_DMEM)1.0-EmaxRiseAlpha) + E*EmaxRiseAlpha;
      EmaxDecayStep = Emax/(FLOAT_DMEM)decayTime;
      EmaxAge = 0;
    } else {
      EmaxAge++;
      if (EmaxAge > holdTime && EmaxAge < holdTime+decayTime && Emax > EmaxDecayStep) {
        Emax -= EmaxDecayStep;
      }
    }
  }

  /* get the current short-term average */
  FLOAT_DMEM getAvg() 
  {
    if (nAvg > 100 || nAvg >= EbufSize) {
      return (FLOAT_DMEM) ( EavgSumCur / (double)nAvg ) ;
    } else { return 0.0; }
  }

  /* get the current envelope (max. hold) */
  FLOAT_DMEM getEnv() 
  {
    if (nAvg > 100  || nAvg >= EbufSize) return Emax;
    else return 0.0;
  }

  ~cEavgHold()
  {
    if (Ebuf != NULL) free(Ebuf);
  }
};


class DLLEXPORT cRnnVad2 : public cDataProcessor {
  private:
    cEavgHold * eUser;
    cEavgHold * eCurrent;
    cEavgHold * eAgent;
    cEavgHold * eBg;
    
    long voiceIdx, agentIdx, energyIdx, f0Idx;

    long cnt; 
    int isV;
    int vadDebug, allowEoverride;
    cVector *frameO;
    
    FLOAT_DMEM voiceThresh, agentThresh;
    
    //long agentBlockTime;
    long agentTurnCntdn;
    long agentTurnPastBlock;
    int alwaysRejectAgent, smartRejectAgent;
    int doReset, agentTurn, userPresence;
    long userCnt, agentCntdn;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);
    virtual int processComponentMessage( cComponentMessage *_msg );

    virtual int setupNewNames(long nEl);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cRnnVad2(const char *_name);

    virtual ~cRnnVad2();
};




#endif // __CRNNVAD2_HPP
