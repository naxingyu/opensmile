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
