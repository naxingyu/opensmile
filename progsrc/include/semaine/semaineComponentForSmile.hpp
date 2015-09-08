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

#ifndef SEMAINE_COMPONENT_FOR_SMILE_H
#define SEMAINE_COMPONENT_FOR_SMILE_H

#include <semaine/config.h>

#include <cms/CMSException.h>

#include <semaine/components/Component.h>
#include <semaine/cms/sender/FeatureSender.h>
#include <semaine/cms/sender/EmmaSender.h>

#include <core/smileCommon.hpp>

#include <core/configManager.hpp>
#include <core/commandlineParser.hpp>
#include <core/componentManager.hpp>
#include <semaine/activeMqSink.hpp>
#include <semaine/activeMqSource.hpp>
#include <semaine/semaineEmmaSender.hpp>

using namespace cms;
using namespace semaine::components;
//using namespace tum::components:smile;
//using namespace semaine::cms::sender;


inline void registerComponentForSmileConfig(cConfigManager *_conf)
{
  if (_conf != NULL) {
    // TOOD: check if configType already exists....!

    ConfigType * cte= new ConfigType("messageSenderCfg");
    cte->setField("OSinstanceName","openSMILE component instance name of this EMMA message sender.","semaineEmmaSender");
    cte->setField("topic","The topic this sender sends its messages to.","semaine.data.state.user.emma");
    cte->setField("componentName","The name of the virtual component, which is the sender's source.","TumOpenSMILE");


    ConfigType * ct= new ConfigType("semaineCfg");
    //ct->setField("audioReceiver","Instance name of the cActiveMqSource openSmile component to read audio output.",(const char *)NULL);
    ct->setField("outputVAtopic","Topic to read voice output activity status messages from (for voice feedback cancellation).",(const char *)NULL);
    ct->setField("callbackRecv","receipient smile components for semaine callback messages (e.g. from player), separate multiple receipients by a comma ','.",(const char *)NULL);

    ct->setField("featureSender","Instance name of the cActiveMqSink openSmile component for sending low-level features.",(const char *)NULL);
    ct->setField("featureSenderSourceName","Instance name of the cActiveMqSink openSmile component for sending low-level features.",(const char *)NULL);
    ct->setField("featureTopic","Topic the featureSender component should send its data to.",(const char *)NULL);

    ct->setField("wordSender","Configuration of EMMA message sender for ASR word (verbal) output.",cte);
    ct->setField("nonverbalSender","Configuration of EMMA message sender for ASR non-linguistics (non-verbal) output.",cte);
    ct->setField("emotionSender","Configuration of EMMA message sender for dimensional affect output.",cte);
    ct->setField("userinfoSender","Configuration of EMMA message sender for user info such as speaking status and gender.",cte);

    ConfigInstance *ci = new ConfigInstance("semaineCfg",ct,1);
    _conf->registerType(ci);
  }
}

#undef class

class sMessageSenderInfo {
public:
  const char * OSinstanceName;
  const char * topic;
  const char * componentName; // source name
};

