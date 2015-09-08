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

This is the main commandline application

*/

#include <core/smileCommon.hpp>

#include <core/configManager.hpp>
#include <core/commandlineParser.hpp>
#include <core/componentManager.hpp>

#define MODULE "SMILExtract"


/************** Ctrl+C signal handler **/
#include  <signal.h>

cComponentManager *cmanGlob = NULL;

void INThandler(int);
int ctrlc = 0;

void INThandler(int sig)
{
  signal(sig, SIG_IGN);
  if (cmanGlob != NULL) cmanGlob->requestAbort();
  signal(SIGINT, INThandler);
  ctrlc = 1;
}
/*******************************************/


int main(int argc, char *argv[])
{
  try {

    smileCommon_fixLocaleEnUs();

    // set up the smile logger
    LOGGER.setLogLevel(1);
    LOGGER.enableConsoleOutput();


    // commandline parser:
    cCommandlineParser cmdline(argc,argv);
    cmdline.addStr( "configfile", 'C', "Path to openSMILE config file", "smile.conf" );
    cmdline.addInt( "loglevel", 'l', "Verbosity level (0-9)", 2 );
#ifdef DEBUG
    cmdline.addBoolean( "debug", 'd', "Show debug messages (on/off)", 0 );
#endif
    cmdline.addInt( "nticks", 't', "Number of ticks to process (-1 = infinite) (only works for single thread processing, i.e. nThreads=1)", -1 );
    //cmdline.addBoolean( "configHelp", 'H', "Show documentation of registered config types (on/off)", 0 );
    cmdline.addBoolean( "components", 'L', "Show component list", 0 );
    cmdline.addStr( "configHelp", 'H', "Show documentation of registered config types (on/off/argument) (if an argument is given, show only documentation for config types beginning with the name given in the argument)", NULL, 0 );
    cmdline.addStr( "configDflt", 0, "Show default config section templates for each config type (on/off/argument) (if an argument is given, show only documentation for config types beginning with the name given in the argument, OR for a list of components in conjunctions with the 'cfgFileTemplate' option enabled)", NULL, 0 );
    cmdline.addBoolean( "cfgFileTemplate", 0, "Print a complete template config file for a configuration containing the components specified in a comma separated string as argument to the 'configDflt' option", 0 );
    cmdline.addBoolean( "cfgFileDescriptions", 0, "Include description in config file templates.", 0 );
    cmdline.addBoolean( "ccmdHelp", 'c', "Show custom commandline option help (those specified in config file)", 0 );
    cmdline.addStr( "logfile", 0, "set log file", "smile.log" );
    cmdline.addBoolean( "nologfile", 0, "don't write to a log file (e.g. on a read-only filesystem)", 0 );
    cmdline.addBoolean( "noconsoleoutput", 0, "don't output any messages to the console (log file is not affected by this option)", 0 );
    cmdline.addBoolean( "appendLogfile", 0, "append log messages to an existing logfile instead of overwriting the logfile at every start", 0 );

    int help = 0;
    if (cmdline.doParse() == -1) {
      LOGGER.setLogLevel(0);
      help = 1;
    }
    if (argc <= 1) {
      printf("\nNo commandline options were given.\n Please run ' SMILExtract -h ' to see some usage information!\n\n");
      return 10;
    }

    if (help==1) { return 0; }

    if (cmdline.getBoolean("nologfile")) {
      LOGGER.setLogFile((const char *)NULL,0,!(cmdline.getBoolean("noconsoleoutput")));
    } else {
      LOGGER.setLogFile(cmdline.getStr("logfile"),cmdline.getBoolean("appendLogfile"),!(cmdline.getBoolean("noconsoleoutput")));
    }
    LOGGER.setLogLevel(cmdline.getInt("loglevel"));
    SMILE_MSG(2,"openSMILE starting!");

#ifdef DEBUG  // ??
    if (!cmdline.getBoolean("debug"))
      LOGGER.setLogLevel(LOG_DEBUG, 0);
#endif

    SMILE_MSG(2,"config file is: %s",cmdline.getStr("configfile"));


    // create configManager:
    cConfigManager *configManager = new cConfigManager(&cmdline);


    cComponentManager *cMan = new cComponentManager(configManager,componentlist);


    const char *selStr=NULL;
    if (cmdline.isSet("configHelp")) {
#ifndef EXTERNAL_BUILD
      selStr = cmdline.getStr("configHelp");
      configManager->printTypeHelp(1/*!!! -> 1*/,selStr,0);
#endif
      help = 1;
    }
    if (cmdline.isSet("configDflt")) {
#ifndef EXTERNAL_BUILD
      int fullMode=0; 
      int wDescr = 0;
      if (cmdline.getBoolean("cfgFileTemplate")) fullMode=1;
      if (cmdline.getBoolean("cfgFileDescriptions")) wDescr=1;
      selStr = cmdline.getStr("configDflt");
      configManager->printTypeDfltConfig(selStr,1,fullMode,wDescr);
#endif
      help = 1;
    }
    if (cmdline.getBoolean("components")) {
#ifndef EXTERNAL_BUILD
      cMan->printComponentList();
#endif  // EXTERNAL_BUILD
      help = 1;
    }

    if (help==1) {
      delete configManager;
      delete cMan;
      return -1; 
    }


    // TODO: read config here and print ccmdHelp...
    // add the file config reader:
    try{ 
      configManager->addReader( new cFileConfigReader( cmdline.getStr("configfile"), -1, &cmdline) );
      configManager->readConfig();
    } catch (cConfigException *cc) {
      return 0;
    }

    /* re-parse the command-line to include options created in the config file */
    cmdline.doParse(1,0); // warn if unknown options are detected on the commandline
    if (cmdline.getBoolean("ccmdHelp")) {
      cmdline.showUsage();
      delete configManager;
      delete cMan;
      return -1;
    }

    /* create all instances specified in the config file */
    cMan->createInstances(0); // 0 = do not read config (we already did that above..)

    /*
    MAIN TICK LOOP :
    */
    cmanGlob = cMan;
    signal(SIGINT, INThandler); // install Ctrl+C signal handler

    /* run single or mutli-threaded, depending on componentManager config in config file */
    long long nTicks = cMan->runMultiThreaded(cmdline.getInt("nticks"));

    /* it is important that configManager is deleted BEFORE componentManger! 
      (since component Manger unregisters plugin Dlls, which might have allocated configTypes, etc.) */
    delete configManager;
    delete cMan;

  } catch(cSMILException *c) { 
    // free exception ?? 
    return EXIT_ERROR; 
  } 

  if (ctrlc) return EXIT_CTRLC;
  return EXIT_SUCCESS;
}
