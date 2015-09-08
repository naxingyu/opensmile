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

the SEMAINE Emma message sender for openSMILE

*/


#include <semaine/semaineEmmaSender.hpp>
//#include <math.h>

#define MODULE "cSemaineEmmaSender"

#ifdef HAVE_SEMAINEAPI

SMILECOMPONENT_STATICS(cSemaineEmmaSender)

SMILECOMPONENT_REGCOMP(cSemaineEmmaSender)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CSEMAINEEMMASENDER;
  sdescription = COMPONENT_DESCRIPTION_CSEMAINEEMMASENDER;

  // configure your component's configType:

  SMILECOMPONENT_CREATE_CONFIGTYPE
  //ct->setField("reader", "dataMemory interface configuration (i.e. what slot to read from)", sconfman->getTypeObj("cDataReader"), NO_ARRAY, DONT_FREE);
  ct->setField("dummy","nothing",0);
  SMILECOMPONENT_IFNOTREGAGAIN( {} )

  SMILECOMPONENT_MAKEINFO(cSemaineEmmaSender);
}

SMILECOMPONENT_CREATE(cSemaineEmmaSender)



//-----

cSemaineEmmaSender::cSemaineEmmaSender(const char *_name) :
  cSmileComponent(_name),
  //gPtr(0),
  emmaSender(NULL), meta(NULL)
{
  // initialisation code...
/*
  int i;
  for (i=0; i<N_GENDER_HISTORY; i++) {
    gender[i] = 0;
  }
  */
}

  /*
void cSemaineEmmaSender::mySetEnvironment()
{

}

int cSemaineEmmaSender::myConfigureInstance()
{

  return 1;
}

int cSemaineEmmaSender::myFinaliseInstance()
{

  return 1;
}
*/

// get smileTime from _msg, convert to semaine time by getting current time from smile and semaine and observing the difference
long long cSemaineEmmaSender::smileTimeToSemaineTime( double smileTime ) 
{
  smileTime *= 1000.0;
  if (meta != NULL) {
    long long smit = (long long)round(getSmileTime()*1000.0);
    return (long long)round(smileTime) + (meta->getTime() - smit);
  } else {
    SMILE_IWRN(1,"warning, no meta component found, times sent are SMILE times and NOT semaine times!");
    return (long long)round(smileTime);
  }
  return 0;
}

// send an XML document and delete it afterwards
void cSemaineEmmaSender::sendDocument( XERCESC_NS::DOMDocument * document )
{
  // Now send it
  if ((emmaSender!=NULL)&&(meta != NULL)&&(meta->isSystemReady())) {
    emmaSender->sendXML(document, meta->getTime());
  } 
  
  delete document;
  /*
  else {
    emmaSender->sendXML(document, 0.0);
  }*/
}

/*
void cSemaineEmmaSender::sendDimension( cComponentMessage *_msg, std::string dimensionStr, std::string dimensionVoc )
{
  // range check:
  if (_msg->floatData[0] < 0) _msg->floatData[0] = 0.0;
  if (_msg->floatData[0] > 1) _msg->floatData[0] = 1.0;
  //--

  char strtmp[50];
  sprintf(strtmp,"%.2f",_msg->floatData[0]);
  std::string dimVal(strtmp);

  sprintf(strtmp,"%ld",smileTimeToSemaineTime(_msg->userTime1));
  std::string startTm(strtmp);
  sprintf(strtmp,"%ld",(long long)round((_msg->userTime2 - _msg->userTime1)*1000.0));
  std::string duration(strtmp);

  sprintf(strtmp,"%s",(const char *)(_msg->custData2));
  std::string codername(strtmp);

  // Create and fill a simple EMMA EmotionML document
  XERCESC_NS::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
  XERCESC_NS::DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
  XMLTool::setAttribute(interpretation, EMMA::A_OFFSET_TO_START, startTm);
  XMLTool::setAttribute(interpretation, EMMA::A_DURATION, duration);
  XMLTool::setAttribute(interpretation, EMMA::A_CONFIDENCE, "1.0");
  XMLTool::setPrefix(interpretation, "emma");
  
  XERCESC_NS::DOMElement * emotion = XMLTool::appendChildElement(interpretation, EmotionML::E_EMOTION, EmotionML::namespaceURI);
  XMLTool::setAttribute(emotion, EmotionML::A_DIMENSION_VOCABULARY, dimensionVoc);
  XMLTool::setPrefix(emotion, "emotion");

  XERCESC_NS::DOMElement * dimension = XMLTool::appendChildElement(emotion, EmotionML::E_DIMENSION, EmotionML::namespaceURI);
  XMLTool::setAttribute(dimension, EmotionML::A_NAME, dimensionStr);
  XMLTool::setAttribute(dimension, EmotionML::A_VALUE, dimVal);
  XMLTool::setPrefix(dimension, "emotion");

  XERCESC_NS::DOMElement * info = XMLTool::appendChildElement(emotion, EmotionML::E_INFO);
  XERCESC_NS::DOMElement * coder = XMLTool::appendChildElement(info, "predictor");
  XMLTool::setAttribute(coder, "value", codername);
  XMLTool::setPrefix(coder, "emotion");
  XMLTool::setPrefix(info, "emotion");

  sendDocument(document);
}
*/

