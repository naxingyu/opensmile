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


#ifndef __COMPONENT_MANAGER_HPP
#define __COMPONENT_MANAGER_HPP

#include <core/smileCommon.hpp>
#include <core/smileComponent.hpp>

// this is the name of the configuration instance in the config file the component manager will search for:
#define CM_CONF_INST  "componentInstances"


// global component list: -----------------
#undef class
class DLLEXPORT cComponentManager;
typedef sComponentInfo * (*registerFunction)(cConfigManager *_confman, cComponentManager *_compman);
typedef void (*unRegisterFunction)();

extern DLLEXPORT const registerFunction componentlist[];

// default number of initial component variable space and plugin variable space to allocate:
#define COMPONENTMANAGER_DEFAULT_NCOMPS  200

// Max. iterations for register type, register instance, configure, and finalise process of components
#define MAX_REG_ITER  4
#define MAX_REGI_ITER 2
#define MAX_CONF_ITER 4  // <- should be nComponents!!
#define MAX_FIN_ITER  4


// openSMILE component runner thread status:
#define THREAD_ACTIVE   0
#define THREAD_WAIT_A   1
#define THREAD_WAIT_B   2
#define THREAD_END      3
#define THREAD_INACTIVE 4

typedef struct {
  cComponentManager *obj;
  long long maxtick;
  int threadId;
  int status;
} sThreadData;


class DLLEXPORT cComponentManager {
protected:
  int printPlugins;
//#ifdef DEBUG
  int execDebug;
//#endif
#ifdef __WINDOWS
  HINSTANCE *handlelist;
#else
  void **handlelist;
#endif

  registerFunction * regFnlist;
  int nPluginHandles, nPluginHandlesAlloc;

  int loadPlugin(const char * path, const char * fname);
  int registerPlugins();
  int checkPluginFilename(const char * str);

  // you may overwrite this method in a derived class to implement custom processing in every tick
  // return 1 on success and 0 on failure (a return value of 0 will terminate the processing loop)
  // threadID = -1 ==> single threaded mode   threadID >= 0  ==> multi-threaded mode with 1 or more threads
  virtual int userOnTick(long long tickNr, int EOI=0, int threadID=-1) { return 1; }

  int pausedNotifyComponents(int threadId, int isPause);

public:
  static void registerType(cConfigManager *_confman);

  cComponentManager(cConfigManager *_confman, const registerFunction _clist[] = componentlist);               // create component manager

  int compIsDm(const char *_compn);
  int ciRegisterComps(int _dm);
  int ciConfigureComps(int _dm);
  int ciFinaliseComps(int _dm, int *_n=NULL);
  int ciConfFinComps(int _dm, int *_n=NULL);

  void createInstances(int readConfig=1); // read config file and create instances as specified in config file

  // end-of-input handling:
  void setEOIcounterInComponents();
  void setEOI() {
    EOI++;
    EOIcondition = 1;
  }
  void unsetEOI() {
    EOIcondition = 0;
  }

  // check for EOIcondition
  int isEOI() { return EOIcondition; }
  // get the EOI counter
  int getEOIcounter() { return EOI; }

  long long tickLoopA(long long maxtick, int threadId, sThreadData *_data);
  void controlLoopA(void);

  long tick(int threadId, long long tickNr, long lastNrun=-1);  // call all components to process one time step ('tick')

  // this function calls tick() until 0 components run successfully, returns total number of ticks
  long long runSingleThreaded(long long maxtick=-1);

  // still a bit todo, basically same as runSingleThreaded with multiple threads, total number of ticks not yet fully supported!!
  long long runMultiThreaded(long long maxtick=-1);

  int addComponent(const char *_instname, const char *_type, const char *_ci=NULL, int _threadId=-1);  // create + register
  cSmileComponent * createComponent(const char *_name, const char *_type);
  cSmileComponent * createComponent(const char *_name, int n);

  // return number of registered component types
  int getNtypes() { return nCompTs; } 

  // filter=0: no filter, filter=1 : no abstract components, filter=2 non abstract and no non-Dmem (non-standalone) components (= sub-components)
  const char * getComponentType(int i, int filter=2, int *isAbstract=NULL, int *isNoDmem=NULL); // get name of component Type "i". abstract=1 = include abstract types (if abstract = 0 , then NULL will be returned if the type is abstract), or you may use the isAbstract flag, which is set by this function

  // get description of component "i"
  const char * getComponentDescr(int i); 

