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


#include <semaine/TumNonverbalFeatureExtractor.h>

#undef MODULE
#define MODULE "TumNonverbalFeatureExtractor.SEMAINEcomponent"

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


    TumNonverbalFeatureExtractor::TumNonverbalFeatureExtractor(cComponentManager *_cMan, cConfigManager *_conf) throw(CMSException) : 
      ComponentForSmile("TumNonverbalFeatureExtractor",_cMan,_conf,true,false)
    {
      if (userinfoSenderInfo.topic != NULL) {
        userinfoSender = new EmmaSender(userinfoSenderInfo.topic, userinfoSenderInfo.componentName );
        senders.push_back(userinfoSender);
      } else {
        // TODO: use semaine exception here...
        SMILE_WRN(1,"userinfoSender.topic == NULL in TumFeatureExtractor, please check semaineCfg section in config file (no emma XML messages with user speaking status and gender will be sent!).");
      }
      
      if (nonverbalSenderInfo.topic != NULL) {
        nonverbalSender = new EmmaSender(nonverbalSenderInfo.topic, nonverbalSenderInfo.componentName );
        senders.push_back(nonverbalSender);
      } else {
        // TODO: use semaine exception here...
        SMILE_WRN(1,"nonverbalSender.topic == NULL in TumFeatureExtractor, please check semaineCfg section in config file (no emma XML messages with non-linguistic vocalisation will be sent!).");
      }



    }



    TumNonverbalFeatureExtractor::~TumNonverbalFeatureExtractor()
    {
      if (cMan != NULL) {
        //cMan->requestAbort();
        //smileThreadJoin( smileMainThread );
      }
    }

    void TumNonverbalFeatureExtractor::customStartIO() throw(CMSException)
    {
      if (cMan == NULL) { SMILE_ERR(1,"componentManager (cMan) is NULL, smileMainThread can not be started!"); }
      else {
        // main openSMILE initialisation
        //cMan->createInstances(0); // 0 = do not read config (we already did that ..)

        //wait for component manager to become ready....
        while (!cMan->isReady()) { smileSleep(100); } // TODO: make the isReady variable in cMan thread safe...!

        /* connect all the feature senders / receivers, etc. */
        setSmileEMMAsender(nonverbalSenderInfo.OSinstanceName, nonverbalSender);
        setSmileEMMAsender(userinfoSenderInfo.OSinstanceName, userinfoSender);
      }
      
    }

    void TumNonverbalFeatureExtractor::react(SEMAINEMessage * m) throw (std::exception) 
    {
    }

    void TumNonverbalFeatureExtractor::act() throw(CMSException)
    {
    }


    } // namespace smile
  } // namespace components
} // namespace semaine


#endif // HAVE_SEMAINEAPI
