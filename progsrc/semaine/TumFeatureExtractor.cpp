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


/*
*  TumFeatureExtractor.cpp
*  semaine
*  
*/
#include <core/smileCommon.hpp>

#ifdef HAVE_SEMAINEAPI


#include <semaine/TumFeatureExtractor.h>

#undef MODULE
#define MODULE "TumFeatureExtractor"

#include <cstdlib>
#include <sstream>

#include <semaine/util/XMLTool.h>
#include <semaine/datatypes/xml/EMMA.h>
#include <semaine/datatypes/xml/EmotionML.h>
#include <semaine/datatypes/xml/SemaineML.h>

using namespace semaine::util;
using namespace semaine::datatypes::xml;
using namespace XERCES_CPP_NAMESPACE;

namespace semaine {
  namespace components {
    namespace smile {


    TumFeatureExtractor::TumFeatureExtractor(cComponentManager *_cMan, cConfigManager *_conf) throw(CMSException) : 
      ComponentForSmile("TumFeatureExtractor",_cMan,_conf,true,false),
      callbackRecv(NULL), emmaSender(NULL), featureSender(NULL), callbackReceiver(NULL)
    {
      if (_conf->isSet_f(myvprint("semaine.callbackRecv[%s]",getName().c_str()))) callbackRecv = _conf->getStr_f(myvprint("semaine.callbackRecv[%s]",getName().c_str()));

      if ((topicR != NULL)) {
        // check for multiple read topics separated by topicR
        char * tr = strdup(topicR);
        char * tmp = tr;
        char * x = NULL;
        do {
          x = (char *)strchr(tmp, ',');
          // separate at ','
          if (x!=NULL) {
            *x = 0;
          }

          // remove spaces
          while (*tmp==' ') { tmp++; }
          size_t l = strlen(tmp);
          while (tmp[l-1] == ' ') { tmp[l-1] = 0; l--; }

          // push-back a callback receiver
          callbackReceiver = new XMLReceiver(tmp);
          receivers.push_back(callbackReceiver);
          printf("Receiver: '%s' \n",tmp);

          // analyse next part of string, if present
          if (x != NULL) {
            tmp = x+1;
          }
        } while (x !=  NULL);
        free(tr);
      } else {
        SMILE_WRN(1,"topicR == NULL in TumFeatureExtractor, please check semaineCfg section in config file (no voice data will be read from speech output for echo compensation).");
      }

      int period = 10; // ms

      if (topicW != NULL) {
        featureSender = new FeatureSender(topicW, "", getName(), period);
        //	featureSender = new FeatureSender("semaine.data.analysis.features.voice", "", getName(), period);

        waitingTime = period;
        senders.push_back(featureSender);
      } else {
        // TODO: use semaine exception here...
        SMILE_WRN(1,"topicW == NULL in TumFeatureExtractor, please check semaineCfg section in config file (no features will be sent now!).");
      }

      if (topicEmma != NULL) {
        emmaSender = new EmmaSender(topicEmma, getName());
        senders.push_back(emmaSender);
      } else {
        // TODO: use semaine exception here...
        SMILE_WRN(1,"topicEmma == NULL in TumFeatureExtractor, please check semaineCfg section in config file (no emma XML messages will be sent!).");
      }

      // Marc, 21 Dec 08: Deactivated, because it leads to dropped messages when the ASR
      // is on a different machine where the clock time is not the same.
      //featureSender->setTimeToLive(100); // discard messages after 100 ms

    }



    TumFeatureExtractor::~TumFeatureExtractor()
    {
      if (cMan != NULL) {
        cMan->requestAbort();
        smileThreadJoin( smileMainThread );
      }
    }

    SMILE_THREAD_RETVAL smileThreadRunner(void *_obj)
    {
      cComponentManager * __obj = (cComponentManager *)_obj;
      if (_obj != NULL) {
        __obj->runMultiThreaded(-1);
      }
      SMILE_THREAD_RET;
    }

