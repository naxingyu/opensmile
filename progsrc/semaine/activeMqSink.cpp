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

ActiveMQ feature sender

*/


#include <semaine/activeMqSink.hpp>

#define MODULE "cActiveMqSink"

#ifdef HAVE_SEMAINEAPI

SMILECOMPONENT_STATICS(cActiveMqSink)

SMILECOMPONENT_REGCOMP(cActiveMqSink)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CACTIVEMQSINK;
  sdescription = COMPONENT_DESCRIPTION_CACTIVEMQSINK;

  // we inherit cDataSink configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSink")
  
    SMILECOMPONENT_IFNOTREGAGAIN( {}
//    ct->makeMandatory(ct->setField("topic","ActiveMQ topic to send to",(const char *)NULL)); // semaine.data.analysis.features.voice
//    ct->setField("lag","output data <lag> frames behind",0);
  )

  SMILECOMPONENT_MAKEINFO(cActiveMqSink);
}

SMILECOMPONENT_CREATE(cActiveMqSink)

//-----

cActiveMqSink::cActiveMqSink(const char *_name) :
  cDataSink(_name),
  featureSender(NULL),
  meta(NULL),
  warned(0)
{
}

void cActiveMqSink::fetchConfig()
{
  cDataSink::fetchConfig();
}

/*
int cActiveMqSink::myConfigureInstance()
{
  int ret=1;

  ret *= cDataSink::myConfigureInstance();
  
  return ret;
}
*/

int cActiveMqSink::myTick(long long t)
{
  if (featureSender == NULL) {
    if (warned==0) {
      SMILE_IERR(1,"the semaine featureSender has not been set (it is NULL!), please check the log output of the semaine components, and check the semaineCfg section in the config file! (this warning will be shown only once, the system will not work!)");
      warned = 1;
    }
    return 0;
  }

  SMILE_DBG(4,"tick # %i, reading value vector:",t);
  cVector *vec= reader->getNextFrame();  //new cVector(nValues+1);
  if (vec == NULL) return 0;
  //else reader->nextFrame();

  long vi = vec->tmeta->vIdx;
  double tm = vec->tmeta->time;

  int n,i;
  n = vec->N;
  if (!featureSender->areFeatureNamesSet()) {
    std::vector<std::string> tumFeatureNames(n);
    for (i=0; i<n; i++) {
      std::string name(vec->fmeta->getName(i)); 
      tumFeatureNames[i] = name;
    }
    //    tumFeatureNames[i]="speaking";
    //      tumFeatureNames[i+1]="seqNr";
    featureSender->setFeatureNames(tumFeatureNames);
  }

  std::vector<float> features(n);
  for (i=0; i<n; i++) {
    features[i] = (float)(vec->dataF[i]);
  }
  //    features[i] = speakingStatus;
  //    features[i+1] = seqNr;
  //    seqNr++;

//printf("about to send\n");
// NOTE: (FIXME) sometimes the activeMq sender hangs here:
//               restarting activeMq helps to solve this problem
//               nevertheless it should be investigated...
  if (meta != NULL) {
    featureSender->sendFeatureVector(features, meta->getTime(), false);
  } else {
    featureSender->sendFeatureVector(features, 0, false);
  }
//printf("after sending\n");

  // tick success
  return 1;
}


cActiveMqSink::~cActiveMqSink()
{
}

#endif // HAVE_SEMAINEAPI
