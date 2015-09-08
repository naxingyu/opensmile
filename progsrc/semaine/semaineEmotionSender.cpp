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


#include <semaine/semaineEmotionSender.hpp>

#define MODULE "cSemaineEmotionSender"

#ifdef HAVE_SEMAINEAPI

SMILECOMPONENT_STATICS(cSemaineEmotionSender)

SMILECOMPONENT_REGCOMP(cSemaineEmotionSender)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CSEMAINEEMOTIONSENDER;
  sdescription = COMPONENT_DESCRIPTION_CSEMAINEEMOTIONSENDER;

  // configure your component's configType:

  SMILECOMPONENT_CREATE_CONFIGTYPE
  //ct->setField("reader", "dataMemory interface configuration (i.e. what slot to read from)", sconfman->getTypeObj("cDataReader"), NO_ARRAY, DONT_FREE);
  ct->setField("dummy","nothing",0);
  ct->setField("unpredictabilityStr","Name of the expectation / unpredictability class to invert (expectation -> unpredictability).","unpredictability");
  ct->setField("intensityStr","Name of the intensity class. This will be encoded differently in EmotionML than the four FSRE dimensions.","intensity");
  SMILECOMPONENT_IFNOTREGAGAIN( {} )

  SMILECOMPONENT_MAKEINFO(cSemaineEmotionSender);
}

SMILECOMPONENT_CREATE(cSemaineEmotionSender)



//-----

cSemaineEmotionSender::cSemaineEmotionSender(const char *_name) :
  cSemaineEmmaSender(_name)
{
}

void cSemaineEmotionSender::fetchConfig()
{
  cSemaineEmmaSender::fetchConfig();

  unpredictabilityStr = getStr("unpredictabilityStr");
  intensityStr = getStr("intensityStr");
}

void cSemaineEmotionSender::sendInterestC( cComponentMessage *_msg )
{
  char strtmp[50];
  sprintf(strtmp,"%.2f",_msg->floatData[0]);
  std::string interestStr(strtmp);

  // Create and fill a simple EMMA EmotionML document
  XERCESC_NS::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
  XMLTool::setPrefix(document->getDocumentElement(), "emma");

  sprintf(strtmp,"%ld",smileTimeToSemaineTime(_msg->userTime1));
  std::string startTm(strtmp);
  sprintf(strtmp,"%ld",(long long)round((_msg->userTime2 - _msg->userTime1)*1000.0));
  std::string duration(strtmp);

  XERCESC_NS::DOMElement * oneof = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_ONEOF);
  XMLTool::setAttribute(oneof, EMMA::A_OFFSET_TO_START, startTm);
  XMLTool::setAttribute(oneof, EMMA::A_DURATION, duration);
  XMLTool::setPrefix(oneof, "emma");

  double v = 0.0;
  //TODO: update this to new classification result message!  check that probEstim is not NULL....!
  XERCESC_NS::DOMElement * interpretation0 = XMLTool::appendChildElement(oneof, EMMA::E_INTERPRETATION);

  if (_msg->custData != NULL) v = ((double*)(_msg->custData))[0];
  sprintf(strtmp,"%.3f",v);
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
  if (_msg->custData != NULL) v = ((double*)(_msg->custData))[1];
  else v = 0;
  sprintf(strtmp,"%.3f",v);
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
  if (_msg->custData != NULL) v = ((double*)(_msg->custData))[2];
  else v = 0;
  sprintf(strtmp,"%.3f",v);
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

void cSemaineEmotionSender::sendDimension( cComponentMessage *_msg, std::string dimensionStr, std::string dimensionVoc )
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
  XMLTool::setPrefix(document->getDocumentElement(), "emma");

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

