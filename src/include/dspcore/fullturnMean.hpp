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

computes mean of full input ?

*/


#ifndef __CFULLTURNMEAN_HPP
#define __CFULLTURNMEAN_HPP

#pragma warning( disable : 4251 )

#include <core/smileCommon.hpp>
#include <core/vectorProcessor.hpp>

// STL includes for the queue
#include <queue>


#define COMPONENT_DESCRIPTION_CFULLTURNMEAN "This component performs mean normalizing on a data series. A 2-pass analysis of the data is performed, which makes this component unusable for on-line analysis. In the first pass, no output is produced and the mean value (over time) is computed for each input element. In the second pass the mean vector is subtracted from all input frames, and the result is written to the output dataMemory level. Attention: Due to the 2-pass processing the input level must be large enough to hold the whole data sequence."
#define COMPONENT_NAME_CFULLTURNMEAN "cFullturnMean"


#undef class

// A message received from the turn detector.
struct TurnTimeMsg {
  TurnTimeMsg() : vIdxStart(0), vIdxEnd(0), isForcedTurnEnd(0) {}
  TurnTimeMsg(long _vIdxStart, long _vIdxEnd, int forceend=0) : vIdxStart(_vIdxStart), vIdxEnd(_vIdxEnd), isForcedTurnEnd(forceend) {}
  long vIdxStart, vIdxEnd;        // vector index
  int isForcedTurnEnd;  // forced turn end = at end of input
};

// A queue of turn detector messages.
typedef std::queue<TurnTimeMsg> TurnTimeMsgQueue;

class DLLEXPORT cFullturnMean : public cDataProcessor {
  private:
    TurnTimeMsgQueue msgQue;

    int dataInQue;
    long curWritePos;
    //TODO: postSil / preSil option for adding extra data at beginning and end of turn

    const char *msgRecp;
    int eNormMode;
    cVector *means;
    long nMeans;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    int checkMessageQueque(long &start, long &end, long &fte);
    virtual int processComponentMessage(cComponentMessage *_msg);
    //virtual int configureWriter(sDmLevelConfig &c);

    //virtual void configureField(int idxi, long __N, long nOut);
    //virtual int setupNamesForField(int i, const char*name, long nEl);
    //virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    //virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cFullturnMean(const char *_name);

    virtual ~cFullturnMean();
};




#endif // __CFULLTURNMEAN_HPP
