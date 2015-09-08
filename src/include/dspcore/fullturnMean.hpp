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
