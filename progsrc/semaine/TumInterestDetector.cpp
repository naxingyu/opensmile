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
*  TumOpenSMILE.cpp
*  semaine
*  
*/
#include <core/smileCommon.hpp>

#ifdef HAVE_SEMAINEAPI


#include <semaine/TumInterestDetector.h>

#undef MODULE
#define MODULE "TumInterestDetector.SEMAINEcomponent"

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


    TumInterestDetector::TumInterestDetector(cComponentManager *_cMan, cConfigManager *_conf) throw(CMSException) : 
      ComponentForSmile("TumInterestDetector",_cMan,_conf,true,false)
    {
      if (emotionSenderInfo.topic != NULL) {
        emotionSender = new EmmaSender(emotionSenderInfo.topic, emotionSenderInfo.componentName );
        senders.push_back(emotionSender);
      } else {
        // TODO: use semaine exception here...
        SMILE_WRN(1,"emotionSender.topic == NULL in TumFeatureExtractor, please check semaineCfg section in config file (no emma XML messages with dimensional affect recognition results will be sent!).");
      }

      // Marc, 21 Dec 08: Deactivated, because it leads to dropped messages when the ASR
      // is on a different machine where the clock time is not the same.
      //featureSender->setTimeToLive(100); // discard messages after 100 ms

    }




    TumInterestDetector::~TumInterestDetector()
    {
      if (cMan != NULL) {
        //cMan->requestAbort();
        //smileThreadJoin( smileMainThread );
      }
    }

    void TumInterestDetector::customStartIO() throw(CMSException)
    {
      if (cMan == NULL) { SMILE_ERR(1,"componentManager (cMan) is NULL, smileMainThread can not be started!"); }
      else {
        // main openSMILE initialisation
        //cMan->createInstances(0); // 0 = do not read config (we already did that ..)

        // wait for component manager to become ready....
        while (!cMan->isReady()) { smileSleep(100); } // TODO: make the isReady variable in cMan thread safe...!

        /* connect all the feature senders / receivers, etc. */
        // NOTE: The emotion analyser already sets this.... (can we override it??)
        setSmileEMMAsender(emotionSenderInfo.OSinstanceName, emotionSender);

      }
      
    }

    void TumInterestDetector::react(SEMAINEMessage * m) throw (std::exception) 
    {
  
    }

    void TumInterestDetector::act() throw(CMSException)
    {
  
    }


    } // namespace smile
  } // namespace components
} // namespace semaine


#endif // HAVE_SEMAINEAPI