/*
void cSemaineEmmaSender::sendArousalC( cComponentMessage *_msg )
{
  char strtmp[50];
  sprintf(strtmp,"%.2f",_msg->floatData[0]);
  std::string aroStr(strtmp);

  sprintf(strtmp,"%ld",smileTimeToSemaineTime(_msg->userTime1));
  std::string startTm(strtmp);
  sprintf(strtmp,"%ld",(long long)round((_msg->userTime2 - _msg->userTime1)*1000.0));
  std::string duration(strtmp);

  // Create and fill a simple EMMA EmotionML document
  XERCESC_NS::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
  XERCESC_NS::DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
  XMLTool::setAttribute(interpretation, EMMA::A_OFFSET_TO_START, startTm);
  XMLTool::setAttribute(interpretation, EMMA::A_DURATION, duration);
  XMLTool::setPrefix(interpretation, "emma");
  
  XERCESC_NS::DOMElement * emotion = XMLTool::appendChildElement(interpretation, EmotionML::E_EMOTION, EmotionML::namespaceURI);
  XMLTool::setAttribute(emotion, EmotionML::A_DIMENSION_VOCABULARY, EmotionML::VOC_FSRE_DIMENSION_DEFINITION);
  XMLTool::setPrefix(emotion, "emotion");

  XERCESC_NS::DOMElement * dimension = XMLTool::appendChildElement(emotion, EmotionML::E_DIMENSION, EmotionML::namespaceURI);
  XMLTool::setAttribute(dimension, EmotionML::A_NAME, EmotionML::VOC_FSRE_DIMENSION_AROUSAL);
  XMLTool::setAttribute(dimension, EmotionML::A_VALUE, aroStr);
  XMLTool::setPrefix(dimension, "emotion");

  sendDocument(document);
}

void cSemaineEmmaSender::sendValenceC( cComponentMessage *_msg )
{
  char strtmp[50];
  sprintf(strtmp,"%.2f",_msg->floatData[0]);
  std::string valStr(strtmp);

  sprintf(strtmp,"%ld",smileTimeToSemaineTime(_msg->userTime1));
  std::string startTm(strtmp);
  sprintf(strtmp,"%ld",(long long)round((_msg->userTime2 - _msg->userTime1)*1000.0));
  std::string duration(strtmp);

  // Create and fill a simple EMMA EmotionML document
  XERCESC_NS::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
  XERCESC_NS::DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
  XMLTool::setAttribute(interpretation, EMMA::A_OFFSET_TO_START, startTm);
  XMLTool::setAttribute(interpretation, EMMA::A_DURATION, duration);
  XMLTool::setPrefix(interpretation, "emma");

  XERCESC_NS::DOMElement * emotion = XMLTool::appendChildElement(interpretation, EmotionML::E_EMOTION, EmotionML::namespaceURI);
  XMLTool::setPrefix(emotion, "emotion");

  XERCESC_NS::DOMElement * dimensions = XMLTool::appendChildElement(emotion, EmotionML::E_DIMENSIONS, EmotionML::namespaceURI);
  XMLTool::setAttribute(dimensions, EmotionML::A_SET, "valenceArousalPotency");
  XMLTool::setPrefix(dimensions, "emotion");

  XERCESC_NS::DOMElement * valence = XMLTool::appendChildElement(dimensions, EmotionML::E_VALENCE, EmotionML::namespaceURI);
  XMLTool::setAttribute(valence, EmotionML::A_VALUE, valStr);
  XMLTool::setPrefix(valence, "emotion");

  // Now send it
  sendDocument(document);
}

void cSemaineEmmaSender::sendPowerC( cComponentMessage *_msg )
{
  char strtmp[50];
  sprintf(strtmp,"%.2f",_msg->floatData[0]);
  std::string valStr(strtmp);

  sprintf(strtmp,"%ld",smileTimeToSemaineTime(_msg->userTime1));
  std::string startTm(strtmp);
  sprintf(strtmp,"%ld",(long long)round((_msg->userTime2 - _msg->userTime1)*1000.0));
  std::string duration(strtmp);

  // Create and fill a simple EMMA EmotionML document
  XERCESC_NS::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
  XERCESC_NS::DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
  XMLTool::setAttribute(interpretation, EMMA::A_OFFSET_TO_START, startTm);
  XMLTool::setAttribute(interpretation, EMMA::A_DURATION, duration);
  XMLTool::setPrefix(interpretation, "emma");

  XERCESC_NS::DOMElement * emotion = XMLTool::appendChildElement(interpretation, EmotionML::E_EMOTION, EmotionML::namespaceURI);
  XMLTool::setPrefix(emotion, "emotion");

  XERCESC_NS::DOMElement * dimensions = XMLTool::appendChildElement(emotion, EmotionML::E_DIMENSIONS, EmotionML::namespaceURI);
  XMLTool::setAttribute(dimensions, EmotionML::A_SET, "Power");
  XMLTool::setPrefix(dimensions, "emotion");

  XERCESC_NS::DOMElement * power = XMLTool::appendChildElement(dimensions, EmotionML::E_POWER, EmotionML::namespaceURI);
  XMLTool::setAttribute(power, EmotionML::A_VALUE, valStr);
  XMLTool::setPrefix(power, "emotion");

  // Now send it
  sendDocument(document);
}
*/
/*
void cSemaineEmmaSender::sendIntensityC( cComponentMessage *_msg )
{
  char strtmp[50];
  sprintf(strtmp,"%.2f",_msg->floatData[0]);
  std::string valStr(strtmp);

  sprintf(strtmp,"%ld",smileTimeToSemaineTime(_msg->userTime1));
  std::string startTm(strtmp);
  sprintf(strtmp,"%ld",(long long)round((_msg->userTime2 - _msg->userTime1)*1000.0));
  std::string duration(strtmp);

  // Create and fill a simple EMMA EmotionML document
  XERCESC_NS::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
  XERCESC_NS::DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
  XMLTool::setAttribute(interpretation, EMMA::A_OFFSET_TO_START, startTm);
  XMLTool::setAttribute(interpretation, EMMA::A_DURATION, duration);
  XMLTool::setPrefix(interpretation, "emma");

  XERCESC_NS::DOMElement * emotion = XMLTool::appendChildElement(interpretation, EmotionML::E_EMOTION, EmotionML::namespaceURI);
  XMLTool::setPrefix(emotion, "emotion");

  XERCESC_NS::DOMElement * dimensions = XMLTool::appendChildElement(emotion, EmotionML::E_DIMENSIONS, EmotionML::namespaceURI);
  XMLTool::setAttribute(dimensions, EmotionML::A_SET, "Intensity");
  XMLTool::setPrefix(dimensions, "emotion");

  XERCESC_NS::DOMElement * intensity = XMLTool::appendChildElement(dimensions, EmotionML::E_INTENSITY, EmotionML::namespaceURI);
  XMLTool::setAttribute(intensity, EmotionML::A_VALUE, valStr);
  XMLTool::setPrefix(intensity, "emotion");

  // Now send it
  sendDocument(document);
}
*/
/*
void cSemaineEmmaSender::sendAnticipationC( cComponentMessage *_msg )
{
  char strtmp[50];
  sprintf(strtmp,"%.2f",_msg->floatData[0]);
  std::string valStr(strtmp);

  sprintf(strtmp,"%ld",smileTimeToSemaineTime(_msg->userTime1));
  std::string startTm(strtmp);
  sprintf(strtmp,"%ld",(long long)round((_msg->userTime2 - _msg->userTime1)*1000.0));
  std::string duration(strtmp);

  // Create and fill a simple EMMA EmotionML document
  XERCESC_NS::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
  XERCESC_NS::DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
  XMLTool::setAttribute(interpretation, EMMA::A_OFFSET_TO_START, startTm);
  XMLTool::setAttribute(interpretation, EMMA::A_DURATION, duration);
  XMLTool::setPrefix(interpretation, "emma");

  XERCESC_NS::DOMElement * emotion = XMLTool::appendChildElement(interpretation, EmotionML::E_EMOTION, EmotionML::namespaceURI);
  XMLTool::setPrefix(emotion, "emotion");

  XERCESC_NS::DOMElement * dimensions = XMLTool::appendChildElement(emotion, EmotionML::E_DIMENSIONS, EmotionML::namespaceURI);
  XMLTool::setAttribute(dimensions, EmotionML::A_SET, "Anticipation");
  XMLTool::setPrefix(dimensions, "emotion");

  XERCESC_NS::DOMElement * anticipation = XMLTool::appendChildElement(dimensions, EmotionML::E_EXPECTATION, EmotionML::namespaceURI);
  XMLTool::setAttribute(anticipation, EmotionML::A_VALUE, valStr);
  XMLTool::setPrefix(anticipation, "emotion");

  // Now send it
  sendDocument(document);
}

void cSemaineEmmaSender::sendSingleEmotionDim( cComponentMessage *_msg, const char * set, std::string dim )
{
  char strtmp[50];
  sprintf(strtmp,"%.2f",_msg->floatData[0]);
  std::string valStr(strtmp);

  sprintf(strtmp,"%ld",smileTimeToSemaineTime(_msg->userTime1));
  std::string startTm(strtmp);
  sprintf(strtmp,"%ld",(long long)round((_msg->userTime2 - _msg->userTime1)*1000.0));
  std::string duration(strtmp);

  // Create and fill a simple EMMA EmotionML document
  XERCESC_NS::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
  XERCESC_NS::DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
  XMLTool::setAttribute(interpretation, EMMA::A_OFFSET_TO_START, startTm);
  XMLTool::setAttribute(interpretation, EMMA::A_DURATION, duration);
  XMLTool::setPrefix(interpretation, "emma");

  XERCESC_NS::DOMElement * emotion = XMLTool::appendChildElement(interpretation, EmotionML::E_EMOTION, EmotionML::namespaceURI);
  XMLTool::setPrefix(emotion, "emotion");

  XERCESC_NS::DOMElement * dimensions = XMLTool::appendChildElement(emotion, EmotionML::E_DIMENSIONS, EmotionML::namespaceURI);
  XMLTool::setAttribute(dimensions, EmotionML::A_SET, set);
  XMLTool::setPrefix(dimensions, "emotion");

  XERCESC_NS::DOMElement * dimension = XMLTool::appendChildElement(dimensions, dim, EmotionML::namespaceURI);
  XMLTool::setAttribute(dimension, EmotionML::A_VALUE, valStr);
  XMLTool::setPrefix(dimension, "emotion");

  // Now send it
  sendDocument(document);
}
*/

