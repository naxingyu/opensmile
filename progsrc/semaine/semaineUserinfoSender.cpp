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


#include <semaine/semaineUserinfoSender.hpp>
//#include <math.h>

#define MODULE "cSemaineUserinfoSender"

#ifdef HAVE_SEMAINEAPI

SMILECOMPONENT_STATICS(cSemaineUserinfoSender)

SMILECOMPONENT_REGCOMP(cSemaineUserinfoSender)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CSEMAINEUSERINFOSENDER;
  sdescription = COMPONENT_DESCRIPTION_CSEMAINEUSERINFOSENDER;

  // configure your component's configType:

  SMILECOMPONENT_CREATE_CONFIGTYPE
  //ct->setField("reader", "dataMemory interface configuration (i.e. what slot to read from)", sconfman->getTypeObj("cDataReader"), NO_ARRAY, DONT_FREE);
  ct->setField("dummy","nothing",0);
  SMILECOMPONENT_IFNOTREGAGAIN( {} )

  SMILECOMPONENT_MAKEINFO(cSemaineUserinfoSender);
}

SMILECOMPONENT_CREATE(cSemaineUserinfoSender)



//-----

cSemaineUserinfoSender::cSemaineUserinfoSender(const char *_name) :
  cSemaineEmmaSender(_name),
  gPtr(0)
{
  // initialisation code...
  int i;
  for (i=0; i<N_GENDER_HISTORY; i++) {
    gender[i] = 0;
  }
}


