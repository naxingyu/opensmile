/*F***************************************************************************
 * openSMILE - the open-Source Multimedia Interpretation by Large-scale
 * feature Extraction toolkit
 * 
 * (c) 2008-2011, Florian Eyben, Martin Woellmer, Bjoern Schuller: TUM-MMK
 * 
 * (c) 2012-2013, Florian Eyben, Felix Weninger, Bjoern Schuller: TUM-MMK
 * 
 * (c) 2013-2014 audEERING UG, haftungsbeschränkt. All rights reserved.
 * 
 * Any form of commercial use and redistribution is prohibited, unless another
 * agreement between you and audEERING exists. See the file LICENSE.txt in the
 * top level source directory for details on your usage rights, copying, and
 * licensing conditions.
 * 
 * See the file CREDITS in the top level directory for information on authors
 * and contributors. 
 ***************************************************************************E*/


/******************************************************************************/

 /*

 openSMILE Message Logging


Log Levels for Messages:
0 . important messages, always displayed
1 . informative messages concerning functionality
2 . program status information (what we are currently doing, not very detailed though)
3 . more detailed status, report on success of operations
4 . component initialisation messages

Log Levels for Debug Messages:
0.  only very obvious information, status, etc.
1.
2.  warnings
3.  detailed information (not in internal loops)
4.
5.  detailed information from internal loops (e.g. every line from config file)
6.  extended details in internal loops


Log Levels for Errors:


Log Levels for Warnings:





 */


#ifndef __SMILE_LOGGER_HPP
#define __SMILE_LOGGER_HPP

#include <core/smileCommon.hpp>
#ifdef __ANDROID__
#include <android/log.h>
#endif

#define LOG_ALL     0
#define LOG_MESSAGE 1
#define LOG_WARNING 2
#define LOG_ERROR   3
#define LOG_DEBUG   4
#define LOG_PRINT   5

#undef class
class DLLEXPORT cSmileLogger {
  private:
    smileMutex logmsgMtx;
    char * logfile;
    FILE * logf;
    int stde;    // flag that indicates wheter stderr output is enabled or not
    int silence; // if set, logger will NOT produce ANY output!
    int _enableLogPrint;  // set, if you want log messages ('print') to be duplicated to the logfile
    int ll_msg;
    int ll_wrn;
    int ll_err;
    int ll_dbg;

    char *msg;  // current log message

    void openLogfile(int append=0);
    void closeLogfile();

    // formating of log message, save result in msg
    void fmtLogMsg(const char *type, char *t, int level, const char *m);

    // write a log message in *msg to current logfile (if open)
    // add a date- and timestamp to message
    void writeMsgToFile(int pr=0);

    // print message to console , without a timestamp
    void printMsgToConsole();

    // main log message dispatcher
    void logMsg(int itype, char *s, int level, const char *m);

  public:
    cSmileLogger(int _loglevel=0, const char *_logfile=NULL, int _append=0);  // logfile == NULL: output to stderr
                                                             // loglevel is the OVERALL loglevel
    cSmileLogger(int loglevel_msg, int loglevel_wrn, int loglevel_err, int loglevel_dbg, const char *logfile=NULL, int _append=0);
    ~cSmileLogger();

    void setLogLevel(int level);
    void setLogLevel(int type, int level);
    void setLogFile(char *file, int _append = 0, int _stde=0);
    void setLogFile(const char *file, int _append = 0, int _stde=0);

    int getLogLevel_msg() { return ll_msg; }
    int getLogLevel_wrn() { return ll_wrn; }
    int getLogLevel_err() { return ll_err; }
    int getLogLevel_dbg() { return ll_dbg; }
    
    void enableLogPrint() { _enableLogPrint = 1; } // enable printing of 'print' messages to log file (they are by default only written to the console)
    void enableConsoleOutput() { stde=1; }    // enable printing of log messages to console, even if a logfile is specified
    void muteLogger() { silence=1; }  // surpress all log messages
    void unmuteLogger() { silence=0; }  // back to normal logging


    /* similar to message, only that no formatting is performed and no extra information is printed
       the messages also do NOT go to the logfile by default (except, the enableLogPrint is set)
    */
		void log(int type, char *s, int level=0, const char *module=NULL)
		      { logMsg(type,s,level,module); }

		void print(char *s, int level=0)
		  { logMsg(LOG_PRINT,s,level,NULL); }

		void message(char *s, int level=0, const char *module=NULL)
		  { logMsg(LOG_MESSAGE,s,level,module); }

		void warning(char *s, int level=0, const char *module=NULL)
		  { logMsg(LOG_WARNING,s,level,module); }

		void error(char *s, int level=0, const char *module=NULL)
		  { logMsg(LOG_ERROR,s,level,module); }

    #ifdef DEBUG
    void debug(char *s, int level=0, const char *module=NULL)
      { logMsg(LOG_DEBUG,s,level,module); }
    #else
    void debug(char *s, int level=0, const char *module=NULL) { if (s!=NULL) free(s); }
    #endif
};

