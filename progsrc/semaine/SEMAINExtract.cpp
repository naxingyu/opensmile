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
 


/****

This is the main file of the smile SEMAINE component.
It handles the option parsing and creates an instance of
the semaine::components::smile::TumFeatureExtractor class
and runs the main loop in a separate thread.

****/


#include <iostream>
#include <stdio.h>
#include <list>
#include <core/smileCommon.hpp>

#ifdef HAVE_SEMAINEAPI

#ifndef _MSC_VER
#include <semaine/config.h>
#endif

#include <semaine/util/XMLTool.h>
#include <semaine/components/Component.h>
#include <semaine/system/ComponentRunner.h>

#include <activemq/library/ActiveMQCPP.h>

//#include <opensmile/TumFeatureExtractor.h>
#include <semaine/TumKeywordSpotter.h>
#include <semaine/TumAudioFeatureExtractor.h>
#include <semaine/TumEmotionAnalyser.h>
#include <semaine/TumInterestDetector.h>
#include <semaine/TumNonverbalFeatureExtractor.h>

#define MODULE "SEMAINExtract"

/************** Ctrl+C signal handler **/
#include  <signal.h>

cComponentManager *cmanGlob = NULL;
semaine::components::smile::TumAudioFeatureExtractor *semaineAPIglobal = NULL;

void INThandler(int);
int ctrlc = 0;

void INThandler(int sig)
{
  //  char  c;
  signal(sig, SIG_IGN);
  if (cmanGlob != NULL) cmanGlob->requestAbort();
  if (semaineAPIglobal != NULL) semaineAPIglobal->requestExit();
  printf("CTRL+C pressed.\n");
  signal(SIGINT, INThandler);
  ctrlc = 1;
}
/*******************************************/