void cSemaineEmotionSender::sendDimensionsFSRE_I( cComponentMessage *_msg )
{
  // range check:
  //if (_msg->floatData[0] < 0) _msg->floatData[0] = 0.0;
  //if (_msg->floatData[0] > 1) _msg->floatData[0] = 1.0;
  //--

  char strtmp[50];
  sprintf(strtmp,"%.2f",_msg->floatData[0]);
  std::string dimVal(strtmp);

  sprintf(strtmp,"%ld",smileTimeToSemaineTime(_msg->userTime1));
  std::string startTm(strtmp);
  sprintf(strtmp,"%ld",(long long)round((_msg->userTime2 - _msg->userTime1)*1000.0));
  std::string duration(strtmp);

  sprintf(strtmp,"%s",(const char *)(_msg->msgname));
  std::string codername(strtmp);

  // Create and fill a simple EMMA EmotionML document
  XERCESC_NS::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
  XMLTool::setPrefix(document->getDocumentElement(), "emma");

  XERCESC_NS::DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
  XMLTool::setAttribute(interpretation, EMMA::A_OFFSET_TO_START, startTm);
  XMLTool::setAttribute(interpretation, EMMA::A_DURATION, duration);
  XMLTool::setAttribute(interpretation, EMMA::A_CONFIDENCE, "1.0");
  XMLTool::setPrefix(interpretation, "emma");
  
  XERCESC_NS::DOMElement * emotion = XMLTool::appendChildElement(interpretation, EmotionML::E_EMOTION, EmotionML::namespaceURI);
  XMLTool::setAttribute(emotion, EmotionML::A_DIMENSION_VOCABULARY, EmotionML::VOC_FSRE_DIMENSION_DEFINITION);
  XMLTool::setAttribute(emotion, EmotionML::A_MODALITY, "voice");
  XMLTool::setPrefix(emotion, "emotion");

  int i; int intIdx = -1;
  sClassifierResults * re = ((sClassifierResults*)(_msg->custData));
  for (i=0; i<re->nFilled; i++) {
    if (!strcmp(re->resnameA[i],intensityStr)) {
      intIdx = i; continue;
    }

    char strtmp[50];
    if (!strcmp(re->resnameA[i],unpredictabilityStr)) {
      re->res[i] = (1.0 - re->res[i])/2.0;   //// not nice hack...
    } else {
      re->res[i] = (re->res[i] + 1.0)/2.0;
    } 
    if (re->res[i] < 0.0) re->res[i] = 0.0;
    if (re->res[i] > 1.0) re->res[i] = 1.0;

    sprintf(strtmp,"%.2f",re->res[i]);
    std::string dimVal(strtmp);
    sprintf(strtmp,"%s",re->resnameA[i]);
    std::string dimStr(strtmp);

    XERCESC_NS::DOMElement * dimension = XMLTool::appendChildElement(emotion, EmotionML::E_DIMENSION, EmotionML::namespaceURI);
    XMLTool::setAttribute(dimension, EmotionML::A_NAME, dimStr); // dimensionStr
    XMLTool::setAttribute(dimension, EmotionML::A_VALUE, dimVal);
    XMLTool::setPrefix(dimension, "emotion");
  }

  XERCESC_NS::DOMElement * info = XMLTool::appendChildElement(emotion, EmotionML::E_INFO);
  XERCESC_NS::DOMElement * coder = XMLTool::appendChildElement(info, "predictor");
  XMLTool::setAttribute(coder, "value", codername);
  XMLTool::setPrefix(coder, "emotion");
  XMLTool::setPrefix(info, "emotion");

  if (intIdx >= 0) {
    XERCESC_NS::DOMElement * intensity = XMLTool::appendChildElement(interpretation, EmotionML::E_EMOTION, EmotionML::namespaceURI);
    XMLTool::setAttribute(intensity, EmotionML::A_DIMENSION_VOCABULARY, EmotionML::VOC_SEMAINE_INTENSITY_DIMENSION_DEFINITON);
    XMLTool::setAttribute(intensity, EmotionML::A_MODALITY, "voice");
    XMLTool::setPrefix(emotion, "emotion");

    sprintf(strtmp,"%.2f",re->res[intIdx]);
    std::string dimVal(strtmp);
    sprintf(strtmp,"%s",re->resnameA[intIdx]);
    std::string dimStr(strtmp);

    XERCESC_NS::DOMElement * idim = XMLTool::appendChildElement(intensity, EmotionML::E_DIMENSION, EmotionML::namespaceURI);
    XMLTool::setAttribute(idim, EmotionML::A_NAME, dimStr); // dimensionStr
    XMLTool::setAttribute(idim, EmotionML::A_VALUE, dimVal);
    XMLTool::setPrefix(idim, "emotion");

    XERCESC_NS::DOMElement * iinfo = XMLTool::appendChildElement(intensity, EmotionML::E_INFO);
    XERCESC_NS::DOMElement * icoder = XMLTool::appendChildElement(iinfo, "predictor");
    XMLTool::setAttribute(icoder, "value", codername);
    XMLTool::setPrefix(icoder, "emotion");
    XMLTool::setPrefix(iinfo, "emotion");
  }

  sendDocument(document);
}

int cSemaineEmotionSender::processComponentMessage( cComponentMessage *_msg ) 
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
    //else if (!strcmp(_msg->msgname,"gender")) sendGender(_msg);
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

  if (isMessageType(_msg,"classificationResults")) {  
    sendDimensionsFSRE_I(_msg);
    //TODO:    sendDimensions(_msg,EmotionML::VOC_SEMAINE_INTENSITY_DIMENSION_INTENSITY);
  }
  
  return 0; // if message was not processed
}  




#endif //HAVE_SEMAINEAPI