  int registerComponent(sComponentInfo *c, int noFree=0); // register a component type
  int findComponentType(const char *_type);
  int registerComponentTypes(const registerFunction _clist[]); // register component types by scanning given struct

  // filter=0: no filter, filter=1 : no abstract components, filter=2 non abstract and no non-Dmem (non-standalone) components (= sub-components)
  int printComponentList(int filter=2, int details=1);

  int registerComponentInstance(cSmileComponent * _component, const char *_typename, int _threadId=-1);  // register a component, return value: component id
  void unregisterComponentInstance(int id, int noDM=0);  // unregister and free component object
  int findComponentInstance(const char *_compname) const;
  cSmileComponent * getComponentInstance(int n);
  const char * getComponentInstanceType(int n);
  cSmileComponent * getComponentInstance(const char *_compname);
  const char * getComponentInstanceType(const char *_compname);

  // send inter component messages directly to component:
  int sendComponentMessage(const char *_compname, cComponentMessage *_msg);
  // send inter component messages directly to component:
  int sendComponentMessage(int compid, cComponentMessage *_msg);

  // get time in seconds (accurate up to the millisecond or microsecond (depending on the system)) since creation if componentManager
  double getSmileTime();

  void resetInstances(void);  // delete all component instances and reset componentManger to state before createInstances
  void waitForAllThreads(int threadID);
  void waitForController(int threadID, int stage);
  void decreaseNActive();

  /* pause tick loop:
how=0  :  resume tick loop
how=1  :  notify components, halt loop in a sleep
how=2  :  notify components,  wait for nRun = 0 (force NOT EOI!), then put loop to sleep
timeout :  if how=2, then max number of ticks of nRun>0 , after which pause request is discarded and EOI is assumed again when nRun=0  (irrelevant when how=1)
*/
  void pause(int how=1, int timeout=20);
  void resume() { pause(0,0); }

  // request program loop termination (works for single and multithreaded)
  void requestAbort() {
    resume();
    smileMutexLock(abortMtx);
    abortRequest = 1;
    if (nThreads != 1) { smileCondSignal(controlCond); }
    smileMutexUnlock(abortMtx);
  }
  int isAbort() { 
    smileMutexLock(abortMtx);
    int ret = abortRequest;
    smileMutexUnlock(abortMtx);
    return ret;
  }

  int isReady() {
    return ready;
  }

  ~cComponentManager();              // unregister and free all component objects

private:
  cConfigManager *confman;
  void fetchConfig(); // read config from confman

  int printLevelStats;
  int profiling;

  struct timeval startTime;
  int nCompTs, nCompTsAlloc;
  sComponentInfo *compTs; // component types

  int ready;    // flag that indicates if all components are set up and ready...
  int isConfigured;
  int isFinalised;
  int EOI;            // EOI iteration counter
  int EOIcondition;   // EOI flag

  //long long tickNr;
  int nComponents, nComponentsAlloc;
  int nActiveComponents;  // number of non-passive (i.e. non-dataMemory components)
  int nThreads;
  int threadPriority; /* default thread priority, or single thread priority */

  int lastComponent;
  cSmileComponent ** component;   // array of component instances
  char **componentInstTs;    // component instance corresponding type names
  int  * componentThreadId;

  long messageCounter;
  int oldSingleIterationTickLoop;

  long long pauseStartNr;
  int tickLoopPaused;
  int tickLoopPauseTimeout;
  smileCond pauseCond;
  smileMutex pauseMtx;

  smileMutex    messageMtx;
  smileMutex    waitEndMtx;
  smileCond     waitEndCond;
  smileCond     waitControllerCond;
  int waitEndCnt;

  // mutli thread sync variables:
  smileMutex    syncCondMtx;
  smileMutex    abortMtx;
  smileCond     syncCond;
  smileCond     controlCond;
  int nWaiting, nProbe, nActive;
  int runStatus, compRunFlag, probeFlag, controllerPresent; // communication flags between controller thread and component runner threads
  int *runFlag;
  int globalRunState;
  int abortRequest, endOfLoop;

  int getNextComponentId();

  
  // pause the current tick loop (by either sleeping or polling, specified via tickLoopPaused variable) and notify all associated components about pause/resume correctly
  // return value: nRun
  int pauseThisTickLoop(int threadID, int nRun, long long tickNr);

};


#endif  // __COMPONENT_MANAGER_HPP