class ComponentForSmile : public Component
{
public:
  ComponentForSmile(const std::string & componentName, cComponentManager *_cMan, cConfigManager *_conf, bool isInput=false, bool isOutput=false) throw (CMSException) :
      Component(componentName, isInput, isOutput),
        cMan(_cMan)
      {

        const char * cfgInstName = "semaine";

        // read destination (W) and source (R) topic names 
        outputVAtopic = _conf->getStr_f(myvprint("%s.outputVAtopic",cfgInstName));
        featureTopic = _conf->getStr_f(myvprint("%s.featureTopic",cfgInstName));
        
        //audioReceiver = _conf->getStr_f(myvprint("%s.audioReceiver",cfgInstName));
        featureSenderOS = _conf->getStr_f(myvprint("%s.featureSender",cfgInstName));
        featureSenderSourceName = _conf->getStr_f(myvprint("%s.featureSenderSourceName",cfgInstName));

        wordSenderInfo.OSinstanceName = _conf->getStr_f(myvprint("%s.wordSender.OSinstanceName",cfgInstName));
        wordSenderInfo.topic = _conf->getStr_f(myvprint("%s.wordSender.topic",cfgInstName));
        wordSenderInfo.componentName = _conf->getStr_f(myvprint("%s.wordSender.componentName",cfgInstName));
        
        nonverbalSenderInfo.OSinstanceName = _conf->getStr_f(myvprint("%s.nonverbalSender.OSinstanceName",cfgInstName));
        nonverbalSenderInfo.topic = _conf->getStr_f(myvprint("%s.nonverbalSender.topic",cfgInstName));
        nonverbalSenderInfo.componentName = _conf->getStr_f(myvprint("%s.nonverbalSender.componentName",cfgInstName));

        emotionSenderInfo.OSinstanceName = _conf->getStr_f(myvprint("%s.emotionSender.OSinstanceName",cfgInstName));
        emotionSenderInfo.topic = _conf->getStr_f(myvprint("%s.emotionSender.topic",cfgInstName));
        emotionSenderInfo.componentName = _conf->getStr_f(myvprint("%s.emotionSender.componentName",cfgInstName));

        userinfoSenderInfo.OSinstanceName = _conf->getStr_f(myvprint("%s.userinfoSender.OSinstanceName",cfgInstName));
        userinfoSenderInfo.topic = _conf->getStr_f(myvprint("%s.userinfoSender.topic",cfgInstName));
        userinfoSenderInfo.componentName = _conf->getStr_f(myvprint("%s.userinfoSender.componentName",cfgInstName));

        // get openSMILE component pointers by name from _cMan
        /* old code, this was moved to customStartIO
        if (cMan != NULL) {
          if (asink != NULL) setSmileAMQsink(cMan->getComponentInstance(asink));
          if (asrc != NULL) setSmileAMQsource(cMan->getComponentInstance(asrc)); 
          if (emmas != NULL) setSmileEMMAsender(cMan->getComponentInstance(emmas)); 
        }
        */

      }

      virtual ~ComponentForSmile() {}

      int setSmileAMQsink(const char * _instanceName, FeatureSender *_sender) 
      { 
        cSmileComponent *co = cMan->getComponentInstance(_instanceName);
        if (co != NULL) {
          ((cActiveMqSink *)co)->setFeatureSender(_sender,&meta);
          return 1;
        } else {
          printf("featureSender == NULL in TumFeatureExtractor, please check semaineCfg section in config file (no features will be sent now!).\n");
        }
        return 0;
      }
      
      ///????
      /*
      int setSmileAMQsource(cSmileComponent *_amqsource) { 
        return ((amqsource = (cActiveMqSource *)_amqsource) != NULL); 
      }*/
      
      int setSmileEMMAsender(const char * _instanceName, EmmaSender * _sender) 
      { 
        cSmileComponent * co = cMan->getComponentInstance(_instanceName);
        if (co !=  NULL) {
          //return ((emmasender = (cSemaineEmmaSender *)_emmas) != NULL); 
          ((cSemaineEmmaSender *)co)->setEmmaSender(_sender,&meta);
          return 1;
        } else {
          printf("cannot find emma sender component '%s' in componentManager!\n",_instanceName);
        }
        return 0;
      }

protected:
  virtual void act() throw (CMSException) {}
  virtual void react(SEMAINEMessage * message) throw (std::exception) {}
  virtual void customStartIO() throw (CMSException) {}

  const char * featureTopic;   // topic to write feature data to
  const char * outputVAtopic;   // topic to read feature data from  (could also be wave data or any other binary data)

  //const char *audioReceiver; // asrc
  const char *featureSenderOS; // asink
  const char *featureSenderSourceName;

  cComponentManager *cMan;
  cActiveMqSink *featureSender;
  //cActiveMqSource *amqsource;

  /*
  cSemaineWordSender * wordSender;
  cSemaineNonverbalSender * nonverbalSender;
  cSemaineEmotionSender * emotionSender;
  cSemaineUserinfoSender * userinfoSender;
*/
  //FeatureSender * featureSender;
  /*EmmaSender * wordSender;
  EmmaSender * nonverbalSender;
  EmmaSender * emotionSender;
  EmmaSender * userinfoSender;*/

  sMessageSenderInfo wordSenderInfo;
  sMessageSenderInfo nonverbalSenderInfo;
  sMessageSenderInfo emotionSenderInfo;
  sMessageSenderInfo userinfoSenderInfo;

private:
};

#endif //SEMAINE_COMPONENT_FOR_SMILE_H