// start = 1: speaking status changed to start, start = 0: status changed to stop
void cSemaineUserinfoSender::sendSpeakingStatus( cComponentMessage *_msg, int start )
{
  const char *strtmp = "stop";
  if (start) strtmp = "start";
  
  std::string statusStr(strtmp);

  char strtmp2[50];
  sprintf(strtmp2,"%ld",smileTimeToSemaineTime(_msg->userTime1));
  std::string startTm(strtmp2);

  // Create and fill a simple EMMA EmotionML document
  XERCESC_NS::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
  XMLTool::setPrefix(document->getDocumentElement(), "emma");

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

// start = 1: speaking status changed to start, start = 0: status changed to stop
void cSemaineUserinfoSender::sendOffTalk( cComponentMessage *msg )
{
  char strtmp[CMSG_textLen];
  sprintf(strtmp,"%ld", smileTimeToSemaineTime(msg->userTime1));
  std::string startTm(strtmp);

  std::string talk_dir = "unknown";
  if (msg->msgtext != NULL) {
    if (msg->msgtext[0] == 'o') {
      talk_dir = "on";
    } else if (msg->msgtext[0] == 'n') {
      talk_dir = "off";
    }
  }
  
  // Create and fill a simple EMMA EmotionML document
  XERCESC_NS::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
  XMLTool::setPrefix(document->getDocumentElement(), "emma");

  XERCESC_NS::DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
  XMLTool::setAttribute(interpretation, EMMA::A_OFFSET_TO_START, startTm);
  XMLTool::setAttribute(interpretation, EMMA::A_DURATION, "0");
  XMLTool::setPrefix(interpretation, "emma");

  XERCESC_NS::DOMElement * offtalk = XMLTool::appendChildElement(interpretation, SemaineML::E_OFFTALK, SemaineML::namespaceURI);
  XMLTool::setAttribute(offtalk, SemaineML::A_DIRECTION, talk_dir);
  XMLTool::setPrefix(offtalk, "semaine");

  // Now send it
  sendDocument(document);
}


void cSemaineUserinfoSender::sendGender( cComponentMessage *_msg )
{
/*
<emma:emma version="1.0" xmlns:emma="http://www.w3.org/2003/04/emma">
    <emma:interpretation  emma:offset-to-start="12345" emma:confidence="0.3">

      <semaine:gender name="female" xmlns:semaine="http://www.semaine-project.eu/semaineml"/>

    </emma:interpretation>
</emma:emma>
*/
  
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
  XMLTool::setPrefix(document->getDocumentElement(), "emma");

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

void cSemaineUserinfoSender::sendSpeakerId( cComponentMessage *msg )
{
/*
<emma:emma version="1.0" xmlns:emma="http://www.w3.org/2003/04/emma">
    <emma:interpretation  emma:offset-to-start="12345" emma:confidence="0.3">

      <semaine:speakerid id="1" name="spkA" modality="" xmlns:semaine="http://www.semaine-project.eu/semaineml"/>

    </emma:interpretation>
</emma:emma>
*/
  
  // fill XML document
  std::string idStr = "unknown";
  std::string idStrInt = "0";
  char strtmp2[50];
  if (msg->msgtext != NULL) {
    idStr = msg->msgtext;
  }
  sprintf(strtmp2, "%ld", msg->intData[0]);
  idStrInt = strtmp2;

  sprintf(strtmp2,"%ld",smileTimeToSemaineTime(msg->userTime1));
  std::string startTm(strtmp2);

  // Create and fill a simple EMMA EmotionML document
  XERCESC_NS::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
  XMLTool::setPrefix(document->getDocumentElement(), "emma");

  XERCESC_NS::DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
  XMLTool::setAttribute(interpretation, EMMA::A_OFFSET_TO_START, startTm);
  XMLTool::setAttribute(interpretation, EMMA::A_DURATION, "0");
  XMLTool::setPrefix(interpretation, "emma");

  XERCESC_NS::DOMElement * id = XMLTool::appendChildElement(interpretation, SemaineML::E_SPEAKER, SemaineML::namespaceURI);
  XMLTool::setAttribute(id, SemaineML::A_NAME, idStr);
  XMLTool::setAttribute(id, SemaineML::A_VALUE, idStrInt);
  if (msg->msgname[0] == 'a') {  // audio
    XMLTool::setAttribute(id, SemaineML::A_MODALITY, "audio");
  } else if (msg->msgname[0] == 'v') {  // video
    XMLTool::setAttribute(id, SemaineML::A_MODALITY, "video");
  }
  XMLTool::setPrefix(id, "semaine");

  // Now send it
  sendDocument(document);
}

void cSemaineUserinfoSender::sendPitchDirection( cComponentMessage *_msg )
{
/*
<emma:emma version="1.0" xmlns:emma="http://www.w3.org/2003/04/emma">
    <emma:interpretation  emma:offset-to-start="12345" emma:duration="444" emma:confidence="0.3">

        <semaine:pitch xmlns:semaine="http://www.semaine-project.eu/semaineml" direction="rise"/>

    </emma:interpretation>
</emma:emma>
*/

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
  XMLTool::setPrefix(document->getDocumentElement(), "emma");

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



int cSemaineUserinfoSender::processComponentMessage( cComponentMessage *msg ) 
{ 
  if (isMessageType(msg,"classificationResult")) {  
     if (!strcmp(msg->msgname, "gender")) { 
       sendGender(msg);
       return 1;
     } else if (!strcmp(msg->msgname, "offtalk")) {
       sendOffTalk(msg);
       return 1;
     }
  }
  else if (isMessageType(msg, "speakerId")) {  
    sendSpeakerId(msg);
    return 1;
  }
  else if (isMessageType(msg, "pitchDirection")) {  
    SMILE_IDBG(3, "received 'pitchDirection' message");
    sendPitchDirection(msg);
    return 1;  // message was processed
  }
  else if (isMessageType(msg, "turnSpeakingStatus")) {
    SMILE_IDBG(3, "received 'turnSpeakingStatus' message: %i", msg->intData[0]);
    sendSpeakingStatus(msg, msg->intData[0]);
    return 1; // message was processed
  }
  return 0; // if message was not processed
}  




#endif //HAVE_SEMAINEAPI