/*
void cSemaineEmmaSender::sendInterestC( cComponentMessage *_msg )
{
  char strtmp[50];
  sprintf(strtmp,"%.2f",_msg->floatData[0]);
  std::string interestStr(strtmp);

  // Create and fill a simple EMMA EmotionML document
  XERCESC_NS::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);

  sprintf(strtmp,"%ld",smileTimeToSemaineTime(_msg->userTime1));
  std::string startTm(strtmp);
  sprintf(strtmp,"%ld",(long long)round((_msg->userTime2 - _msg->userTime1)*1000.0));
  std::string duration(strtmp);

  XERCESC_NS::DOMElement * oneof = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_ONEOF);
  XMLTool::setAttribute(oneof, EMMA::A_OFFSET_TO_START, startTm);
  XMLTool::setAttribute(oneof, EMMA::A_DURATION, duration);
  XMLTool::setPrefix(oneof, "emma");

  XERCESC_NS::DOMElement * interpretation0 = XMLTool::appendChildElement(oneof, EMMA::E_INTERPRETATION);
  sprintf(strtmp,"%.3f",((double*)(_msg->custData))[0]);
  std::string conf0(strtmp);
  XMLTool::setAttribute(interpretation0, EMMA::A_CONFIDENCE, conf0);
  XMLTool::setPrefix(interpretation0, "emma");

  XERCESC_NS::DOMElement * emotion0 = XMLTool::appendChildElement(interpretation0, EmotionML::E_EMOTION, EmotionML::namespaceURI);
  XMLTool::setAttribute(emotion0, EmotionML::A_CATEGORY_VOCABULARY , EmotionML::VOC_SEMAINE_INTEREST_CATEGORY_DEFINITION);
  XMLTool::setPrefix(emotion0, "emotion");
  
  XERCESC_NS::DOMElement * category0 = XMLTool::appendChildElement(emotion0, EmotionML::E_CATEGORY, EmotionML::namespaceURI);
  XMLTool::setAttribute(category0, EmotionML::A_NAME, "bored");
  XMLTool::setPrefix(category0, "emotion");

  XERCESC_NS::DOMElement * interpretation1 = XMLTool::appendChildElement(oneof, EMMA::E_INTERPRETATION);
  sprintf(strtmp,"%.3f",((double*)(_msg->custData))[1]);
  std::string conf1(strtmp);
  XMLTool::setAttribute(interpretation1, EMMA::A_CONFIDENCE, conf1);
  XMLTool::setPrefix(interpretation1, "emma");

  XERCESC_NS::DOMElement * emotion1 = XMLTool::appendChildElement(interpretation1, EmotionML::E_EMOTION, EmotionML::namespaceURI);
  XMLTool::setAttribute(emotion1, EmotionML::A_CATEGORY_VOCABULARY , EmotionML::VOC_SEMAINE_INTEREST_CATEGORY_DEFINITION);
  XMLTool::setPrefix(emotion1, "emotion");
  XERCESC_NS::DOMElement * category1 = XMLTool::appendChildElement(emotion1, EmotionML::E_CATEGORY, EmotionML::namespaceURI);
  XMLTool::setAttribute(category1, EmotionML::A_NAME, "neutral");
  XMLTool::setPrefix(category1, "emotion");
  
  XERCESC_NS::DOMElement * interpretation2 = XMLTool::appendChildElement(oneof, EMMA::E_INTERPRETATION);
  sprintf(strtmp,"%.3f",((double*)(_msg->custData))[2]);
  std::string conf2(strtmp);
  XMLTool::setAttribute(interpretation2, EMMA::A_CONFIDENCE, conf2);
  XMLTool::setPrefix(interpretation2, "emma");

  XERCESC_NS::DOMElement * emotion2 = XMLTool::appendChildElement(interpretation2, EmotionML::E_EMOTION, EmotionML::namespaceURI);
  XMLTool::setAttribute(emotion2, EmotionML::A_CATEGORY_VOCABULARY , EmotionML::VOC_SEMAINE_INTEREST_CATEGORY_DEFINITION);
  XMLTool::setPrefix(emotion2, "emotion");
  XERCESC_NS::DOMElement * category2 = XMLTool::appendChildElement(emotion2, EmotionML::E_CATEGORY, EmotionML::namespaceURI);
  XMLTool::setAttribute(category2, EmotionML::A_NAME, "interested");
  XMLTool::setPrefix(category2, "emotion");

  // Now send it
  sendDocument(document);
}

// start = 1: speaking status changed to start, start = 0: status changed to stop
void cSemaineEmmaSender::sendSpeakingStatus( cComponentMessage *_msg, int start )
{
  const char *strtmp = "stop";
  if (start) strtmp = "start";
  
  std::string statusStr(strtmp);

  char strtmp2[50];
  sprintf(strtmp2,"%ld",smileTimeToSemaineTime(_msg->userTime1));
  std::string startTm(strtmp2);

  // Create and fill a simple EMMA EmotionML document
  XERCESC_NS::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
  XERCESC_NS::DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
  XMLTool::setAttribute(interpretation, EMMA::A_OFFSET_TO_START, startTm);
  XMLTool::setAttribute(interpretation, EMMA::A_DURATION, "0");
  XMLTool::setPrefix(interpretation, "emma");

  XERCESC_NS::DOMElement * speaking = XMLTool::appendChildElement(interpretation, SemaineML::E_SPEAKING, SemaineML::namespaceURI);
  XMLTool::setAttribute(speaking, SemaineML::A_STATUS_CHANGE, strtmp);
  XMLTool::setPrefix(speaking, "semaine");

  // Now send it
  sendDocument(document);
}
*/