// Global logger variable (used by exception classes for automatically logging...)
#define LOGGER   smileLog
#define SMILE_LOG_GLOBAL smileLog
extern DLLEXPORT cSmileLogger SMILE_LOG_GLOBAL;


// The "I" logger functions (e.g. SMILE_IMSG) are meant to be used within cSmileComponent descendants. They will print the actual instance name instead of the component name
#ifdef SMILE_LOG_GLOBAL
#define SMILE_PRINT(...) SMILE_LOG_GLOBAL.print(FMT(__VA_ARGS__))

#define SMILE_PRINTL(level, ...) { if (level <= SMILE_LOG_GLOBAL.getLogLevel_msg()) SMILE_LOG_GLOBAL.print(FMT(__VA_ARGS__), level); }

#define SMILE_MSG(level, ...) { if (level <= SMILE_LOG_GLOBAL.getLogLevel_msg()) SMILE_LOG_GLOBAL.message(FMT(__VA_ARGS__), level, MODULE); }

#define SMILE_IMSG(level, ...) { if (level <= SMILE_LOG_GLOBAL.getLogLevel_msg()) { char *__mm = FMT("instance '%s'",getInstName()); SMILE_LOG_GLOBAL.message(FMT(__VA_ARGS__), level, __mm); free(__mm); } }

#define SMILE_ERR(level, ...) { if (level <= SMILE_LOG_GLOBAL.getLogLevel_err()) SMILE_LOG_GLOBAL.error(FMT(__VA_ARGS__), level, MODULE); }
#define SMILE_IERR(level, ...) { if (level <= SMILE_LOG_GLOBAL.getLogLevel_err()) { char *__mm = FMT("instance '%s'",getInstName()); SMILE_LOG_GLOBAL.error(FMT(__VA_ARGS__), level, __mm); free(__mm); } }

#define SMILE_WRN(level, ...) { if (level <= SMILE_LOG_GLOBAL.getLogLevel_wrn()) SMILE_LOG_GLOBAL.warning(FMT(__VA_ARGS__), level, MODULE); }
#define SMILE_IWRN(level, ...) { if (level <= SMILE_LOG_GLOBAL.getLogLevel_wrn()) { char *__mm = FMT("instance '%s'",getInstName()); SMILE_LOG_GLOBAL.warning(FMT(__VA_ARGS__), level, __mm); free(__mm); } }

/*
#define SMILE_PRINTL(level,rest...) SMILE_LOG_GLOBAL.print(FMT(rest), level)
#define SMILE_MSG(level, rest...) SMILE_LOG_GLOBAL.message(FMT(rest), level, MODULE)
#define SMILE_IMSG(level, rest...) SMILE_LOG_GLOBAL.message(FMT(rest), level, FMT("instance '%s'",getInstName()))
#define SMILE_ERR(level, rest...) SMILE_LOG_GLOBAL.error(FMT(rest), level, MODULE)
#define SMILE_IERR(level, rest...) SMILE_LOG_GLOBAL.error(FMT(rest), level, FMT("instance '%s'",getInstName()))
#define SMILE_WRN(level, rest...) SMILE_LOG_GLOBAL.warning(FMT(rest), level, MODULE)
#define SMILE_IWRN(level, rest...) SMILE_LOG_GLOBAL.warning(FMT(rest), level, FMT("instance '%s'",getInstName()))
*/

#ifdef DEBUG
#define SMILE_DBG(level, ...) { if (level <= SMILE_LOG_GLOBAL.getLogLevel_dbg()) SMILE_LOG_GLOBAL.debug(FMT(__VA_ARGS__), level, MODULE); }
#define SMILE_IDBG(level, ...) { if (level <= SMILE_LOG_GLOBAL.getLogLevel_dbg()) { char *__mm = FMT("instance '%s'",getInstName()); SMILE_LOG_GLOBAL.debug(FMT(__VA_ARGS__), level, __mm); free(__mm); } }

/*
#define SMILE_DBG(level, rest...) SMILE_LOG_GLOBAL.debug(FMT(rest), level, MODULE)
#define SMILE_IDBG(level, rest...) SMILE_LOG_GLOBAL.debug(FMT(rest), level, FMT("instance '%s'",getInstName()))
*/

#else
#define SMILE_DBG(level, ...)
#define SMILE_IDBG(level, ...)
#endif
#else
#define SMILE_PRINT(...)
#define SMILE_PRINTL(level, ...)
#define SMILE_MSG(level, ...)
#define SMILE_IMSG(level, ...)
#define SMILE_ERR(level, ...)
#define SMILE_IERR(level, ...)
#define SMILE_WRN(level, ...)
#define SMILE_IWRN(level, ...)
#define SMILE_DBG(level, ...)
#define SMILE_IDBG(level, ...)
#endif


#endif // __SMILE_LOGGER_HPP
