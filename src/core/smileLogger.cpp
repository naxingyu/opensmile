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


/******************************************************************************/

 /*

 openSMILE Message Logging

 */


#include <time.h>

//#include <exceptions.hpp>
#include <core/smileLogger.hpp>

// include android native logging functionality
#ifdef __ANDROID__
#include <android/log.h>
#endif  //  __ANDROID__

// Global logger variable (used by exception classes for automatically logging...)
DLLEXPORT cSmileLogger SMILE_LOG_GLOBAL(1);

#define MODULE "smileLogger"

/********************* class implementation ***********************************/

cSmileLogger::cSmileLogger(int _loglevel, const char * _logfile, int _append) :
  logf(NULL),
  msg(NULL),
  silence(0),
  _enableLogPrint(1)
{
  if (_logfile != NULL) {
    logfile = strdup(_logfile);
    stde = 0;
    openLogfile(_append);
  }
  else { stde = 1; logfile = NULL; }

  if (_loglevel >= 0) {
    ll_msg = ll_wrn = ll_err = ll_dbg = _loglevel;
  } else {
    ll_msg = ll_wrn = ll_err = ll_dbg = 0;
  }
  smileMutexCreate(logmsgMtx);
}

cSmileLogger::cSmileLogger(int loglevel_msg, int loglevel_wrn, int loglevel_err, int loglevel_dbg, const char *_logfile, int _append):
  logf(NULL),
  msg(NULL),
  silence(0),
  _enableLogPrint(1)
{
  if (_logfile != NULL) {
    logfile = strdup(_logfile);
    stde = 0;
    openLogfile(_append);
  }
  else { stde = 1; logfile = NULL; }

  ll_msg = loglevel_msg;
  ll_wrn = loglevel_wrn;
  ll_err = loglevel_err;
  ll_dbg = loglevel_dbg;
  smileMutexCreate(logmsgMtx);
}

cSmileLogger::~cSmileLogger()
{
  smileMutexLock(logmsgMtx);
  if (msg != NULL) free(msg);
  closeLogfile();
  if (logfile != NULL) free(logfile);
  smileMutexUnlock(logmsgMtx);
  smileMutexDestroy(logmsgMtx);
}

/* opens the logfile */
void cSmileLogger::openLogfile(int append)
{
  if (logfile == NULL) return;
  if (logf) { fclose(logf); logf=NULL; }
  if (append) {
    logf = fopen(logfile,"a");
  } else {
    logf = fopen(logfile,"w");
  }
  if (logf == NULL) {
    throw(IOException(FMT("cannot open logfile for writing!"),0,MODULE));
  }
}

void cSmileLogger::closeLogfile()
{
  if (logf != NULL) { fclose(logf); logf = NULL; }
}

void cSmileLogger::setLogLevel(int _type, int level)
{
  switch(_type) {
    case LOG_ALL:
         ll_msg = ll_wrn = ll_err = ll_dbg = level;
         break;
    case LOG_MESSAGE: ll_msg = level; break;
    case LOG_WARNING: ll_wrn = level; break;
    case LOG_ERROR:   ll_err = level; break;
    case LOG_DEBUG:   ll_dbg = level; break;
    default:
         throw(ComponentException(FMT("invalid log level type (%i) specified in call to setLogLevel",level),MODULE));
  }
}

void cSmileLogger::setLogLevel(int level)
{
  setLogLevel(LOG_ALL,level);
}

void cSmileLogger::setLogFile(char *file, int _append, int _stde)
{
  if (file != NULL) {
    if (logfile) {
      free(logfile); logfile = NULL;
    }
    logfile = strdup(file);
    stde = _stde;
    openLogfile(_append);
  }
}

void cSmileLogger::setLogFile(const char *file, int _append, int _stde)
{
  if (file != NULL) {
    if (logfile) {
      free(logfile); logfile = NULL;
    }
    logfile = strdup(file);
    stde = _stde;
    openLogfile(_append);
  }
}

// formating of log message, save result in msg
void cSmileLogger::fmtLogMsg(const char *type, char *t, int level, const char *m)
{
  if (t == NULL) return;
  if (msg != NULL) {
    free(msg); msg = NULL;
  }
  if ((type != NULL)&&(m!=NULL)) {
    msg = myvprint("(%s) [%i] in %s : %s",type,level,m,t);
  } else if (type != NULL) {
    msg = myvprint("(%s) [%i] : %s",type,level,t);
  } else if (m != NULL) {
    msg = myvprint("(MSG) [%i] in %s : %s",level,m,t);
  } else {
    msg = myvprint("(MSG) [%i] : %s",level,t);
  }
}

// main log message dispatcher
void cSmileLogger::logMsg(int itype, char *s, int level, const char *m)
{
  if (!silence) {
    // check loglevel and type
    const char *type=NULL;
    switch (itype) {
       case LOG_PRINT :
            if (level > ll_msg) { free(s); return; }
            type=NULL;
            break;
       case LOG_MESSAGE :
            if (level > ll_msg) { free(s); return; }
            type=NULL;
            break;
       case LOG_ERROR :
            if (level > ll_err) { free(s); return; }
            type="ERROR";
            break;
       case LOG_WARNING :
            if (level > ll_wrn) { free(s); return; }
            type="WARN";
            break;
       case LOG_DEBUG :
            if (level > ll_dbg) { free(s); return; }
            type="DBG";
            break;
       default: return;
    }

    smileMutexLock(logmsgMtx);
    // format log message
    if (itype == LOG_PRINT) {
      if (msg != NULL) {
        free(msg); msg = NULL;
      }
      msg = myvprint("%s",s);
      if (_enableLogPrint) {
        // write to file
        writeMsgToFile(1);
      }

    } else {
      fmtLogMsg(type,s,level,m);
      // write to file
      writeMsgToFile();
    }
    free(s);
    

    // print to console
    if ((stde)||(logf == NULL)||(itype==LOG_PRINT))
      printMsgToConsole();

    smileMutexUnlock(logmsgMtx);
  }
}

// print message to console , without a timestamp
void cSmileLogger::printMsgToConsole()
{
  if (msg != NULL) {
    #ifdef __ANDROID__
      #ifndef __STATIC_LINK
         __android_log_print(ANDROID_LOG_INFO, "opensmile", "%s",msg);
      #else
        fprintf(stderr,"%s\n",msg);
        fflush(stderr);
      #endif
    #else
    fprintf(stderr,"%s\n",msg);
    fflush(stderr);
    #endif
    //#ifdef __MINGW32
    //fflush(stderr);
    //#endif
  }
}

// write a log message in *msg to current logfile (if open)
// add a date- and timestamp to message
void cSmileLogger::writeMsgToFile(int pr)
{
  if ((logf != NULL)&&(msg != NULL)) {
    if (pr==0) {
      // date string
      time_t t;
      time(&t);
      struct tm *ti;
      ti = localtime(&t);
      fprintf(logf,"[ %.2i.%.2i.%.4i - %.2i:%.2i:%.2i ]\n    ",
        ti->tm_mday, ti->tm_mon+1, ti->tm_year+1900,
        ti->tm_hour, ti->tm_min, ti->tm_sec
      );
    }
    // log message
    fprintf(logf,"%s\n",msg);
    fflush(logf);
  }
}