//void cSemaineEmmaSender::sendGender( cComponentMessage *_msg )
//{
/*
<emma:emma version="1.0" xmlns:emma="http://www.w3.org/2003/04/emma">
    <emma:interpretation  emma:offset-to-start="12345" emma:confidence="0.3">

      <semaine:gender name="female" xmlns:semaine="http://www.semaine-project.eu/semaineml"/>

    </emma:interpretation>
</emma:emma>
*/
  
/*
  const char *strtmp = "female";

  // manage gender result history buffer
  if (_msg->msgtext[0] == 'M') {
    gender[gPtr] = GENDER_M;
  } else {
    gender[gPtr] = GENDER_F;
  }

  gPtr++; 
  if (gPtr >= N_GENDER_HISTORY) gPtr = 0;

  // count # male/female in gender result history
  int m=0,f=0;
  int i;
  for (i=0; i<N_GENDER_HISTORY; i++) {
    if (gender[i] == GENDER_M) m++;
    else if (gender[i] == GENDER_F) f++;
  }

  // majority vote on gender
  if (m>f) { strtmp = "male"; }


  // fill XML document
  std::string genderStr(strtmp);

  char strtmp2[50];
  sprintf(strtmp2,"%ld",smileTimeToSemaineTime(_msg->userTime1));
  std::string startTm(strtmp2);

  // Create and fill a simple EMMA EmotionML document
  XERCESC_NS::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
  XERCESC_NS::DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
  XMLTool::setAttribute(interpretation, EMMA::A_OFFSET_TO_START, startTm);
  XMLTool::setAttribute(interpretation, EMMA::A_DURATION, "0");
  XMLTool::setPrefix(interpretation, "emma");

  XERCESC_NS::DOMElement * gender = XMLTool::appendChildElement(interpretation, SemaineML::E_GENDER, SemaineML::namespaceURI);
  XMLTool::setAttribute(gender, SemaineML::A_NAME, strtmp);
  XMLTool::setPrefix(gender, "semaine");

  // Now send it
  sendDocument(document);
}

void cSemaineEmmaSender::sendPitchDirection( cComponentMessage *_msg )
{
*/
/*
<emma:emma version="1.0" xmlns:emma="http://www.w3.org/2003/04/emma">
    <emma:interpretation  emma:offset-to-start="12345" emma:duration="444" emma:confidence="0.3">

        <semaine:pitch xmlns:semaine="http://www.semaine-project.eu/semaineml" direction="rise"/>

    </emma:interpretation>
</emma:emma>
*/
/*
  const char *strtmp = "rise"; // _msg->intData[0] == 0
  if (_msg->intData[0] == 1) strtmp = "fall";
  else if (_msg->intData[0] == 2) strtmp = "rise-fall";
  else if (_msg->intData[0] == 3) strtmp = "fall-rise";
  std::string pitchStr(strtmp);

  char strtmp2[50];
  sprintf(strtmp2,"%ld",smileTimeToSemaineTime(_msg->userTime1));
  std::string startTm(strtmp2);

  // Create and fill a simple EMMA EmotionML document
  XERCESC_NS::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
  XERCESC_NS::DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
  XMLTool::setAttribute(interpretation, EMMA::A_OFFSET_TO_START, startTm);
  XMLTool::setAttribute(interpretation, EMMA::A_DURATION, "0");
  XMLTool::setPrefix(interpretation, "emma");

  XERCESC_NS::DOMElement * pitchDir = XMLTool::appendChildElement(interpretation, SemaineML::E_PITCH, SemaineML::namespaceURI);
  XMLTool::setAttribute(pitchDir, SemaineML::A_DIRECTION, strtmp);
  XMLTool::setPrefix(pitchDir, "semaine");

  // Now send it
  sendDocument(document);
}

// this includes the juliusResult struct/class definition
#include <classifiers/julius/kwsjKresult.h>

void cSemaineEmmaSender::sendKeywords( cComponentMessage *_msg )
{
  int i;
  juliusResult *k = (juliusResult *)(_msg->custData);
  if (k==NULL) return;

  char strtmp[150];
  sprintf(strtmp,"%.2f",_msg->floatData[0]);
  std::string valStr(strtmp);
  long long startTime = smileTimeToSemaineTime(_msg->userTime1);
  sprintf(strtmp,"%ld",startTime);
  std::string startTm(strtmp);
  sprintf(strtmp,"%ld",(long long)round((_msg->userTime2 - _msg->userTime1)*1000.0));
  std::string duration(strtmp);

  // Create and fill a simple EMMA EmotionML document
  XERCESC_NS::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
  XERCESC_NS::DOMElement * sequence = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_SEQUENCE);
  XMLTool::setAttribute(sequence, EMMA::A_OFFSET_TO_START, startTm);
  XMLTool::setAttribute(sequence, EMMA::A_DURATION, duration);
  XMLTool::setPrefix(sequence, "emma");

  for (i=0; i<k->numW; i++) {

    // split combined keywords (TALK_TO_POPPY) etc. at the special character "_" and put them in individual tags
    char * tr = strdup(k->word[i]);
    char * tmp = tr;
    char * x = NULL;
    do {
      x = (char *)strchr(tmp, '_');
      // separate at '_'
      if (x!=NULL) {
        *x = 0;
      }

      // remove spaces
      //while (*tmp==' ') { tmp++; }
      //size_t l = strlen(tmp);
      //while (tmp[l-1] == ' ') { tmp[l-1] = 0; l--; }

      // append an xml keyword tag
      XERCESC_NS::DOMElement * interpretation = XMLTool::appendChildElement(sequence, EMMA::E_INTERPRETATION);
      sprintf(strtmp,"%ld",startTime + (long long)round((k->start[i])*1000.0));
      std::string offs(strtmp);
      sprintf(strtmp,"%s",tmp);
      std::string keyword(strtmp);
      sprintf(strtmp,"%.3f",k->conf[i]);
      std::string confidence(strtmp);
      XMLTool::setAttribute(interpretation, EMMA::A_OFFSET_TO_START, offs);
      XMLTool::setAttribute(interpretation, EMMA::A_TOKENS, keyword);
      XMLTool::setAttribute(interpretation, EMMA::A_CONFIDENCE, confidence);
      XMLTool::setPrefix(interpretation, "emma");


      // analyse next part of string, if present
      if (x != NULL) {
        tmp = x+1;
      }
    } while (x !=  NULL);
    free(tr);



    // one word:
    */
  /*
  XERCESC_NS::DOMElement * interpretation = XMLTool::appendChildElement(sequence, EMMA::E_INTERPRETATION);
  sprintf(strtmp,"%ld",startTime + (long long)round((k->kwStartTime[i])*1000.0));
  std::string offs(strtmp);
  sprintf(strtmp,"%s",k->keyword[i]);
  std::string keyword(strtmp);
  sprintf(strtmp,"%.3f",k->kwConf[i]);
  std::string confidence(strtmp);
  XMLTool::setAttribute(interpretation, EMMA::A_OFFSET_TO_START, offs);
  XMLTool::setAttribute(interpretation, EMMA::A_TOKENS, keyword);
  XMLTool::setAttribute(interpretation, EMMA::A_CONFIDENCE, confidence);
  XMLTool::setPrefix(interpretation, "emma");*/

