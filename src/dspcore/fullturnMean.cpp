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


*/


#include <dspcore/fullturnMean.hpp>

#define MODULE "cFullturnMean"

SMILECOMPONENT_STATICS(cFullturnMean)

SMILECOMPONENT_REGCOMP(cFullturnMean)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CFULLTURNMEAN;
  sdescription = COMPONENT_DESCRIPTION_CFULLTURNMEAN;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")
  
    SMILECOMPONENT_IFNOTREGAGAIN( {}
    //ct->setField("expandFields", "expand fields to single elements, i.e. each field in the output will correspond to exactly one element in the input [not yet implemented]", 0);
    //ct->setField("htkcompatible", "1 = HTK compatible mean subtraction (this ignores (i.e. does not subtract the mean of) the 0th MFCC or energy field (and its deltas) specified by the option 'idx0')", 0);
    //ct->setField("idx0", "if > 0, index of field to ignore for mean subtraction (attention: 1st field is 0)", 0);
    ct->setField("htkLogEnorm","performs HTK compatible energy normalisation on all input fields instead of the default action of mean subtraction. The energy normalisation subtracts the maximum value of each value in the sequence and adds 1.",0);
    ct->setField("messageRecp","recipient list for new (timestamp adjusted) turnFrameTime message",(const char*)NULL);
  )

  SMILECOMPONENT_MAKEINFO(cFullturnMean);
}

SMILECOMPONENT_CREATE(cFullturnMean)

//-----

/*
receive turnFrameTime messages,
read all data referred to in message + (preSil/postSil)
compute mean, 
subtract mean,  
write to output level
send new turn start/end message (with corrected timestamps) to asr component, for example
*/


cFullturnMean::cFullturnMean(const char *_name) :
  cDataProcessor(_name),
  means(NULL),
  nMeans(0),
  eNormMode(0), dataInQue(0)
{
}

void cFullturnMean::fetchConfig()
{
  cDataProcessor::fetchConfig();

  eNormMode = (int)getInt("htkLogEnorm");
  msgRecp = getStr("messageRecp");
}

int cFullturnMean::processComponentMessage(cComponentMessage *_msg)
{
  if (isMessageType(_msg,"turnFrameTime")) {
    msgQue.push(TurnTimeMsg((long)(_msg->floatData[0]), (long)(_msg->floatData[1]), _msg->intData[0]));
    dataInQue++;
    return 1;
  }
  return 0;
}

int cFullturnMean::checkMessageQueque(long &start, long &end, long &fte)
{
   int ret=0;
   lockMessageMemory();
   if (!msgQue.empty()) {
     start = msgQue.front().vIdxStart;
     end = msgQue.front().vIdxEnd;
     if (msgQue.front().isForcedTurnEnd) fte=1;
     else fte = 0;
     msgQue.pop(); dataInQue--;
     ret = 1;
   }
   unlockMessageMemory();
   return ret;
}

int cFullturnMean::myTick(long long t)
{
  // check for new message...
  long start, end, fte;
  if (checkMessageQueque(start, end, fte)) {
    reader_->setCurR(start);

    cMatrix *mat = reader_->getMatrix(start,end-start+1);
    if (mat == NULL) {
      SMILE_IWRN(2,"the matrix containing the turn could ne read, perhaps something is wrong with the values the turn detector sends? or the timing of the levels do not match?");
      return 0;
    }

    // 1st pass , get statistics
    long i,j;
    FLOAT_DMEM *df = mat->dataF;
    if (means == NULL) {
      means = new cVector( mat->N, mat->type );
    }
    for (j=0; j<mat->N; j++) {
      means->dataF[j] = df[j];
    }
    nMeans = 1;
    
    for (i=1; i<mat->nT; i++) {
      if (eNormMode) {
        for (j=0; j<mat->N; j++) {
          if (df[j] > means->dataF[j]) means->dataF[j] = df[j];
        }
      } else {
        for (j=0; j<mat->N; j++) {
          means->dataF[j] += df[j];
        }
        nMeans++;
      }
      df += mat->N;
    }

    // 2nd pass, subtract means
    if (means == NULL) {
      SMILE_IWRN(1,"sequence too short, cannot compute statistics (mean or max value)!");
      long N = reader_->getLevelN();
      means = new cVector( N, DMEM_FLOAT );
      for (i=0; i<N; i++) {
        means->dataF[i] = 0;
      }
      nMeans = 1;
    }
    if (!eNormMode) {
      for (i=0; i<means->N; i++) {
        means->dataF[i] /= (FLOAT_DMEM)nMeans;
      }
    }

    df = mat->dataF;
    for (i=0; i<mat->nT; i++) {
      if (eNormMode) {
        for (j=0; j<mat->N; j++) {
          df[j] -= means->dataF[j] - (FLOAT_DMEM)1.0;  // x - max + 1 
        }
      } else {
        for (j=0; j<mat->N; j++) {
          df[j] -= means->dataF[j]; 
        }
      }
      df += mat->N;
    }

    // write to output
    writer_->setNextMatrix(mat);

    // send new messages (start + end!)
    if (msgRecp != NULL) {

      /*
      cComponentMessage cmsg("turnEnd");
      cmsg.floatData[0] = (double)nPost;
      cmsg.floatData[1] = (double)(lastVIdx - nPost - eoiMis);
      
      cmsg.intData[0] = 0; // 0 = incomplete turn...
      cmsg.intData[1] = 2;  // set forced turn end flag to EOI (2)
      cmsg.userTime1 = lastVTime;
      sendComponentMessage( recComp, &cmsg );

      cmsg.floatData[0] = (double)nPre;
          cmsg.floatData[1] = (double)startP;
          cmsg.floatData[2] = (double)(reader->getLevelT());
          cmsg.userTime1 = startSmileTime;
          sendComponentMessage( recComp, &cmsg );
          SMILE_IMSG(debug,"sending turnStart message to '%s'",recComp);
*/

      cComponentMessage _msgS("turnStart");
      _msgS.floatData[0] = 0.0;
      _msgS.floatData[1] = curWritePos;
      _msgS.floatData[2] = (double)(reader_->getLevelT());
      sendComponentMessage(msgRecp, &_msgS);

      curWritePos += end-start+1;

      cComponentMessage _msgE("turnEnd");
      _msgE.floatData[0] = 0.0;
      _msgE.floatData[1] = curWritePos-1-fte*5;
      _msgE.floatData[2] = (double)(reader_->getLevelT());
      sendComponentMessage(msgRecp, &_msgE);

      
    }

    return 1;
  }
  
  lockMessageMemory();
  int diq = dataInQue;
  unlockMessageMemory();
  return diq;
}


cFullturnMean::~cFullturnMean()
{
  if (means != NULL) delete means;
}