int main (int argc, char *argv[]) {

  cConfigManager *configManager = NULL;
  cComponentManager *cMan = NULL;
  cCommandlineParser cmdline(argc,argv);

  try {

    // setup the openSMILE logger
    LOGGER.setLogFile("smile.log");
    LOGGER.setLogLevel(1);
    LOGGER.enableConsoleOutput();

    // openSMILE commandline parser:
    cmdline.addStr( "configfile", 'C', "Path to openSMILE config file", "smile.conf" );
    cmdline.addInt( "loglevel", 'l', "Verbosity level (0-9)", 2 );
    cmdline.addInt( "nticks", 't', "Number of ticks to process (-1 = infinite)", -1 );
#ifdef DEBUG
    cmdline.addBoolean( "debug", 'd', "Show debug messages (on/off)", 0 );
#endif
    cmdline.addBoolean( "configHelp", 'H', "Show documentation of registered config types (on/off)", 0 );
    cmdline.addBoolean( "components", 'L', "Show component list", 0 );
    cmdline.addBoolean( "ccmdHelp", 'c', "Show custom commandline option help (those specified in config file)", 0 );

    cmdline.addBoolean( "noInterest", 0, "Disable TumInterestDetector component (3 levels of user interest)", 0 );
    cmdline.addBoolean( "noEmotion", 0, "Disable TumEmotionAnalyser component (dimensional affect)", 0 );
    cmdline.addBoolean( "noNonverbals", 0, "Disable TumNonverbalFeatureExtractor component (WARNING: this includes VAD, and probably will make the system unusable)", 0 );
    cmdline.addBoolean( "noWords", 0, "Disable TumKeywordSpotter component (keywords / ASR)", 0 );
    cmdline.addBoolean( "noAudioFeatures", 0, "Disable TumAudioFeatureExtractor component (low level prosodic features)", 0 );

    int help = 0;
    if (cmdline.doParse() == -1) {
      LOGGER.setLogLevel(0);
      help = 1;
    }
    LOGGER.setLogLevel(cmdline.getInt("loglevel"));

#ifdef DEBUG
    if (!cmdline.getBoolean("debug"))
      LOGGER.setLogLevel(LOG_DEBUG, 0);
#endif

    //SMILE_PRINT("config file is: %s",cmdline.getStr("configfile"));

    // create configManager:
    configManager = new cConfigManager(&cmdline);
    // create config type that maps semaine component names to corresponding openSMILE activeMqSink/Source components
    registerComponentForSmileConfig(configManager);
    // add the file config reader:
    configManager->addReader( new cFileConfigReader( cmdline.getStr("configfile") ) );


    cMan = new cComponentManager(configManager,componentlist);
    /*++ add our components (which are not in openSmileLib) here ++*/
    cMan->registerComponent(cActiveMqSource::registerComponent(configManager,cMan));
    cMan->registerComponent(cActiveMqSink::registerComponent(configManager,cMan));
    cMan->registerComponent(cSemaineEmmaSender::registerComponent(configManager,cMan));
    cMan->registerComponent(cSemaineWordSender::registerComponent(configManager,cMan));
    cMan->registerComponent(cSemaineNonverbalSender::registerComponent(configManager,cMan));
    cMan->registerComponent(cSemaineEmotionSender::registerComponent(configManager,cMan));
    cMan->registerComponent(cSemaineUserinfoSender::registerComponent(configManager,cMan));
    /*-- end add components --*/

    if (cmdline.getBoolean("configHelp")) {
      configManager->printTypeHelp(0);
      help = 1;
    }
    if (cmdline.getBoolean("components")) {
      cMan->printComponentList();
      help = 1;
    }

    if (help==1) { 
      delete configManager;
      delete cMan;
      return -1; 
    }

    // TODO: read config here and print ccmdHelp...
    configManager->readConfig();
    cmdline.doParse(1,0); // warn if unknown options are detected on the commandline
    if (cmdline.getBoolean("ccmdHelp")) {
      cmdline.showUsage();
      delete configManager;
      delete cMan;
      return -1;
    }

    cmanGlob = cMan;  // initialise global, so the Ctrl+C handler has access to it...
    signal(SIGINT, INThandler); // now set the signal handler for Ctrl+C (SIGINT)

	// TODO: move this to customStartIO()
//    cMan->createInstances(0); // 0 = do not read config (we already did that above..)

  } catch (cSMILException) { 
#ifdef _MSC_VER
    system("pause");
#endif
    return EXIT_ERROR; 
  }

  if (!ctrlc) {

    // start the actual component and its main loop
    try {
      activemq::library::ActiveMQCPP::initializeLibrary();
      semaine::util::XMLTool::startupXMLTools();

      std::list<semaine::components::Component *> comps;

      semaineAPIglobal = new semaine::components::smile::TumAudioFeatureExtractor(cMan,configManager);
      comps.push_back(semaineAPIglobal);

      if (!cmdline.getBoolean("noWords")) {
        semaine::components::smile::TumKeywordSpotter *keywordSpotter;
        keywordSpotter = new semaine::components::smile::TumKeywordSpotter(cMan,configManager);
        comps.push_back(keywordSpotter);
      }

      if (!cmdline.getBoolean("noInterest")) {
        semaine::components::smile::TumInterestDetector *interestDetector;
        interestDetector = new semaine::components::smile::TumInterestDetector(cMan,configManager);
        comps.push_back(interestDetector);
      }

      if (!cmdline.getBoolean("noEmotion")) {
        semaine::components::smile::TumEmotionAnalyser *emotionAnalyser;
        emotionAnalyser = new semaine::components::smile::TumEmotionAnalyser(cMan,configManager);
        comps.push_back(emotionAnalyser);
        // WARNING: interest and emotion share a common sender component...!!??
      }

      if (!cmdline.getBoolean("noNonverbals")) {
        semaine::components::smile::TumNonverbalFeatureExtractor *nonverbal;
        nonverbal = new semaine::components::smile::TumNonverbalFeatureExtractor(cMan,configManager);
        comps.push_back(nonverbal);
      }

      semaine::system::ComponentRunner cr(comps);
      cr.go();
      cr.waitUntilCompleted();

      semaine::util::XMLTool::shutdownXMLTools();
      activemq::library::ActiveMQCPP::shutdownLibrary();
      
    } catch (cms::CMSException & ce) {
      ce.printStackTrace();
    } catch (std::exception & e) {
      std::cerr << e.what();
    } catch(cSMILException) { 
#ifdef _MSC_VER
      system("pause");
#endif
      // it is important that configManager is deleted BEFORE componentManger! (since component Manger unregisters plugin Dlls)
      delete configManager;
      delete cMan;

      return EXIT_ERROR; 
    }


  }
  // it is important that configManager is deleted BEFORE componentManger! (since component Manger unregisters plugin Dlls)
  delete configManager;
  delete cMan;

  if (ctrlc) return EXIT_CTRLC;
  return EXIT_SUCCESS;

}

#else // not HAVE_SEMAINEAPI

#ifndef _MSC_VER // Visual Studio specific macro
#warning cannot build SEMAINExtract without being configured with libsemaineapi support!
#endif

int main (int argc, char *argv[]) {
  printf("\nSEMAINExtract was not compiled, because openSMILE was not configured with SEMAINE API support!\n\n");
  return 2;
}


#endif  //  HAVE_SEMAINEAPI