/*
  }


  // Now send it
  sendDocument(document);
}

*/



/*
int cSemaineEmmaSender::processComponentMessage( cComponentMessage *_msg ) 
{ 
  
  if (isMessageType(_msg,"classificationResult")) {  
    // determine origin by message's user-defined name, which can be set in the config file
    SMILE_IDBG(3,"received 'classificationResult' message");
    if (!strcmp(_msg->msgname,"arousal")) {
      // convert to EmotionML standard range: (SEMAINE uses -1 to +1, EmotionML 0 .. 1)
      _msg->floatData[0] = (_msg->floatData[0] + 1.0)/2.0;
      sendDimensionFSRE(_msg,EmotionML::VOC_FSRE_DIMENSION_AROUSAL);
    }
    else if (!strcmp(_msg->msgname,"valence")) {
      // convert to EmotionML standard range: (SEMAINE uses -1 to +1, EmotionML 0 .. 1)
      _msg->floatData[0] = (_msg->floatData[0] + 1.0)/2.0;
      sendDimensionFSRE(_msg,EmotionML::VOC_FSRE_DIMENSION_VALENCE);
    }
    else if (!strcmp(_msg->msgname,"interest")) sendInterestC(_msg);
    else if (!strcmp(_msg->msgname,"gender")) sendGender(_msg);
    else if (!strcmp(_msg->msgname,"power")) {
      // convert to EmotionML standard range: (SEMAINE uses -1 to +1, EmotionML 0 .. 1)
      _msg->floatData[0] = (_msg->floatData[0] + 1.0)/2.0;
      sendDimensionFSRE(_msg,EmotionML::VOC_FSRE_DIMENSION_POTENCY);
    }
    else if (!strcmp(_msg->msgname,"intensity")) {
      // convert to EmotionML standard range: (SEMAINE uses -1 to +1, EmotionML 0 .. 1)
      _msg->floatData[0] = (_msg->floatData[0] + 1.0)/2.0;
      sendDimension(_msg,EmotionML::VOC_SEMAINE_INTENSITY_DIMENSION_INTENSITY, EmotionML::VOC_SEMAINE_INTENSITY_DIMENSION_DEFINITON);
    }
    else if (!strcmp(_msg->msgname,"anticipation")) {
      // invert values and map to EmotionML range: 
      _msg->floatData[0] = (1.0 - _msg->floatData[0])/2.0;
      sendDimensionFSRE(_msg,EmotionML::VOC_FSRE_DIMENSION_UNPREDICTABILITY);
    }
    return 1;  // message was processed
  }
  else if (isMessageType(_msg,"pitchDirection")) {  
    SMILE_IDBG(3,"received 'pitchDirection' message");
    sendPitchDirection(_msg);
    return 1;  // message was processed
  }
  else if (isMessageType(_msg,"asrKeywordOutput")) {  
    SMILE_IDBG(3,"received 'asrKeywordOutput' message");
    sendKeywords(_msg);
    return 1;  // message was processed
  }
  else if (isMessageType(_msg,"turnSpeakingStatus")) {
    SMILE_IDBG(2,"received 'turnSpeakingStatus' message: %i",_msg->intData[0]);
    sendSpeakingStatus(_msg, _msg->intData[0]);
    return 1; // message was processed
  }


  return 0; // if message was not processed
}  
*/

int cSemaineEmmaSender::myTick(long long t) 
{

  // return 1;  // tick did succeed!

  return 0; // tick did not succeed, i.e. nothing was processed or there was nothing to process
}

cSemaineEmmaSender::~cSemaineEmmaSender()
{
  // cleanup code...

}

#endif //HAVE_SEMAINEAPI

