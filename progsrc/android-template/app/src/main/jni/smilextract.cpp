/*F***************************************************************************
 * 
 * openSMILE - the Munich open source Multimedia Interpretation by 
 * Large-scale Extraction toolkit
 * 
 * This file is part of openSMILE.
 * 
 * openSMILE is copyright (c) by audEERING GmbH. All rights reserved.
 * 
 * See file "COPYING" for details on usage rights and licensing terms.
 * By using, copying, editing, compiling, modifying, reading, etc. this
 * file, you agree to the licensing terms in the file COPYING.
 * If you do not agree to the licensing terms,
 * you must immediately destroy all copies of this file.
 * 
 * THIS SOFTWARE COMES "AS IS", WITH NO WARRANTIES. THIS MEANS NO EXPRESS,
 * IMPLIED OR STATUTORY WARRANTY, INCLUDING WITHOUT LIMITATION, WARRANTIES OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ANY WARRANTY AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE SOFTWARE OR ANY WARRANTY OF TITLE
 * OR NON-INFRINGEMENT. THERE IS NO WARRANTY THAT THIS SOFTWARE WILL FULFILL
 * ANY OF YOUR PARTICULAR PURPOSES OR NEEDS. ALSO, YOU MUST PASS THIS
 * DISCLAIMER ON WHENEVER YOU DISTRIBUTE THE SOFTWARE OR DERIVATIVE WORKS.
 * NEITHER TUM NOR ANY CONTRIBUTOR TO THE SOFTWARE WILL BE LIABLE FOR ANY
 * DAMAGES RELATED TO THE SOFTWARE OR THIS LICENSE AGREEMENT, INCLUDING
 * DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL OR INCIDENTAL DAMAGES, TO THE
 * MAXIMUM EXTENT THE LAW PERMITS, NO MATTER WHAT LEGAL THEORY IT IS BASED ON.
 * ALSO, YOU MUST PASS THIS LIMITATION OF LIABILITY ON WHENEVER YOU DISTRIBUTE
 * THE SOFTWARE OR DERIVATIVE WORKS.
 * 
 * Main authors: Florian Eyben, Felix Weninger, 
 * 	      Martin Woellmer, Bjoern Schuller
 * 
 * Copyright (c) 2008-2013, 
 *   Institute for Human-Machine Communication,
 *   Technische Universitaet Muenchen, Germany
 * 
 * Copyright (c) 2013-2015, 
 *   audEERING UG (haftungsbeschraenkt),
 *   Gilching, Germany
 * 
 * Copyright (c) 2016,	 
 *   audEERING GmbH,
 *   Gilching Germany
 ***************************************************************************E*/

// NOTE: it is important that this include comes before #define MODULE..
// why is unclear, but otherwise it does not compile ;-)
#include "smilextract.h"

#define MODULE "SMILExtract"
#include <android/log.h>
#include <jni.h>

cComponentManager *cmanGlob = NULL;
cComponentManager *getComponentManager() {
    return cmanGlob;
};

int SMILExtractFunction(int argc, const char *argv[], JavaVM *jvm, void * classLoader, void * findClassMethod)
{
    cmanGlob = NULL;
    __android_log_print(ANDROID_LOG_INFO, "opensmile", "Started openSMILE thread.");
    //same as SMILEXtract, only renamed the main() function
    try {
        smileCommon_fixLocaleEnUs();
        // set up the smile logger
        LOGGER.setLogLevel(1);
        LOGGER.enableConsoleOutput();

        // commandline parser:
        cCommandlineParser cmdline(argc, argv);
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
            __android_log_print(ANDROID_LOG_INFO, "opensmile", "\nNo commandline options were given.\n Please run ' SMILExtract -h ' to see some usage information!\n\n");
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


#ifdef DEBUG  
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

        configManager->addExternalPointer("JavaVM", jvm);
        configManager->addExternalPointer("ClassLoader", classLoader);
        configManager->addExternalPointer("FindClassMethod", findClassMethod);
        /* create all instances specified in the config file */
        cMan->createInstances(0); // 0 = do not read config (we already did that above..)
        cmanGlob = cMan;
        /* run single or mutli-threaded, depending on componentManager config in config file */
        long long nTicks = cMan->runMultiThreaded(cmdline.getInt("nticks"));

        /* it is important that configManager is deleted BEFORE componentManger!
          (since component Manger unregisters plugin Dlls, which might have allocated configTypes, etc.) */
        delete configManager;
        delete cMan;

    } catch (cSMILException *c) {
        // TODO: free cMan and configManager !?
        // free exception ??
        return EXIT_ERROR;
    }

    return EXIT_SUCCESS;
}