    void TumFeatureExtractor::customStartIO() throw(CMSException)
    {
      if (cMan == NULL) { SMILE_ERR(1,"componentManager (cMan) is NULL, smileMainThread can not be started!"); }
      else {
        // main openSMILE initialisation
        cMan->createInstances(0); // 0 = do not read config (we already did that ..)

        /* connect all the feature senders / receivers, etc. */

        // get openSMILE component pointers by name from _cMan
        if (asink != NULL) setSmileAMQsink(cMan->getComponentInstance(asink));
        if (asrc != NULL) setSmileAMQsource(cMan->getComponentInstance(asrc)); 
        if (emmas != NULL) setSmileEMMAsender(cMan->getComponentInstance(emmas)); 

        if (amqsink != NULL) {
          amqsink->setFeatureSender(featureSender,&meta);
        } else {
          SMILE_WRN(1,"amqsink == NULL in TumFeatureExtractor, please check semaineCfg section in config file (no features will be sent now!).");
        }

        if (emmasender != NULL) {
          emmasender->setEmmaSender(emmaSender,&meta);
        } else {
          SMILE_WRN(1,"emmasender == NULL in TumFeatureExtractor, please check semaineCfg section in config file (no emma XML messages will be sent!).");
        }

        // start the smile main thread, and call run
        smileThreadCreate( smileMainThread, smileThreadRunner, (void*)cMan  );
      }
      
    }

    void TumFeatureExtractor::react(SEMAINEMessage * m) throw (std::exception) 
    {
      // use amqsource->writer to save data to dataMemory  (if configured to use source..)
      /*
      if (amqsource == NULL) {
      // TODO: use semaine exception here...
      SMILE_WRN(1,"amqsource == NULL in TumFeatureExtractor, please check semaineCfg section in config file (discarding received data!).");
      return;
      }
      */

      /*
      cDataWriter *writer = amqsource->getWriter();
      cVector *vec = amqsource->getVec();
      */

      // TOOD: parse Semaine Message and write features to dataMemory...
      // Problem: featureNames.....!! We must assign generic names like "feature01" to "featureNN" and update them after the first message.... yet, we also don't know how many features we will be receiving, if we have not received the first message... BIG PROBLEM!!

      printf("GOT MESSAGE '%i'\n",m); fflush(stdout);

        /*printf("y\n"); fflush(stdout);
        std::string a = m->getText();
        printf("b\n"); fflush(stdout);
        const char *ctype = a.c_str();
        printf("a\n"); fflush(stdout);
        printf("msg e type='%s'\n",ctype); fflush(stdout);*/

      SEMAINEXMLMessage * xm = dynamic_cast<SEMAINEXMLMessage *>(m);
      if (xm == NULL) {
        printf("xml message err '%s'\n",std::string(typeid(*m).name()));
        throw MessageFormatException("expected XML message, got "+std::string(typeid(*m).name()));
      }

      XERCESC_NS::DOMDocument * doc = xm->getDocument();
      XERCESC_NS::DOMElement * callback = doc->getDocumentElement();
      if (XMLTool::getNamespaceURI(callback) != SemaineML::namespaceURI ||
        XMLTool::getLocalName(callback) != "callback") {
          // check for userPresence message
          if (XMLTool::getNamespaceURI(callback) == SemaineML::namespaceURI ||
            XMLTool::getLocalName(callback) == "situational-context") {
              printf("userPresence msg\n");
              XERCESC_NS::DOMElement * user = XMLTool::getChildElementByLocalNameNS(callback, SemaineML::E_USER, SemaineML::namespaceURI);
              if (user != NULL) {
                std::string status = XMLTool::getAttribute(user, "status");
                const char *ctype = status.c_str();
                printf("msg type='%s'\n",ctype);
                //char *ctime = timeString.cStr();
                cComponentMessage callbackMsg("semaineCallback",ctype);
                callbackMsg.sender = "TumFeatureExtractor";
                cMan->sendComponentMessage(callbackRecv,&callbackMsg);

              }
          } else {
            // else:
            printf("xml message err, exp callback \n");
            throw MessageFormatException("Expected callback message, but found "+ XMLTool::getLocalName(callback) + " element in namespace " + XMLTool::getNamespaceURI(callback));
          }

      } else { // process callback

        XERCESC_NS::DOMElement * event = XMLTool::getChildElementByLocalNameNS(callback, SemaineML::E_EVENT, SemaineML::namespaceURI);
        if (event != NULL) {
          std::string type = XMLTool::getAttribute(event, "type");
          std::string timeString = XMLTool::getAttribute(event, SemaineML::A_TIME);
          const char *ctype = type.c_str();
          printf("msg type='%s'\n",ctype);
          //char *ctime = timeString.cStr();
          cComponentMessage callbackMsg("semaineCallback",ctype);
          callbackMsg.sender = "TumFeatureExtractor";
          cMan->sendComponentMessage(callbackRecv,&callbackMsg);

        }

      }

      /*
      SEMAINEFeatureMessage * fm = dynamic_cast<SEMAINEFeatureMessage *>(m);
      if (fm != NULL) {
      std::vector<float> features = fm->getFeatureVector();
      // TODO: get vecsize!!

      int i;
      for (i=0; i<vec->N; i++) { // limit to vecsize! !! TODO
      mat->dataF[i] = (FLOAT_DMEM)(features[i]);
      }
      writer->setNextFrame(vec);
      //TODO??
      }
      */	

    }

