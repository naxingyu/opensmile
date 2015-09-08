/*F***************************************************************************
 * openSMILE - the Munich open source Multimedia Interpretation by Large-scale
 * Extraction toolkit
 * 
 * (c) 2008-2011, Florian Eyben, Martin Woellmer, Bjoern Schuller @ TUM-MMK
 * (c) 2012-2013, Florian Eyben, Felix Weninger, Bjoern Schuller @ TUM-MMK (c)
 * 2013-2014 audEERING UG, haftungsbeschränkt. All rights reserved.
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

data reader from activeMq... TODO

*/


#include <semaine/activeMqSource.hpp>
#define MODULE "cActiveMqSource"

#ifdef HAVE_SEMAINEAPI

SMILECOMPONENT_STATICS(cActiveMqSource)

SMILECOMPONENT_REGCOMP(cActiveMqSource)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CACTIVEMQSOURCE;
  sdescription = COMPONENT_DESCRIPTION_CACTIVEMQSOURCE;

  // we inherit cDataSource configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSource")
  
  SMILECOMPONENT_IFNOTREGAGAIN(
   // ct->makeMandatory(ct->setField("topic","topic to read data vectors from",(const char*) NULL));
   ct->setField("nValues","number of (feature) values to read (if vector obtained from activeMq is smaller than this value, it will be padded with zeros, otherwise it will be stripped)",1);
   ct->setField("fStep","reference frameStep in seconds (TODO: read from an input level via a dataReader)",0.01);
   ct->setField("vadRecv","VAD component to send the vadInhibit message to",(const char*)NULL);
  )

  SMILECOMPONENT_MAKEINFO(cActiveMqSource);
}

SMILECOMPONENT_CREATE(cActiveMqSource)

//-----

cActiveMqSource::cActiveMqSource(const char *_name) :
  cDataSource(_name),
  warned(0),
  nValues(1),
  running(0)
{
  // ...
}

void cActiveMqSource::fetchConfig()
{
  cDataSource::fetchConfig();

  nValues = getInt("nValues");
  SMILE_IDBG(2,"nValues = %i",nValues);

  fStep = (FLOAT_DMEM)getDouble("fStep");
  SMILE_IDBG(2,"fStep = %f",fStep);

  vadRecv = getStr("vadRecv");
  SMILE_IDBG(2,"vadRecv = '%s'",vadRecv);
}

/*
int cActiveMqSource::myConfigureInstance()
{
    // call writer->setConfig here to set the dataMemory level configuration and override config file and defaults...
    
  int ret = 1;
  ret *= cDataSource::myConfigureInstance();
  // ...
  return ret;
}
*/

int cActiveMqSource::setupNewNames(long nEl)
{
  writer->addField("amqData",nValues);

  allocVec(nValues);

  namesAreSet=1;
  return 1;
}

int cActiveMqSource::myFinaliseInstance()
{
  int ret = cDataSource::myFinaliseInstance();
  return ret;

//  return cDataSource::myFinaliseInstance();
}

// This is called from TumFeatureExtractor.cpp
void cActiveMqSource::waveFileReceive(void *data, long long N)
{
  sWaveParameters para;
  if (smilePcm_parseWaveHeader(data, N, &para)) {

    float sr = (float)(para.sampleRate);
    float n  = (float)(para.nBlocks);
    float lenSec = ((float)1.0/sr * n);
    long lenFrames = (long)(lenSec / fStep);

    SMILE_IDBG(3,"received a wave file, length = %f s (%i frames)",lenSec,lenFrames);
    // now send a message with this information to the VAD component
    if (vadRecv != NULL) {
      cComponentMessage m;
      

    }

  }
}

int cActiveMqSource::myTick(long long t)
{
  //SMILE_DBG(4,"tick # %i, writing value vector",t);
  running = 1;

  // todo: fill with random values...
//  vec->dataF[0] = (FLOAT_DMEM)t+(FLOAT_DMEM)3.3;
//  writer->setNextFrame(vec);
  
  return 0;
}


cActiveMqSource::~cActiveMqSource()
{
  running = 0;
  // if (featureReceiver != NULL) delete featureReceiver;
}


#endif // HAVE_SEMAINEAPI
