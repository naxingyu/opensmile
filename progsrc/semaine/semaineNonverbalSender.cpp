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


#include <semaine/semaineNonverbalSender.hpp>
//#include <math.h>

#define MODULE "cSemaineNonverbalSender"

#ifdef HAVE_SEMAINEAPI

SMILECOMPONENT_STATICS(cSemaineNonverbalSender)

SMILECOMPONENT_REGCOMP(cSemaineNonverbalSender)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CSEMAINENONVERBALSENDER;
  sdescription = COMPONENT_DESCRIPTION_CSEMAINENONVERBALSENDER;

  // configure your component's configType:

  SMILECOMPONENT_CREATE_CONFIGTYPE
  //ct->setField("reader", "dataMemory interface configuration (i.e. what slot to read from)", sconfman->getTypeObj("cDataReader"), NO_ARRAY, DONT_FREE);
  ct->setField("dummy","nothing",0);
  ct->setField("laughterString","laughter symbol as output by the ASR engine","*laugh*");
  ct->setField("sighString","sigh symbol as output by the ASR engine","*sigh*");
  ct->setField("breathString","breath symbol as output by the ASR engine","*breathe*");
  SMILECOMPONENT_IFNOTREGAGAIN( {} )

  SMILECOMPONENT_MAKEINFO(cSemaineNonverbalSender);
}

SMILECOMPONENT_CREATE(cSemaineNonverbalSender)



//-----

cSemaineNonverbalSender::cSemaineNonverbalSender(const char *_name) :
  cSemaineEmmaSender(_name)
{
}

void cSemaineNonverbalSender::fetchConfig() 
{
  cSemaineEmmaSender::fetchConfig();

  laughterString = getStr("laughterString");
  sighString = getStr("sighString");
  breathString = getStr("breathString");
}

// this includes the juliusResult struct/class definition
#include <classifiers/julius/kwsjKresult.h>

void cSemaineNonverbalSender::sendNonverbals( cComponentMessage *_msg )
{
  int i;
  juliusResult *k = (juliusResult *)(_msg->custData);
  if (k==NULL) return;

  int nNv = 0;
  for (i=0; i<k->numW; i++) {
    // check for non-verbals.... and remove them, only preceed if words are left
    if (k->word[i][0] == '*') nNv++;
  }
  if (nNv == 0) return;

  char strtmp[150];
  sprintf(strtmp,"%.2f",_msg->floatData[0]);
  std::string valStr(strtmp);
  long long startTime = smileTimeToSemaineTime(_msg->userTime1);
  sprintf(strtmp,"%ld",startTime);
  std::string startTm(strtmp);
  sprintf(strtmp,"%ld",(long long)round((_msg->userTime2 - _msg->userTime1)*1000.0));
  std::string duration(strtmp);

  for (i=0; i<k->numW; i++) {
    if (k->word[i][0] == '*') {

      // Create and fill a simple EMMA document
      XERCESC_NS::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
      XMLTool::setPrefix(document->getDocumentElement(), "emma");

      /*
      XERCESC_NS::DOMElement * sequence = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_SEQUENCE);
      XMLTool::setAttribute(sequence, EMMA::A_OFFSET_TO_START, startTm);
      XMLTool::setAttribute(sequence, EMMA::A_DURATION, duration);
      XMLTool::setPrefix(sequence, "emma");
    */
      sprintf(strtmp,"%ld",startTime + (long long)round((k->start[i])*1000.0));
      std::string offs(strtmp);
      sprintf(strtmp,"%.3f",k->conf[i]);
      std::string confidence(strtmp);

      XERCESC_NS::DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
      XMLTool::setAttribute(interpretation, EMMA::A_OFFSET_TO_START, offs);
      XMLTool::setAttribute(interpretation, EMMA::A_CONFIDENCE, confidence);
      XMLTool::setPrefix(interpretation, "emma");
      XERCESC_NS::DOMElement * vocal = XMLTool::appendChildElement(interpretation, SemaineML::E_VOCALIZATION, SemaineML::namespaceURI);

      if ((laughterString!=NULL)&&(!strcmp(k->word[i],laughterString))) {
        XMLTool::setAttribute(vocal, SemaineML::A_NAME, SemaineML::V_LAUGHTER);
      } else if ((sighString!=NULL)&&(!strcmp(k->word[i], sighString))) {
        XMLTool::setAttribute(vocal, SemaineML::A_NAME, SemaineML::V_SIGH);
      } else if ((breathString!=NULL)&&(!strcmp(k->word[i], breathString))) {
        XMLTool::setAttribute(vocal, SemaineML::A_NAME, SemaineML::V_BREATH);
      }
      XMLTool::setPrefix(vocal, "semaine");

      // Now send it
      sendDocument(document);
    }
  }


 
}




int cSemaineNonverbalSender::processComponentMessage( cComponentMessage *_msg ) 
{ 
  if (isMessageType(_msg,"asrKeywordOutput")) {  
    SMILE_IDBG(3,"received 'asrKeywordOutput' message");
    sendNonverbals(_msg);
    return 1;  // message was processed
  }
  
  return 0; // if message was not processed
}  




#endif //HAVE_SEMAINEAPI