    void TumFeatureExtractor::act() throw(CMSException)
    {
      //	SMILE_DBG(1,"act() called!");

      // NOTE: act is unused here, since the activeMqSink components will handle the sending of data directly...


      //------------------------------------------------------------------------------------------------
      //	SMILE_DBG(9,"calling getFrame (id1=%i)",framerId1);

      // ++AMQ++  send (arousal, valence, interest) as EMMA
      /*
      char strtmp[50];
      sprintf(strtmp,"%.2f",a);
      std::string aroStr(strtmp);
      sprintf(strtmp,"%.2f",v);
      std::string valStr(strtmp);
      sprintf(strtmp,"%1.0f",i);
      std::string interestStr(strtmp);

      // Create and fill a simple EMMA EmotionML document
      DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
      DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
      XMLTool::setAttribute(interpretation, EMMA::A_START, "time not implemented");
      DOMElement * emotion = XMLTool::appendChildElement(interpretation, EmotionML::E_EMOTION, EmotionML::namespaceURI);
      if ((svmPredA != NULL)||(svmPredV != NULL)) {
      DOMElement * dimensions = XMLTool::appendChildElement(emotion, EmotionML::E_DIMENSIONS, EmotionML::namespaceURI);
      XMLTool::setAttribute(dimensions, EmotionML::A_SET, "valenceArousalPotency");
      if (svmPredA != NULL) {
      DOMElement * arousal = XMLTool::appendChildElement(dimensions, EmotionML::E_AROUSAL, EmotionML::namespaceURI);
      XMLTool::setAttribute(arousal, EmotionML::A_VALUE, aroStr);
      }
      if (svmPredV != NULL) {
      DOMElement * valence = XMLTool::appendChildElement(dimensions, EmotionML::E_VALENCE, EmotionML::namespaceURI);
      XMLTool::setAttribute(valence, EmotionML::A_VALUE, valStr);
      }
      }
      if (svmPredI != NULL) {
      DOMElement * category = XMLTool::appendChildElement(emotion, EmotionML::E_CATEGORY, EmotionML::namespaceURI);
      XMLTool::setAttribute(category, EmotionML::A_NAME, "interest");
      XMLTool::setAttribute(category, EmotionML::A_VALUE, interestStr);
      }

      // Now send it
      emmaSender->sendXML(document, meta.getTime());
      */

      /*}    else {
      // TODO: skip frame...
      }
      turntime = 0;


      }
      }
      #endif

      }
      */


    }


    } // namespace smile
  } // namespace components
} // namespace semaine


#endif // HAVE_SEMAINEAPI
