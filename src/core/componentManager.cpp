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


/*

central object to manage all components

componentManager registers a config type, which specifies the component list.
Example:

[componentInstances:cComponentManager]
instance[myWavReader].type = cWavReader
instance[myWavReader].configInstance = wavReader1Config
instance[myXYZcomp].type = cCompType
...



*********init procedure:

create componentManager
register all components (from componentlist, or later from dyn. libs)
load smile and componentManger config
create component instances (datamemory, readers, writers, and the rest)
configure all instances (once all instances are ready) [writers register dataMemory levels, etc.,]
finalise all instances (if all instances are configured, datamemory finalises here! (datamemory does sanity check on r/w level usage during finalise)

tick() loop

?? -> exit? external signal(ctrl+c, message, etc.) + component exception or state..? (e.g. EOF input file)
OR: no progress for X ticks..?
=> implement mechanism for signalling end-of-processing (EOP) to component manager (e.g. new exception type?)
PROBLEM: if end-of-input is reached, then there might be more processing required till we are completely finished...
(only components without a data reader can signal EOP?)
THUS: no progress for X ticks..AFTER at least one component has signalled EOP?

???? support reconf via componentManager ????
*/

#include <core/componentManager.hpp>


//TODO:
/*
handle component creation, config, and finalisation better, give more suitable (success/fail) debug messages
halt on config and/or finalise errors
allow manual configuration of components by calling create, config, and finalise seperately
(however, also allow for single call setup (default!))
*/


// global component list: -----------------
/* list all built-in components here... 
Also be sure to include the necessary component header files
*/
#include <core/componentList.hpp>

#define MODULE "cComponentManager"

//-----------------------------------------



/************************/

void cComponentManager::registerType(cConfigManager *_confman) {
  //confman = _confman;
  if (_confman != NULL) {
    ConfigType *comp = new ConfigType("cComponentManagerInst");
    if (comp == NULL) OUT_OF_MEMORY;
    comp->setField( "type", "name of component type to create an instance of", (char*)NULL);
    comp->setField( "configInstance", "config instance to connect to component instance (UNTESTED?)", (char*)NULL);
    comp->setField( "threadId", "thread nr. to run this component in (default = -1: either run in 1st thread or automatically run each component in one thread if nThread==0)", -1);

    ConfigType *complist = new ConfigType( "cComponentManager" );
    if (complist == NULL) OUT_OF_MEMORY;
    complist->setField( "instance", "Associative array storing component list.\n   Array indicies are the instance names.",
      comp, 1 );
    complist->setField( "printLevelStats", "1 = print detailed information about data memory level configuration, 2 = print even more details (?)",1);
    complist->setField( "profiling", "1 = collect per component instance run-time stats and show summary at end of processing.", 0);
    complist->setField( "nThreads", "number of threads to run (0=auto(=one thread per component), >0 = actual number of threads",1);
    complist->setField( "threadPriority", "The default thread scheduling priority (multi-thread mode) or the priority of the single thread (single thread mode). 0 is normal priority (-15 is background/idle priority, +15 is time critical). This option is currently only supported on windows!",0);
    //#ifdef DEBUG
    complist->setField( "execDebug", "print summary of component run statistics to log for each tick", 0);
    complist->setField( "oldSingleIterationTickLoop", "1 = run the old single iteration tick loop with a single EOI tick loop after the main tick loop. Use this for backwards compatibility for older configs with components such as fullinputMean.", 0);
    //#endif
    ConfigInstance *Tdflt = new ConfigInstance( "cComponentManagerInst", complist, 1 );
    _confman->registerType(Tdflt);
    //confman->registerType( complist );
  } else {
    SMILE_ERR(1,"cannot register component manager config type! _confman is NULL in registerType()!");
  }
}

/********************/

// sComponentInfo *comps; // component types

int cComponentManager::registerComponent(sComponentInfo *c, int noFree) // register a component type and free the sComponentInfo struct memory (if noFree=0)
{
  if (c!=NULL) {
    // find existing:
    int t = findComponentType(c->componentName);
    if (t == -1) {
      if (nCompTs >= nCompTsAlloc) { // realloc
        sComponentInfo *_compTs = (sComponentInfo *)crealloc(compTs, (nCompTs+COMPONENTMANAGER_DEFAULT_NCOMPS)*sizeof(sComponentInfo), nCompTsAlloc*sizeof(sComponentInfo));
        if (_compTs == NULL) OUT_OF_MEMORY;
        compTs = _compTs;
        nCompTsAlloc = (nCompTs+COMPONENTMANAGER_DEFAULT_NCOMPS);
      }
      t = nCompTs++;
      memcpy(compTs+t, c, sizeof(sComponentInfo));
      compTs[t].next = NULL;
      SMILE_DBG(4,"registered component type '%s' with id %i", compTs[t].componentName, t);
    } else {
      memcpy(compTs+t, c, sizeof(sComponentInfo));
      compTs[t].next = NULL;
      SMILE_DBG(4,"re-registered component type '%s' with id %i", compTs[t].componentName, t);
    }
    if (!noFree) delete c; // <<-- NOTE: TODO! we are not allowed to call this delete when the component info struct has been allocated in a plugin!
    return t;
  }
  return -1;
}

int cComponentManager::findComponentType(const char *_type)
{
  int i;
  if (compTs == NULL) return -1;
  for (i=0; i<nCompTs; i++) {
    if (!strcmp(_type, compTs[i].componentName)) {
      SMILE_DBG(5,"findComponentType: found componentType '%s' at index %i", _type, i);
      return i;
    }
  }
  return -1;
}

const char * cComponentManager::getComponentType(int i, int filter, int *isAbstract, int *isNoDmem) // get name of component Type "i". abstract=1 = include abstract types (if abstract = 0 , then NULL will be returned if the type is abstract), or you may use the isAbstract flag, which is set by this function
{
  if ( (i>=0)&&(i<nCompTs) ) {
    if (isAbstract != NULL) *isAbstract = compTs[i].abstract;
    if (isNoDmem != NULL) *isNoDmem = compTs[i].noDmem;
    if (filter == 0) { // filter = 0: return all components
      return compTs[i].componentName;
    } else {
      if (filter == 1) {  // filter=1 : return non-abstract components
        if (compTs[i].abstract) return NULL;
        else return compTs[i].componentName;
      } else if (filter==2) { // filter = 2: return only dmem interface componenets (non-abstract and non-noDmem)
        if ((compTs[i].abstract)||(compTs[i].noDmem)) return NULL;
        else return compTs[i].componentName;
      }
    }
  } else {
    return NULL;
  }
  return NULL;
}

const char * cComponentManager::getComponentDescr(int i) // get name of component Type "i". abstract=1 = include abstract types (if abstract = 0 , then NULL will be returned if the type is abstract), or you may use the isAbstract flag, which is set by this function
{
  if ( (i>=0)&&(i<nCompTs) ) {
    return compTs[i].description;
  } else { return NULL; }
}

#ifdef SMILE_SUPPORT_PLUGINS

int cComponentManager::checkPluginFilename(const char * str)
{
  const char * dot = strrchr(str,'.');
#ifdef __WINDOWS
  const char * ending = "dll";
  int n = 3;
#else
  const char * ending = "so";
  int n = 2;
#endif
  if (dot != NULL) {
    dot++;
    if (!strncasecmp(dot,ending,n)) return 1;
  }
  //TODO: find .so/.dll files even if a version number e.g. .so.0.0.1 is appended!

  return 0;
}


#ifndef __WINDOWS

//#include <windows.h>
//#include <tchar.h>
//#include <string.h>
//#include <strsafe.h>

//#else

// unix/gnu
#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>

#endif


int cComponentManager::loadPlugin(const char * path, const char * fname)
{
#ifdef __STATIC_LINK
  SMILE_ERR(1,"This binary is statically linked, dynmaic loading of plugins is not supported!");
  return 0;
#else
  //sComponentInfo * registerComponent(cConfigManager *_confman, cComponentManager *_compman)
  //sComponentInfo * (*regFn)(cConfigManager *_confman, cComponentManager *_compman);
  registerFunction regFn;

  char * fullname;
  if (path != NULL) {
#ifdef __WINDOWS
    fullname = myvprint("%s\\%s",path,fname);
#else
    fullname = myvprint("%s/%s",path,fname);
#endif
  } else {
    fullname = myvprint("%s",fname);
  }

#ifdef __WINDOWS
  HINSTANCE lib_handle;
  lib_handle = LoadLibrary(TEXT(fullname));

  // If the handle is valid, try to get the function address.
  if (lib_handle != NULL) {
    regFn = (registerFunction) GetProcAddress(lib_handle, "registerPluginComponent");
    // If the function address is valid, call the function.
    if (regFn == NULL) {
      SMILE_ERR(1, "error registering plugin '%s' (symbol 'registerPluginComponent' not found)",fullname);
      FreeLibrary(lib_handle);
      return 0;
    }
  } else {
    SMILE_ERR(1,"cannot open library '%s' : LoadLibrary() failed!",fullname);
    return 0;
  }

#else
  const char *error;
  void *lib_handle;

  lib_handle = dlopen(fullname, RTLD_LAZY);
  if (!lib_handle) {
    SMILE_ERR(1, "cannot open plugin '%s' : '%s'", fullname, dlerror());
    return 0;
  }

  regFn = (registerFunction)dlsym(lib_handle, "registerPluginComponent");
  if ( ((error = dlerror()) != NULL) || (regFn == NULL)) {
    SMILE_ERR(1, "error registering plugin '%s' (symbol 'registerPluginComponent' not found): '%s'",fullname, error);
    dlclose(lib_handle);
    return 0;
  }
#endif

  SMILE_MSG(4, "found registerPluginComponent() in '%s'",fullname);

  free(fullname);
  if (nPluginHandles >= nPluginHandlesAlloc) {
#ifdef __WINDOWS
    handlelist = (HINSTANCE*)crealloc(handlelist, (nPluginHandlesAlloc+COMPONENTMANAGER_DEFAULT_NCOMPS)*sizeof(HINSTANCE), (nPluginHandlesAlloc)*sizeof(HINSTANCE) );
#else
    handlelist = (void**)crealloc(handlelist, (nPluginHandlesAlloc+COMPONENTMANAGER_DEFAULT_NCOMPS)*sizeof(void*), (nPluginHandlesAlloc)*sizeof(void*) );
#endif
    regFnlist =  (registerFunction *)crealloc((void *)regFnlist, (nPluginHandlesAlloc+COMPONENTMANAGER_DEFAULT_NCOMPS)*sizeof(registerFunction), (nPluginHandlesAlloc)*sizeof(registerFunction) );
    if (handlelist == NULL) OUT_OF_MEMORY;
    if (regFnlist == NULL) OUT_OF_MEMORY;
    nPluginHandlesAlloc += COMPONENTMANAGER_DEFAULT_NCOMPS;
  }
  handlelist[nPluginHandles] = lib_handle;
  regFnlist[nPluginHandles++] = regFn;

  return 1;
#endif // __STATIC_LINK
}


int cComponentManager::registerPlugins()
{
  int nPlugins = 0;
  nPluginHandles = 0;
  nPluginHandlesAlloc = 0;

  // scan plugins in plugin-path:
#ifdef __WINDOWS
  // TODO: config file option "pluginpath" and "pluginlist"
  const char * pluginpath = ".\\plugins";

  // WIN: use FindFirstFile, FindNextFile, FindClose
  WIN32_FIND_DATA ffd;
  HANDLE hFind = INVALID_HANDLE_VALUE;
  char szDir[MAX_PATH];
  DWORD dwError=0;

  if (strlen(pluginpath) > (MAX_PATH - 2)) {
    SMILE_ERR(1,"Plugin-directory path '%s' is too long for windows systems....",pluginpath);
    return 0;
  }

  // Prepare string for use with FindFile functions.  First, copy the
  // string to a buffer, then append '\*' to the directory name.

#ifdef __MINGW32
  strncpy(szDir, pluginpath, MAX_PATH);
#else
  strcpy_s(szDir, MAX_PATH, pluginpath);
#endif
  strncat(szDir, "\\*", MAX_PATH);

  hFind = FindFirstFile(szDir, &ffd);
  if (INVALID_HANDLE_VALUE == hFind) {
    SMILE_WRN(3,"Cannot find files in plugin directory '%s'! You can ignore this, if you are not using openSMILE plugins.",pluginpath);
    return 0;
  }

  // List all the files in the directory with some info about them.
  do {
    if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
      if (checkPluginFilename(ffd.cFileName)) {
        SMILE_MSG(2,"found plugin : '%s'",ffd.cFileName);
        // load the plugin:
        //#ifdef __WINDOWS
        if (loadPlugin(pluginpath, ffd.cFileName)) nPlugins++;
        //#else
        //if (loadPlugin(NULL, ffd.cFileName)) nPlugins++;
        //#endif
      }
    }
  }
  while (FindNextFile(hFind, &ffd) != 0);

  dwError = GetLastError();
  if (dwError != ERROR_NO_MORE_FILES) {
    SMILE_ERR(1,"error reading contents of plugin-directory '%s' (nPlugins=%i plugins loaded)! ",pluginpath,nPlugins);
    FindClose(hFind);
    return nPlugins;
  }
  FindClose(hFind);

#else  // UNIX:
  // TODO: config file option "pluginpath" and "pluginlist"
  const char * pluginpath = "./plugins";

  DIR *dp;
  struct dirent *dirp;
  if((dp  = opendir(pluginpath)) == NULL) {
    SMILE_WRN(3,"cannot open plugin directory '%s' for reading! You can ignore this if you are not using openSMILE plugins.",pluginpath);
    return 0;
  }

  while ((dirp = readdir(dp)) != NULL) {
    if (checkPluginFilename(dirp->d_name)) { // TODO: skip directories!
      SMILE_MSG(2,"found plugin : '%s'",dirp->d_name);
      // load the plugin:
      if (loadPlugin(pluginpath, dirp->d_name)) nPlugins++;
    }
  }
  closedir(dp);
#endif

  if (nPlugins == 0) return 0;

  int i;
  int nAgain=0, nReg=0;
  int regIter = MAX_REG_ITER;

  int *regDone = (int *) calloc(1,sizeof(int)*nPlugins);

  if (printPlugins) {
    SMILE_PRINT (" == List of detected plugins: ==");
  }

  do {

    for (i=0; i<nPlugins; i++) {
      if (!(regDone[i])) {
        sComponentInfo *c = (regFnlist[i])(confman,this);

        //        registerFunction *tmp = (registerFunction *) c; //--

        do {
          //          c = (*(tmp++))(confman,this); //--

          if ((c!=NULL)&&(printPlugins)) {
            SMILE_PRINT(" plugin '%s' : '%s'",c->componentName,c->description);
          }

          sComponentInfo *nextc;
          if (c!=NULL) nextc = c->next;

          int t = registerComponent( c, 1 );

          if ( (t>=0) && (t < nCompTsAlloc)) {
            if (compTs[t].registerAgain) { nAgain++; }
            else { nReg++; regDone[i] = 1; }
          }
          c = nextc;
        } while (c!=NULL);
        //} while (*tmp!=NULL);
      }
    }

#ifdef DEBUG
    //printf("reg round %i - nAgain %i\n",regIter,nAgain);
    if (nAgain > 0)
      SMILE_DBG(3,"%i of %i plugin-components requested re-register in round %i",nAgain,nAgain+nReg,MAX_REG_ITER-regIter+1);
#endif

  } while ((nAgain > 0 )&&(regIter-- > 0));
  if (printPlugins) {
    SMILE_PRINT (" ===============================");
  }

  free(regDone);

  return nReg;
}
#endif

// register component types by scanning given struct
int cComponentManager::registerComponentTypes(const registerFunction _clist[]) 
{
  int i=0;
  int nAgain, nReg=0;
  int regIter = MAX_REG_ITER;

  int nc=0;
  while (_clist[i] != NULL) { nc++; i++; }
  i=0;
  int *regDone = (int *) calloc(1,sizeof(int)*nc);

  if (_clist != NULL) {
    do {
      nAgain = 0; i=0;
      //TODO: remember already registered componentes right here.... do not call registerComponent twice!
      while (_clist[i] != NULL) {
        if (!(regDone[i])) {
          int t = registerComponent( (_clist[i])(confman,this) );
          if ( (t>=0) && (t < nCompTsAlloc)) {
            if (compTs[t].registerAgain) { nAgain++; }
            else { nReg++; regDone[i] = 1; }
          }
        }
        i++;
      }
#ifdef DEBUG
      //printf("reg round %i - nAgain %i\n",regIter,nAgain);
      if (nAgain > 0)
        SMILE_DBG(3,"%i of %i components requested re-register in round %i",nAgain,nAgain+nReg,MAX_REG_ITER-regIter+1);
#endif
    } while ((nAgain > 0 )&&(regIter-- > 0));
    if ((regIter == 0)&&(nAgain > 0)) {
      free(regDone);
      COMP_ERR("%i of %i components could not register successfully during %i registration iterations",nAgain,nAgain+nReg,MAX_REG_ITER);
    } else
      SMILE_MSG(2,"successfully registered %i component types.",nReg);
  }
  free(regDone);

#ifdef SMILE_SUPPORT_PLUGINS
  // now register plug-in components...
  nReg += registerPlugins();
#endif

  return nReg;
}

int cComponentManager::printComponentList(int filter, int details)
{
  int nTp = getNtypes();
  int i,j=0;
  //  if (details) {
  SMILE_PRINT("==> The following %i components are currently registered in openSMILE:\n",nTp);
  //  }
  for (i=0; i<nTp; i++) {
    const char * tp = getComponentType(i,filter);
    if (tp!=NULL) {
      if (details) {
        SMILE_PRINT(" +++ '%s' +++",tp);
        SMILE_PRINT("   %s\n",getComponentDescr(i));
        // TODO: print if component is built in OR dll registered...
      } else {
        SMILE_PRINT("  '%s'",tp);
      }
    }
  }
  return nTp;
}

extern void resetFunctionalsRaCounter();
cComponentManager::cComponentManager(cConfigManager *_confman, const registerFunction _clist[]) :
nComponents(0),
lastComponent(0),
nComponentsAlloc(0),
ready(0),
confman(_confman),
nCompTs(0),
handlelist(NULL),
regFnlist(NULL),
nCompTsAlloc(0),
nWaiting(0),
nActive(0),
printPlugins(1),
//  threadNRun(NULL),
globalRunState(0),
compTs(NULL),
abortRequest(0),
componentThreadId(NULL),
EOI(0),
EOIcondition(0),
tickLoopPaused(0),
tickLoopPauseTimeout(10),
pauseStartNr(-1),
isConfigured(0), isFinalised(0), printLevelStats(0),
messageCounter(0), profiling(0),
oldSingleIterationTickLoop(0)
{
  resetFunctionalsRaCounter();
  if (confman == NULL) COMP_ERR("cannot create component manager with _confman == NULL!");
  cComponentManager::confman = _confman;

  smileMutexCreate(messageMtx);
  smileMutexCreate(abortMtx);
  smileMutexCreate(pauseMtx);
  smileCondCreate(pauseCond);

  // register components (component types)
  registerComponentTypes(_clist);

  // register our config type:
  cComponentManager::registerType(confman);

  component = (cSmileComponent **) calloc(1,sizeof(cSmileComponent *) * COMPONENTMANAGER_DEFAULT_NCOMPS);
  componentInstTs = (char **) calloc(1,sizeof(char *) * COMPONENTMANAGER_DEFAULT_NCOMPS);
  componentThreadId = (int*) calloc(1,sizeof(int *) * COMPONENTMANAGER_DEFAULT_NCOMPS);
  if (component == NULL) OUT_OF_MEMORY;
  if (componentInstTs == NULL) OUT_OF_MEMORY;
  if (componentThreadId == NULL) OUT_OF_MEMORY;
  nComponentsAlloc = COMPONENTMANAGER_DEFAULT_NCOMPS;

  // get system start time
  gettimeofday(&startTime,NULL);
}

int cComponentManager::compIsDm(const char *_compn)
{
  return (!strcmp(_compn,COMPONENT_NAME_CDATAMEMORY)); 
}

int cComponentManager::ciRegisterComps(int _dm)
{
#ifdef DEBUG
  if (_dm == 0) {
    SMILE_DBG(2,"==> createInstances: REGISTER PHASE (components) <==");
  } else {
    SMILE_DBG(2,"==> createInstances: REGISTER PHASE (dataMemories) <==");
  }
  SMILE_DBG(2,"    %i instances in total (max. %i iterations)",  nComponents, MAX_REGI_ITER );
#endif

  int it,i;
  int nDone = 0; 
  int nDef = 0;
  int nIgn = 0;

  for (it=0; it<MAX_REGI_ITER; it++) {
    nDone = 0; 
    nDef = 0;
    nIgn = 0;

    // register all components 
    for (i=0; i<lastComponent; i++) {
      if (component[i]!=NULL) {
        if (compIsDm(component[i]->getTypeName()) == _dm) {

          if (component[i]->registerInstance()) { // TODO: save runMe config in componentManager..
            nDone++;
            SMILE_DBG(3,"registered component instance '%s' (type '%s'), index %i",component[i]->getInstName(),component[i]->getTypeName(),i);
          } else {
            nDef++;
            SMILE_DBG(3,"component instance '%s' (type '%s'), index %i, could not be registered",component[i]->getInstName(),component[i]->getTypeName(),i);
          }

        } else {
          nIgn++;
        }
      }
    }

    if (nDef == 0) {
      if (_dm == 0) 
      { SMILE_MSG(3,"successfully registered %i of %i component instances (non dataMemory type)", nDone, nComponents-nIgn ); }
      else
      { SMILE_MSG(3,"successfully registered %i of %i dataMemory instances", nDone, nComponents-nIgn ); }
      break; // else: continue configuration process for MAX_REGI_ITER
    } else {
      SMILE_DBG(2,"need to re-register %i components after iteration %i",nDef,it);
    }
  }

  return (nDef);
}

int cComponentManager::ciConfigureComps(int _dm)
{
#ifdef DEBUG
  if (_dm == 0) {
    SMILE_DBG(2,"==> createInstances: CONFIGURE PHASE (components) <==");
  } else {
    SMILE_DBG(2,"==> createInstances: CONFIGURE PHASE (dataMemories) <==");
  }
  SMILE_DBG(2,"    %i instances in total (max. %i iterations)",  nComponents, MAX_CONF_ITER );
#endif

  int it,i;
  int nDone = 0; 
  int nDef = 0;
  int nIgn = 0;

  for (it=0; it<MAX_CONF_ITER; it++) {
    nDone = 0; 
    nDef = 0;
    nIgn = 0;

    // register all components 
    for (i=0; i<lastComponent; i++) {
      if (component[i]!=NULL) {
        if (compIsDm(component[i]->getTypeName()) == _dm) {

          if (component[i]->configureInstance()) {
            nDone++;
            SMILE_DBG(3,"configured component instance '%s' (type '%s'), index %i",component[i]->getInstName(),component[i]->getTypeName(),i);
          } else {
            nDef++;
            SMILE_DBG(3,"component instance '%s' (type '%s'), index %i, could not be configured (yet)",component[i]->getInstName(),component[i]->getTypeName(),i);
          }

        } else {
          nIgn++;
        }
      }
    }

    if (nDef == 0) {
      if (_dm == 0) {
        SMILE_MSG(3,"successfully configured %i of %i component instances (non dataMemory type)", nDone, nComponents-nIgn );
      } else {
        SMILE_MSG(3,"successfully configured %i of %i dataMemory instances", nDone, nComponents-nIgn );
      }
      break; // else: continue configuration process for MAX_CONF_ITER
    } else {
      SMILE_DBG(2,"need to re-configure %i components after iteration %i",nDef,it);
    }
  }

  return (nDef);
}

int cComponentManager::ciFinaliseComps(int _dm, int *_n)
{
#ifdef DEBUG
  if (_dm == 0) {
    SMILE_DBG(2,"==> createInstances: FINALISE PHASE (components) <==");
  } else {
    SMILE_DBG(2,"==> createInstances: FINALISE PHASE (dataMemories) <==");
  }
  SMILE_DBG(2,"    %i instances in total (max. %i iterations)",  nComponents, MAX_FIN_ITER );
#endif

  int it,i;
  int nDone = 0; 
  int nDef = 0;
  int nIgn = 0;

  //for (it=0; it<MAX_FIN_ITER; it++) {
  for (it=0; it<(nComponents-1); it++) { // was: it < MAX_FIN_ITER , however we need to use nComponents - 1  !!
    nDone = 0; 
    nDef = 0;
    nIgn = 0;

    // finalise components 
    for (i=0; i<lastComponent; i++) {
      if (component[i]!=NULL) {
        if (compIsDm(component[i]->getTypeName()) == _dm) {

          if (component[i]->finaliseInstance()) {
            nDone++;
            SMILE_MSG(3,"finalised component instance '%s' (type '%s'), index %i",component[i]->getInstName(),component[i]->getTypeName(),i);
          } else {
            nDef++;
            SMILE_DBG(3,"component instance '%s' (type '%s'), index %i, could not be finalised",component[i]->getInstName(),component[i]->getTypeName(),i);
          }

          if ( (_dm)&&(printLevelStats) ) {
            ((cDataMemory *)component[i])->printDmLevelStats(printLevelStats);
          }
        } else {
          nIgn++;
        }
      }
    }

    if (nDef == 0) {
      if (_dm == 0) {
        SMILE_MSG(3,"successfully finalised %i of %i component instances (non dataMemory type)", nDone, nComponents-nIgn );
      } else {
        SMILE_MSG(3,"successfully finalised %i of %i dataMemory instances", nDone, nComponents-nIgn );
      }
      break; // else: continue configuration process for MAX_FIN_ITER
    } else {
      SMILE_DBG(2,"need to re-finalise %i components after iteration %i",nDef,it);
    }
  }

  if (_n != NULL) *_n = nDone;
  return (nDef);
}

// configure and finalise in turns..
int cComponentManager::ciConfFinComps(int _dm, int *_n)
{
#ifdef DEBUG
  if (_dm == 0) {
    SMILE_DBG(2,"==> createInstances: CONFIGURE PHASE (components) <==");
  } else {
    SMILE_DBG(2,"==> createInstances: CONFIGURE PHASE (dataMemories) <==");
  }
  SMILE_DBG(2,"    %i instances in total (max. %i iterations)",  nComponents, MAX_CONF_ITER );
#endif

  int it,i;
  int nDone = 0; 
  int nDef = 0;
  int nIgn = 0;

  for (it=0; it<(nComponents-1); it++) { // was: it < MAX_CONF_ITER , however we need to use nComponents - 1  !!
    nDone = 0; 
    nDef = 0;
    nIgn = 0;

    // register all components 
    for (i=0; i<lastComponent; i++) {
      if (component[i]!=NULL) {
        if (compIsDm(component[i]->getTypeName()) == _dm) {

          if (component[i]->configureInstance()) {
            //nDone++;
            SMILE_DBG(3,"configured component instance '%s' (type '%s'), index %i",component[i]->getInstName(),component[i]->getTypeName(),i);
            if (component[i]->finaliseInstance()) {
              nDone++;
              SMILE_DBG(3,"configured component instance '%s' (type '%s'), index %i",component[i]->getInstName(),component[i]->getTypeName(),i);
            } else {
              nDef++;
              SMILE_DBG(3,"component instance '%s' (type '%s'), index %i, could not be configured (yet)",component[i]->getInstName(),component[i]->getTypeName(),i);
            }

          } else {
            nDef++;
            SMILE_DBG(3,"component instance '%s' (type '%s'), index %i, could not be configured (yet)",component[i]->getInstName(),component[i]->getTypeName(),i);
          }

        } else {
          nIgn++;
        }
      }
    }

    if (nDef == 0) {
      if (_dm == 0) {
        SMILE_MSG(3,"successfully configured %i of %i component instances (non dataMemory type)", nDone, nComponents-nIgn );
      } else {
        SMILE_MSG(3,"successfully configured %i of %i dataMemory instances", nDone, nComponents-nIgn );
      }
      break; // else: continue configuration process for MAX_CONF_ITER
    } else {
      SMILE_DBG(2,"need to re-configure %i components after iteration %i",nDef,it);
    }
  }

  if (_n != NULL) *_n = nDone;
  return (nDef);
}

void cComponentManager::createInstances(int readConfig)
{
  /*
  create component instances (datamemory, readers, writers, and the rest)
  configure all instances (once all instances are ready) [writers register dataMemory levels, etc.,]
  finalise all instances (if all instances are configured, datamemory finalises here! (datamemory does sanity check on r/w level usage during finalise)
  */
  // load smile and componentManger config
  if (readConfig) confman->readConfig();

  char *tmp = myvprint("%s.printLevelStats",CM_CONF_INST);
  printLevelStats = confman->getInt(tmp);
  free(tmp);
  tmp = myvprint("%s.profiling",CM_CONF_INST);
  profiling = confman->getInt(tmp);
  free(tmp);
  //#ifdef DEBUG
  tmp = myvprint("%s.execDebug",CM_CONF_INST);
  execDebug = confman->getInt(tmp);
  free(tmp);
  tmp = myvprint("%s.oldSingleIterationTickLoop", CM_CONF_INST);
  oldSingleIterationTickLoop = confman->getInt(tmp);
  free(tmp);
  //#endif


  // create component instances (datamemory, readers, writers, and the rest)
  //     const char **getArrayKeys(const char *_name, int *N=NULL) const;
  int N_=0;
  tmp = myvprint("%s.instance",CM_CONF_INST);
  char **insts = confman->getArrayKeys(tmp,&N_);
  if (tmp!=NULL) free(tmp);
  if ((insts!=NULL)&&(N_>0)) {
    SMILE_DBG(2,"found %i component instances in config to create.",N_);
    int i;

    // prepare threads:
    nThreads = confman->getInt_f(myvprint("%s.nThreads",CM_CONF_INST));
    threadPriority = confman->getInt_f(myvprint("%s.threadPriority",CM_CONF_INST));
    if (threadPriority < -15) threadPriority = -15;
    if (threadPriority > 20) threadPriority = 20;
    if (threadPriority > 11) SMILE_WRN(2,"componentManager: Running SMILE threads with real-time default priority (prio = %i > 11)! Be careful with this, you mouse may hang or disk caches not get flushed!",threadPriority);

    // create compnent objects and register them
    for (i=0; i<N_; i++) {
      const char *k = insts[i];
      if (k!=NULL) {
        const char *tp = confman->getStr_f(myvprint("%s.instance[%s].type",CM_CONF_INST,k));
        const char *ci = confman->getStr_f(myvprint("%s.instance[%s].configInstance",CM_CONF_INST,k));
        // check config:
        if (tp == NULL) CONF_INVALID_ERR("%s.instance[%s].type is missing!",CM_CONF_INST,k);
        if (ci == NULL) ci = k;
        SMILE_DBG(2," adding %i. component instance: name '%s', type '%s', configInstance '%s'",i,k,tp,ci);
        int tmpId = confman->getInt_f(myvprint("%s.instance[%s].threadId",CM_CONF_INST,k));
        if (tmpId < -2) tmpId = -1;  // NOTE: threadId = -2 => do not tick this component at all!!
        if (tmpId >= nThreads) {
          //CONF_INVALID_ERR("%s.instance[%s].threadId must be < %s.nThreads (first Id = 0)!",CM_CONF_INST,k,CM_CONF_INST);
          SMILE_MSG(1, "[componentManager] threadId of instance %s must be < nThreads (%i). Setting to threadId 0.", k, nThreads);
          tmpId = 0;
        }
        /* TODO: add the reading of a *per component* threadPriority here, which will be passed to addComponent call */
        /*
        int threadPrio = threadPriority;
        char * ctmp = myvprint("%s.instance[%s].threadPriority",CM_CONF_INST,k);
        if (confman->isSet(ctmp))
        threadPrio = confman->getInt_f(ctmp);
        }
        */
        int a = addComponent(k,tp,ci,tmpId /*, threadPrio */);
        if (a>=0) {
          SMILE_DBG(4," added %i. component instance at index %i (lc=%i)",i,a,lastComponent);
          // set configInstance name...
          //          setConfigInstanceName..ok!
        } else {
          COMP_ERR("error during addComponent (returnVal=%i)!",a);
        }
      }
    }

    // configure and finalise components in turn:
    // 1a. register (components, NON dataMemory)
    // 1b. register (dataMemories)
    // 2a. configure (components, NON dataMemory)
    // 2b. configure (dataMemories)
    // 3a. finalise (components, NON dataMemory)
    // 3b. finalise (dataMemories)

    // we need to do this so the readers of e.g. dataProcessors can be configured after the writers for the corresponding levels have been finalised

    if (ciRegisterComps(0)) COMP_ERR("createInstances: failed registering component instances");
    if (ciRegisterComps(1)) COMP_ERR("createInstances: failed registering dataMemory instances");

    int nFinC = 0, nFinD = 0;
    if (ciConfFinComps(0,&nFinC)) COMP_ERR("createInstances: failed configuring&finalising component instances");
    //if (ciConfigureComps(0)) COMP_ERR("createInstances: failed configuring component instances");
    //if (ciFinaliseComps(0,&nFinC)) COMP_ERR("createInstances: failed finalising component instances");
    isConfigured=1;

    if (ciConfigureComps(1)) COMP_ERR("createInstances: failed configuring dataMemory instances");
    if (ciFinaliseComps(1,&nFinD)) COMP_ERR("createInstances: failed finalising dataMemory instances");
    isFinalised=1;

    SMILE_MSG(2,"successfully finished createInstances\n                                 (%i component instances were finalised, %i data memories were finalised)", nFinC, nFinD);
    ready = 1;
  }
}

int cComponentManager::getNextComponentId()
{
  if (lastComponent >= nComponentsAlloc) {  // reallocate a larger component array
    cSmileComponent **tmp;
    tmp = (cSmileComponent **) crealloc(component,
      sizeof(cSmileComponent *) * (lastComponent + COMPONENTMANAGER_DEFAULT_NCOMPS),
      sizeof(cSmileComponent *) * nComponentsAlloc);
    char **tmpn;
    tmpn = (char **) crealloc(componentInstTs,
      sizeof(char *) * (lastComponent + COMPONENTMANAGER_DEFAULT_NCOMPS),
      sizeof(char *) * nComponentsAlloc);
    int *tmpi;
    tmpi = (int *) crealloc(componentThreadId,
      sizeof(int) * (lastComponent + COMPONENTMANAGER_DEFAULT_NCOMPS),
      sizeof(int) * nComponentsAlloc);

    if (tmp == NULL) OUT_OF_MEMORY;
    if (tmpn == NULL) OUT_OF_MEMORY;
    if (tmpi == NULL) OUT_OF_MEMORY;
    nComponentsAlloc = (lastComponent + COMPONENTMANAGER_DEFAULT_NCOMPS);
    component = tmp;
    componentInstTs = tmpn;
    componentThreadId = tmpi;
  }
  nComponents++;
  return lastComponent++;
}

// ci = config instance name (set to _instname by default in createInstances)
int cComponentManager::addComponent(const char *_instname, const char *_type, const char *_ci, int _threadId /*, int threadPrio */)
{
  SMILE_DBG(3,"addComponent: instname='%s' type='%s'",_instname,_type);
  int t = findComponentType(_type);
  if (t >= 0) {
    cSmileComponent *c = createComponent(_instname,t);
    if (c==NULL) COMP_ERR("failed creating component '%s' (type: '%s')",_instname,_type);
    if (_ci != NULL) c->setConfigInstanceName(_ci);
    if (profiling) c->setProfiling(1, 0);
    return registerComponentInstance(c, _type, _threadId /*, threadPrio */);
  } else {
    SMILE_ERR(1,"cannot add component (instname='%s' type='%s'): unknown component type!!",_instname,_type);
  }

  return t;
}

cSmileComponent * cComponentManager::createComponent(const char *_instname, const char *_type)
{
  int t = findComponentType(_type);
  if (t >= 0) {
    return createComponent(_instname,t);
  } else {
    SMILE_ERR(1,"cannot create component (instname='%s' type='%s'): unknown component type!!",_instname,_type);
    return NULL;
  }
}

// n is index of component type in compTs
cSmileComponent * cComponentManager::createComponent(const char *_instname, int n)
{
  if ((n>=0)&&(n<nCompTs)) {
    cSmileComponent *c = (compTs[n].create)(_instname);
    if (c==NULL) OUT_OF_MEMORY;
    c->setComponentEnvironment(this, -1, NULL); // set component manager reference and the component ID, used by componentManager
    return c;
  } else {
    SMILE_ERR(1,"cannot create component of type (index=%i): index is out of range (0 - %i)!",n,nCompTs);
  }
  return NULL;
}

int cComponentManager::registerComponentInstance(cSmileComponent * _component, const char *_typename, int _threadId /*, int threadPrio */)
{
  if (_component == NULL) {
    SMILE_ERR(1,"registerComponentInstance: cannot register NULL component instance!");
    return -1;
  }
  int id = getNextComponentId();
  if (id >= 0) {
    _component->setComponentEnvironment(this, id, NULL);
    //_component->setId(id);
    component[id] = _component;
    if (_typename != NULL)
      componentInstTs[id] = strdup(_typename);
    else
      componentInstTs[id] = NULL;
    componentThreadId[id] = _threadId;
    // componentThreadPrio[id] = threadPrio;
  } else {
    SMILE_ERR(1,"registerComponentInstance: could not get next component id, return value == %i!",id);
  }
  return id;
}

void cComponentManager::unregisterComponentInstance(int id, int noDM)  // unregister and destroy component object
{
  // TODO:::: the component would have to unregister it's reader/writer etc...
  if ((id>=0)&&(id < lastComponent)&&(id < nComponentsAlloc)) {
    if (component[id] != NULL) {
      if ((noDM)&&(!strcmp(component[id]->getTypeName(),COMPONENT_NAME_CDATAMEMORY))) {
        return;
      } else {
        delete component[id];
        if (componentInstTs[id] != NULL) {
          free(componentInstTs[id]);
          componentInstTs[id] = NULL;
          componentThreadId[id] = 0;  // TODO: stop thread ?????
        }
        component[id] = NULL;
        nComponents--;
        if (id == lastComponent) lastComponent--;
      }
    }
  }
}

void cComponentManager::resetInstances()
{
  int i;
  // TODO: abort processing , stop threads...etc.??
  int mylastComponent = lastComponent;
  for (i=0; i<mylastComponent; i++) {
    unregisterComponentInstance(i,1);  // all BUT dataMemories...
  }

  mylastComponent = lastComponent;
  for (i=0; i<mylastComponent; i++) {
    unregisterComponentInstance(i); // now free the dataMemories
  }
  nComponents = 0;
  lastComponent = 0; // ???
  ready=0;    // flag that indicates if all components are set up and ready...
  isConfigured=0;
  isFinalised=0;
  EOI=0;
}

int cComponentManager::findComponentInstance(const char *_compname) const
{
  int i;
  if (_compname == NULL) return -1;
  for (i=0; i<lastComponent; i++) {
    if ((component[i] != NULL)&&(!strcmp(_compname,component[i]->getInstName()))) { return i; }
  }
  return -1;
}

cSmileComponent * cComponentManager::getComponentInstance(int n)
{
  if ((n>=0)&&(n<nComponents)) return component[n];
  else return NULL;
}

const char * cComponentManager::getComponentInstanceType(int n)
{
  if ((n>=0)&&(n<nComponents)) return componentInstTs[n];
  else return NULL;
}

cSmileComponent * cComponentManager::getComponentInstance(const char *_compname)
{
  return getComponentInstance(findComponentInstance(_compname));
}

const char * cComponentManager::getComponentInstanceType(const char *_compname)
{
  return getComponentInstanceType(findComponentInstance(_compname));
}

// send inter component messages directly to component (string version, takes recepient component name as first argument, sending to multiple components is possible, if they are separated via a comma ',' in the recepient name argument)
int cComponentManager::sendComponentMessage(const char *_compname, cComponentMessage *_msg)
{
  if (_compname != NULL) {
    char *inp = strdup(_compname);
    char *tmp = strchr(inp,','); // is array of recepients ?
    if (tmp != NULL) {
      char *t = inp;
      tmp = strchr(t,','); // is array of recepients ?
      tmp[0] = 0;
      int ret;
      // first part:
      ret = sendComponentMessage(findComponentInstance(t),_msg);
      // second part:
      tmp++;
      ret += sendComponentMessage(tmp, _msg);
      free(inp);
      return ret;
    } else {
      free(inp);
      return sendComponentMessage(findComponentInstance(_compname),_msg);
    }
  }
  return 0;
}

// send inter component messages directly to component:
int cComponentManager::sendComponentMessage(int compid, cComponentMessage *_msg)
{
  cSmileComponent *c = getComponentInstance(compid);
  int ret = 0;
  if (c!=NULL) {
    if (_msg != NULL) {
      // assign timestamp
      _msg->smileTime = getSmileTime();
      // assign unique message id from message counter:
      smileMutexLock(messageMtx);
      _msg->msgid = messageCounter++; // MUTEX!!
      smileMutexUnlock(messageMtx);
    }
    ret = c->receiveComponentMessage(_msg);
  }
  return ret;
}

double cComponentManager::getSmileTime()
{
  struct timeval curTime;
  gettimeofday( &curTime, NULL );
  return ( (double)(curTime.tv_sec - startTime.tv_sec) + (double)(curTime.tv_usec - startTime.tv_usec)/1000000.0 );
}

long cComponentManager::tick(int threadId, long long tickNr, long lastNrun)
{
  if (!ready) return 0;
  int i;
  long nRun = 0; int nRunnable = 0;
  char *stat=NULL;
  for (i=0; i<=lastComponent; i++) {
    if (component[i] != NULL) {
      if (((threadId == -1)||(threadId == componentThreadId[i]))&&(componentThreadId[i]!=-2)) {
        nRunnable++;
        //fprintf(stderr,"~~~~> 'ticking' component '%s' (idx %i)\n",component[i]->getInstName(),i); fflush(stderr);
        SMILE_DBG(4,"~~~~> 'ticking' component '%s' (idx %i)",component[i]->getInstName(),i);
        if (component[i]->tick(tickNr, EOIcondition, lastNrun)) {
          nRun++;
          SMILE_DBG(4,"%s.tick() executed successfully!",component[i]->getInstName());
          //#ifdef DEBUG
          if (execDebug) { // show summary of components executed during this tick
            if (stat != NULL) {
              char *x = stat;
              stat = myvprint("%s %s",x,component[i]->getInstName());
              free(x);
            } else {
              stat = strdup(component[i]->getInstName());
            }
          }
          //#endif // DEBUG
        }
      }
    }
  }
#ifdef DEBUG
  if (nRun < nRunnable) { SMILE_DBG(4,"not all components were run during tick %i (threadID: %i)! (%i<%i)",tickNr,threadId, nRun,nRunnable); }
  else { SMILE_DBG(4,"ran all components in thread %i (tick: %i)",threadId,(int)tickNr); }
#endif
  if (execDebug) { // show summary of components executed during this tick
    SMILE_PRINT("SUMMARY tick #%i thread %i, (eoi=%i) ran (%i): %s\n", (int)tickNr, (int)threadId, EOI, nRun, stat);
    if (stat != NULL) free(stat);
  }
  //#endif // DEBUG

  return nRun;
}


int cComponentManager::pausedNotifyComponents(int threadId, int isPause)
{
  if (!ready) return 0;
  int ret = 1;
  int i;
  for (i=0; i<=lastComponent; i++) {
    if (component[i] != NULL) {
      if (((threadId == -1)||(threadId == componentThreadId[i]))&&(componentThreadId[i]!=-2)) {
        if (isPause) {
          SMILE_DBG(4,"~~~~> pausing component '%s' (idx %i)",component[i]->getInstName(),i);
          if (!component[i]->pause()) {
            SMILE_ERR(2," component '%s' rejected pause.", component[i]->getInstName());
            ret = 0;
          }
        } else {
          SMILE_DBG(4,"~~~~> resuming component '%s' (idx %i)",component[i]->getInstName(),i);
          component[i]->resume();
        }
      }
    }
  }
  if (ret == 0) {
    SMILE_ERR(2,"Failed to pause tick loop.");
  }
  return ret;
}

/* pause tick loop:
how=1  :  notify components, halt loop in a sleep
how=2  :  notify components,  wait for nRun = 0 (force NOT EOI!), then put loop to sleep
timeout :  if how=2, then max number of ticks of nRun>0 , after which pause request is discarded and EOI is assumed again when nRun=0  (irrelevant when how=1)
*/
void cComponentManager::pause(int how, int timeout)
{
  // WARNING: pause/resume is currently only implemented for single thread processing!
  smileMutexLock(pauseMtx);
  if (tickLoopPaused && how > 0) {
    smileMutexUnlock(pauseMtx);  
    return; 
  }
  int oldTickLoopPaused = tickLoopPaused;
  tickLoopPaused = how;
  tickLoopPauseTimeout = timeout;
  pauseStartNr = -1;
  if (how == 0 && oldTickLoopPaused) {
    //smileCondBroadcast(pauseCond);  // for multi-threaded tick loops ?
    smileCondSignal(pauseCond);
  }
  smileMutexUnlock(pauseMtx);
}

int cComponentManager::pauseThisTickLoop(int threadID, int nRun, long long tickNr)
{
  smileMutexLock(pauseMtx);
  int ret = 0;
  if (tickLoopPaused) {
    if (tickLoopPaused == 1) {
      // notify components about the pause/sleep. this is very useful if components have asynchronous background threads or callbacks the process data independently (e.g. portaudio)
      ret = pausedNotifyComponents(threadID, 1 /*pause*/);
      if (!ret) {
        tickLoopPaused = 0;
        pauseStartNr = -1;
        pausedNotifyComponents(threadID, 0 /*resume*/);
        return nRun;
      }
      // tick loop pause will lead to a sleep at this point
      // thus, the pause my be initiated from the main thread (i.e. from any myTick() functions within components), but needs a *separate* thread to resume from it!!
      // this thread needs to have a pointer to the component manager to call the unpause function!
      smileCondWaitWMtx(pauseCond, pauseMtx);
      // notify components of resume:
      pausedNotifyComponents(threadID, 0 /*resume*/);
    } else if (tickLoopPaused == 2) {
      if (pauseStartNr == -1) { 
        pauseStartNr = tickNr; 
        // notify components about the pause/sleep. this is very useful if components have asynchronous background threads or callbacks the process data independently (e.g. portaudio)
        ret = pausedNotifyComponents(threadID, 1 /*pause*/);
        if (!ret) {
          tickLoopPaused = 0;
          pauseStartNr = -1;
          pausedNotifyComponents(threadID, 0 /*resume*/);
          return nRun;
        }
      } else if (tickNr - pauseStartNr > tickLoopPauseTimeout) { // resume due to timeout, loop still running
        tickLoopPaused = 0;
        pauseStartNr = -1;
        // notify components of resume
        pausedNotifyComponents(threadID, 0 /*resume*/);
      } else if (nRun <= 0) {
        // sleep until resumed  (TODO: or sleep fixed amount of time, if mode 3)
        smileCondWaitWMtx(pauseCond, pauseMtx);
        // notify components of resume  (TODO: do not resume, if in polling mode (3))
        pausedNotifyComponents(threadID, 0 /*resume*/);
        tickLoopPaused = 0;
        pauseStartNr = -1;
        nRun = 1; // continue tick loop
      }
    }
  }
  smileMutexUnlock(pauseMtx);

  // TODO: how=3 : no sleep, but polling of tick loop until resumed. configurable poll intervals.
  // TODO: in how=2 mode: check if resume is ok, or if next iteration after resume will return nRun=0 again because components are not ready yet

  return nRun;
}

void cComponentManager::setEOIcounterInComponents()
{
  for (int i = 0; i <= lastComponent; i++) {
    if (component[i] != NULL) {
      component[i]->setEOIcounter(EOI);
    }
  }
}

// single thread tick loop
long long cComponentManager::runSingleThreaded(long long maxtick)
{
  long nRun=-1;
  if (!ready) return 0;
  SMILE_MSG(2,"starting single thread processing loop");
#ifdef __WINDOWS
  /* set priority of current thread here... */
  SetThreadPriority(GetCurrentThread(), threadPriority);
#endif
  // TODO: external abort mechanism... (Message, etc.)
  int running = 1;
  long long tickNr = -1;
  do {
    int firstTick = 1;
    do {
      tickNr++;
      SMILE_DBG(4,"<------- TICK # %lld ---------->", tickNr);
      nRun = tick(-1, tickNr,nRun);
      if (firstTick && nRun == 0) {
        running = 0;
        SMILE_MSG(3, "Empty tick loop iteration # %i. tickNr = %lld. End of processing loop.", EOI, tickNr)
      }
      firstTick = 0;
      if ((maxtick != -1)&&(tickNr >= maxtick)) nRun = 0;
      smileMutexLock(abortMtx);
      if (abortRequest) nRun = 0;
      smileMutexUnlock(abortMtx);
      if (nRun > 0) userOnTick(tickNr,EOI);
      // pause function:  // WARNING: pause/resume is currently only implemented for single thread processing!
      nRun = pauseThisTickLoop(-1,nRun,tickNr);
    } while(nRun > 0);
    // TODO:
    // new run scheme
    //   run N tick loops, until first run of new tick loop after EOI has failed.
    //  signale EOI counter and EOI condition to components. The condition is set on every other tick loop.
    //
    nRun=-1;
    // TODO: if aborted (Ctrl+C etc.) do not run another EOI tick loop!
    if (running && ((tickNr < maxtick)||(maxtick==-1))) {
      setEOI();  // set EOIcondition and run another tick loop
      setEOIcounterInComponents();
      SMILE_MSG(3, "Entering 'EOI condition == 1' tick loop. EOI counter = %i. tickNr = %ld", EOI, tickNr);
      do {
        tickNr++;
        SMILE_DBG(4,"<------- TICK # %lld ---------->", tickNr);
        nRun = tick(-1, tickNr, nRun);
        if ((maxtick != -1)&&(tickNr >= maxtick)) nRun = 0;
        // ???
        smileMutexLock(abortMtx);
        if (abortRequest) {
          nRun = 0;
          running = 0;
        }
        smileMutexUnlock(abortMtx);
        if (nRun > 0) userOnTick(tickNr,EOI);
      } while(nRun > 0);
    }
    if (oldSingleIterationTickLoop) {
      running = 0;
    } else {
      unsetEOI();
    }
    // running = 0 if no component ran in first tick loop part..
    // EOI: EOI counter
    // EOIcondition : new variable flag, indicating if in EOI tick loop.
    if (running) {
      SMILE_MSG(3, "Starting the next tick loop iteration. EOI counter = %i. tickNr = %ld", EOI, tickNr);
    }
  } while (running);
  SMILE_MSG(2,"Processing finished! System ran for %i ticks.",tickNr);
  // do profiling:
  if (profiling) {
    double psum = 0.0;
    double *ps = (double*)calloc(1, sizeof(double) * (lastComponent + 1));
    for (int i=0; i<=lastComponent; i++) {
      if (component[i] != NULL) {
        ps[i] = component[i]->getProfile(1);
        psum += ps[i];
      }
    }
    SMILE_PRINT(" == Component run-time profiling ==");
    SMILE_PRINT("    Total time in component tick() in seconds: %f", psum);
    // normalise to percentages:
    if (psum > 0.0) {
      for (int i=0; i<=lastComponent; i++) {
        if (component[i] != NULL) {
          ps[i] /= psum;
          SMILE_PRINT("  %s:   %.1f %s", component[i]->getInstName(), ps[i] * 100.0, "%");
        }
      }
    }
    free(ps);
  }
  return tickNr;
}


SMILE_THREAD_RETVAL threadRunner(void *_data)
{
  sThreadData *data = (sThreadData*)_data;
  if ((data != NULL)&&(data->obj!=NULL)) {
    data->obj->waitForController(data->threadId,0);
    data->obj->tickLoopA(data->maxtick, data->threadId, data);
    // wait for all threads to be in this state....
    data->obj->waitForController(data->threadId,2);
    // post EOI processing
    data->obj->tickLoopA(data->maxtick, data->threadId, data);
  }
  SMILE_THREAD_RET;
}

void cComponentManager::waitForAllThreads(int threadID)
{
  smileMutexLock(waitEndMtx);
  SMILE_DBG(4,"thread controller entered waiting state.. %i threads of %i are now waiting\n",threadID,waitEndCnt,nThreads);

  if (waitEndCnt < nThreads) { 
    smileCondWaitWMtx(waitEndCond,waitEndMtx);
  }
  EOI = 1; 
  nActive = nThreads; //??
  //smileCondBroadcast(waitEndCond);
  endOfLoop=0;
  waitEndCnt = 0;
  controllerPresent = 0;
  smileMutexUnlock(waitEndMtx);
}

void cComponentManager::waitForController(int threadID, int stage)
{
  smileMutexLock(waitEndMtx);
  if (stage==2) {
    waitEndCnt++; 
    SMILE_DBG(4,"thread %i entered waiting state.. %i threads of %i are now waiting\n",threadID,waitEndCnt,nThreads);
    if (waitEndCnt == nThreads) {
      smileCondSignal(waitEndCond);
    }
    smileCondWaitWMtx(waitControllerCond,waitEndMtx);
  } else {
    if (!controllerPresent) { smileCondWaitWMtx(waitControllerCond,waitEndMtx); }
  }
  //EOI = _eoi; 
  //nActive = nThreads; //??
  //  smileCondBroadcast(waitEndCond);
  //endOfLoop=0;
  smileMutexUnlock(waitEndMtx);
}

SMILE_THREAD_RETVAL threadRunnerControl(void *_data)
{
  sThreadData *data = (sThreadData*)_data;
  if ((data != NULL)&&(data->obj!=NULL)) {
    data->obj->controlLoopA();
    // wait for all threads to be in this state....
    data->obj->waitForAllThreads(-1); // thread ID is only used for display of messages
    //    fprintf(stderr,"CR eoi=1\n");
    // post EOI processing
    data->obj->controlLoopA();
  }
  SMILE_THREAD_RET;
}

void cComponentManager::controlLoopA()
{
  int run = 1;

  smileMutexLock(syncCondMtx);
  endOfLoop=0;

  smileMutexLock(waitEndMtx);
  waitEndCnt=0;
  controllerPresent=1;
  smileCondBroadcast(waitControllerCond);
  smileMutexUnlock(waitEndMtx);

  do {
    if (!abortRequest) smileCondWaitWMtx(controlCond,syncCondMtx);
    // if control cond was signald, all thread are waiting...!
    // set a flag, an thus execute each thread loop excatly ONCE!
    // then check the status, if still all are waiting, exit, else continue!
    if (probeFlag) {
      //      fprintf(stderr,"probe end  cr=%i nP %i nA %i\n",compRunFlag,nProbe,nActive);fflush(stderr);
      if (compRunFlag == 0) {
        // final exit!
        run = 0;
      } else {
        // continue
        nProbe = 0;
        nWaiting = 0;
        probeFlag = 0;
        //cnt = 0;
      }
    } else {
      // probe components..
      //      fprintf(stderr,"probe start nW %i\n",nWaiting); fflush(stderr);
      nWaiting = 0;
      compRunFlag = 0;
      probeFlag = 1;
      nProbe = 0;
    }

    smileMutexLock(abortMtx);
    if (abortRequest) { run = 0; }
    else if (run==0) { endOfLoop = 1; }
    smileMutexUnlock(abortMtx);
    //    fprintf(stderr,"cr stat nW %i nP %i ab %i run %i pf %i\n",nWaiting,nProbe,abortRequest,run,probeFlag); fflush(stderr);

    smileCondBroadcast(syncCond); 

  } while(run);
  smileMutexUnlock(syncCondMtx);

  compRunFlag = 0; probeFlag = 0; nProbe = 0; nWaiting = 0;
}


long long cComponentManager::tickLoopA(long long maxtick, int threadId, sThreadData *_data)
{
  int nRun; int _abort = 0; int doYield=0;
  long long tickNr = -1;

  SMILE_MSG(3,"starting processing loop of thread %i",threadId);

  do {
    smileMutexLock(syncCondMtx);
    // if runFlag == 0 here...
    runFlag[threadId] = 0;
    tickNr++; //fprintf(stderr, "tick %i eol %i eoi %i\n",tickNr,endOfLoop,EOI); fflush(stderr);
    SMILE_DBG(3,"<------- TICK # %i (thread %i) ---------->",tickNr,threadId);
    smileMutexUnlock(syncCondMtx);
    nRun = tick(threadId,tickNr);
    SMILE_DBG(3,"nRun = %i (thread %i)",nRun,threadId);

    smileMutexLock(abortMtx);
    if (abortRequest) {
      _abort = 1;
    }
    smileMutexUnlock(abortMtx);

    if (!_abort) {

      smileMutexLock(syncCondMtx);
      //    fprintf(stderr,"mutlock...%i\n",threadId);
      if (nRun==0) {
        if (runFlag[threadId] == 1) {
          runFlag[threadId] = 0;
          nRun = 1;
        } else {
          if (!probeFlag) {
            nWaiting++;
            if (nWaiting < nActive) {
              //SMILE_MSG(3, "thread %i sleeping (tick %i) (nAct %i, nWait %i)", threadId, tickNr, nActive, nWaiting);
              smileCondWaitWMtx(syncCond,syncCondMtx); // sleep until another thread or the controller thread wakes us
              //nRun = 1; // runStatus is set by the controller thread, it is 1 by default (=continue)
            } else {
              smileCondSignal(controlCond); // wake up controller thread...
              //SMILE_MSG(3, "thread %i sleeping and signalled controller (tick %i) (nAct %i, nWait %i)", threadId, tickNr, nActive, nWaiting);
              smileCondWaitWMtx(syncCond,syncCondMtx); // and sleep until the controller thread OR ANOTHER THREAD! wakes us
              //nRun = 1; // runStatus is set by the controller thread, it is 1 by default (=continue)
            }
            if (!probeFlag && nWaiting > 0)
              nWaiting--;
          } else {
            nProbe++;
            //nWaiting++;
            if (nProbe == nActive) {
              smileCondSignal(controlCond); // wake up controller thread if we are the last in the probe...
              //SMILE_MSG(3, "PROBE: thread %i signalled controller (tick %i), nprobe = %i, nA %i, nW %i", threadId, tickNr, nProbe, nActive, nWaiting);
            }
            //SMILE_MSG(3, "PROBE: thread %i sleeping (tick %i), nprobe = %i", threadId, tickNr, nProbe);
            smileCondWaitWMtx(syncCond,syncCondMtx); // sleep until the controller thread OR ANOTHER THREAD! wakes us
            //if (nWaiting > 0) nWaiting--;
            //if (probeFlag) nProbe--;
          }
          nRun = 1;
          doYield = 0;
        }
      } else {
        userOnTick(tickNr,EOI,threadId);
        for (int i = 0; i < nActive; i++) {
          runFlag[i] = 1;
        }
        //for (int i=0; i<nActive; i++){
        //  smileCondSignalRaw(syncCond);
        //}
        //if (!probeFlag) 
        //else {
        compRunFlag = 1; 
        //SMILE_MSG(3, "PROCESSED thread %i run>0 (tick %i), nprobe = %i", threadId, tickNr, nProbe);
        if (probeFlag) {
          nProbe++;
          // TODO: actually once one component has run, we can exit probe.. however, if we do the nWaiting counter is wrong and we enter a deadlock (all waiting afterwards)
          if (nProbe == nActive) {
            //SMILE_MSG(3, "PROBE: thread %i run>0 and signalled controller (tick %i), nprobe = %i, nA %i, nW %i", threadId, tickNr, nProbe, nActive, nWaiting);
            smileCondSignal(controlCond);
          }
          //SMILE_MSG(3, "PROBE: thread %i run>0 sleeping (tick %i), nprobe = %i", threadId, tickNr, nProbe);
          smileCondWaitWMtx(syncCond,syncCondMtx); // sleep until the controller thread OR ANOTHER THREAD! wakes us
        }
        /*else {
          smileCondBroadcastRaw(syncCond);
          if (nWaiting == nActive - 1) {
            nWaiting++;
            //SMILE_MSG(3, "thread %i sleeping while waiting for others (tick %i) (nAct %i, nWait %i)", threadId, tickNr, nActive, nWaiting);
            smileCondWaitWMtx(syncCond,syncCondMtx); // sleep until the controller thread OR ANOTHER THREAD! wakes us
          }
        }*/
        //nProbe++;
        //if (nProbe == nActive) smileCondSignal(controlCond); // wake up controller thread...
        //smileCondWaitWMtx(syncCond,syncCondMtx); // and sleep until the controller thread wakes us
        //}
        doYield=0;

        // TODO: if all others waiting... make sure we sleep at the end

      }
      if ((_data->maxtick != -1)&&(tickNr >= _data->maxtick)) nRun = 0;
      smileMutexUnlock(syncCondMtx);
    }

    smileMutexLock(abortMtx);
    if ((endOfLoop)||(_abort)) { nRun = 0; }
    smileMutexUnlock(abortMtx);

    smileMutexLock(syncCondMtx);
    if (nRun == 0) nActive--;
    smileMutexUnlock(syncCondMtx);
    if (doYield) { smileYield(); doYield = 0; }

  } while (nRun>0) ;

  SMILE_MSG(3,"leaving processing loop of thread %i (ticks: %i)",threadId,tickNr);
  // first part... now enter EOI state, however wait for other threads...???

  return tickNr;

}


#if 0
long long cComponentManager::tickLoopA(long long maxtick, int threadId, sThreadData *_data)
{
  int nRun; int _abort = 0; int doYield=0;
  long long tickNr = -1;
  int weDidRun = 0;
  SMILE_MSG(3,"starting processing loop of thread %i",threadId);

  do {
    smileMutexLock(syncCondMtx);
    // if runFlag == 0 here...
    runFlag[threadId] = 0;
    tickNr++; //fprintf(stderr, "tick %i eol %i eoi %i\n",tickNr,endOfLoop,EOI); fflush(stderr);
    SMILE_DBG(3,"<------- TICK # %i (thread %i) ---------->",tickNr,threadId);
    smileMutexUnlock(syncCondMtx);
    nRun = tick(threadId,tickNr);
    SMILE_DBG(3,"nRun = %i (thread %i)",nRun,threadId);
    smileMutexLock(abortMtx);
    if (abortRequest) {
      _abort = 1;
    }
    smileMutexUnlock(abortMtx);

    if (!_abort) {
      smileMutexLock(syncCondMtx);
      if (nWaiting == nActive - 1) {  // all waiting except us.. looks like a probe round
        nProbe++;
      }
      if (nRun > 0) {
        smileCondBroadcastRaw(syncCond);
        SMILE_MSG(3, "thread %i processed data (%i)", threadId, (int)tickNr);
        //compRunFlag = 1;
        for (int i = 0; i < nActive; i++) {
            runFlag[i] = 1;
        }
        weDidRun = 1;
        // we need some mechanism to ensure that if we just ran, but then will be waiting for all other threads,
        // that all other threads have had a chance to run before we terminate!
      } else {
        if (runFlag[threadId] == 1) {
          runFlag[threadId] = 0;
          nRun = 1;
        } else {
          //smileCondBroadcastRaw(syncCond);
          nWaiting++;
          // actually: if nWaiting==nActive  terminate!
          if (nWaiting == nActive) {
            // we need some mechanism to ensure that if we just ran, but then will be waiting for all other threads,
                    // that all other threads have had a chance to run before we terminate!
            if (weDidRun == 1) {
              // check a counter == nActive - 1?
              if (nProbe == nActive - 1) {
                nRun = 0;
                SMILE_MSG(3, "thread %i ending AFTER PROBE, tick %i -> ALL WAITING NOW (nW = %i)", threadId, (int)tickNr, nWaiting);
              } else {
                nRun = 1;
                SMILE_MSG(3, "thread %i re-run (probe), tick %i -> ALL WAITING NOW (nW = %i)", threadId, (int)tickNr, nWaiting);
              }
            } else {
              SMILE_MSG(3, "thread %i ending, tick %i -> ALL WAITING NOW (nW = %i)", threadId, (int)tickNr, nWaiting);
              nRun = 0;
            }
            nWaiting--;
          } else {
            SMILE_MSG(3, "thread %i sleeping, tick %i (nW = %i)", threadId, (int)tickNr, nWaiting);
            smileCondWaitWMtx(syncCond,syncCondMtx); // sleep until another thread or the controller thread wakes us
            nWaiting--;
            SMILE_MSG(3, "thread %i woken (%i)", threadId, (int)tickNr);
            nRun = 1;
            weDidRun = 0;
          }
        }
      }
      if (nProbe >= nActive - 1) {
        nProbe = 0;
        weDidRun = 1;
      }
      smileMutexUnlock(syncCondMtx);
      if ((_data->maxtick != -1)&&(tickNr >= _data->maxtick)) nRun = 0;
    }
    smileMutexLock(abortMtx);
    if (_abort) { nRun = 0; }
    smileMutexUnlock(abortMtx);
  } while (nRun>0) ;

  smileMutexLock(syncCondMtx);
  nActive--;
  smileMutexUnlock(syncCondMtx);

  SMILE_MSG(3,"leaving processing loop of thread %i (ticks: %i)",threadId,tickNr);
  // first part... now enter EOI state, however wait for other threads...???

  return tickNr;

}
#endif
#if 0
// multi thread tick loop (tick loop for one of many threads)
long long cComponentManager::tickLoopA(long long maxtick, int threadId, sThreadData *_data)
{
  int nRun;
  long long tickNr = -1;
  int nRun0cnt = 0;

  SMILE_MSG(2,"starting processing loop of thread %i",threadId);

  /*
  smileMutexLock(syncCondMtx);
  // TODO: BUG, if nActive == 1, because other threads have not yet been started!!! (maybe initialise nActive with nThreads!?)
  nActive++;
  smileMutexUnlock(syncCondMtx);
  */

  do {
    tickNr++;
    SMILE_DBG(4,"<------- TICK # %i (thread %i) ---------->",tickNr,threadId);
    nRun = tick(threadId,tickNr);
    //    smileYield();
    SMILE_DBG(4,"nRun = %i (thread %i)",nRun,threadId);
    smileMutexLock(syncCondMtx);
    //    fprintf(stderr,"mutlock...%i\n",threadId);
    if (nRun==0) {
      // if not all waiting... wait!
      //          fprintf(stderr,"wc? %i nW %i nA %i\n",threadId,nWaiting,nActive); fflush(stderr);
      //if (nWaiting+nWaiting2 < nActive) {

      if (nWaiting+nWaiting2 < nActive-1) {
        if (_data->status == THREAD_ACTIVE) {
          nWaiting++;
        }
        if (_data->status == THREAD_WAIT_B) {
          nWaiting2--;
          nWaiting++;
        }
        _data->status = THREAD_WAIT_A;

        nRun0cnt = 0;
        //fprintf(stderr,"entering wait cond %i  (T=%i) nW %i nA %i\n",threadId,tickNr,nWaiting,nActive); fflush(stderr);
        //smileCondBroadcastRaw(syncCond);
        //fprintf(stderr,"w1 state %i  (T=%i) nW %i nA %i nW2 %i\n",threadId,tickNr,nWaiting,nActive,nWaiting2); fflush(stderr);
        if (nWaiting + nWaiting2 >= nActive) { smileCondBroadcastRaw(syncCond); }
        smileCondWaitWMtx(syncCond,syncCondMtx);
        //fprintf(stderr,"w1 state end %i  (T=%i) nW %i nA %i nW2 %i\n",threadId,tickNr,nWaiting,nActive,nWaiting2); fflush(stderr);
        //fprintf(stderr,"exiting wait cond %i\n",threadId); fflush(stderr);
        //nWaiting--;
        nRun=1;
      } //else  //all threads seem to have failed.... nRun=0, this will exit the first loop
      else {
        if (_data->status == THREAD_WAIT_A) {
          nWaiting--; 
        }
        if (_data->status != THREAD_WAIT_B) {
          nWaiting2++; _data->status = THREAD_WAIT_B;
        }
        int nWaiting_old = nWaiting;
        fprintf(stderr,"w2 state %i  (T=%i) nW %i nA %i nW2 %i\n",threadId,tickNr,nWaiting,nActive,nWaiting2); fflush(stderr);
        //nWaiting2++;
        //_data->status = THREAD_WAIT_B;
        //if (nWaiting + nWaiting2 >= nActive) {smileCondBroadcastRaw(syncCond); }
        smileCondBroadcastRaw(syncCond);
        //fprintf(stderr,"bc w2\n"); fflush(stderr);
        if (nWaiting2 < nActive) {
          nRun = 1;
          smileCondWaitWMtx(syncCond,syncCondMtx);
        } else {
          // exit for sure (?)... inform other threads?
          fprintf(stderr,"exit of %i\n",threadId); fflush(stderr);
        }
        if (nWaiting_old > nWaiting) {
          _data->status = THREAD_ACTIVE;
          nWaiting2--;
        }

        //fprintf(stderr,"w2 state end %i  (T=%i) nW %i nA %i nW2 %i\n",threadId,tickNr,nWaiting,nActive,nWaiting2); fflush(stderr);

        //nRun0cnt++;

        //if (nRun0cnt <= 1) { nRun = 1; 
        //fprintf(stderr,"nRun0cnt %i = %i\n",threadId,nRun0cnt); fflush(stderr);
        //nWaiting++;
        //           nWaiting++;
        //        smileCondBroadcastRaw(syncCond);
        //smileCondWaitWMtx(syncCond,syncCondMtx);
        //        smileMutexUnlock(syncCondMtx);
        //        smileYield();
        //        smileMutexLock(syncCondMtx);
        //        nWaiting--;
        //smileCondWaitWMtx(syncCond,syncCondMtx);
        //nWaiting--;
        //}
      }
      //}
    } else {
      //      smileMutexLock(syncCondMtx);
      if (_data->status == THREAD_WAIT_A) {
        _data->status = THREAD_ACTIVE;
        nWaiting--;
      } else
        if (_data->status == THREAD_WAIT_B) {
          _data->status = THREAD_ACTIVE;
          //nWaiting--; 
          nWaiting2--;
        }

        smileCondBroadcastRaw(syncCond); 
        //fprintf(stderr,"bc ok\n"); fflush(stderr);
        userOnTick(tickNr,EOI,threadId);
        nRun0cnt = 0;
    }
    if ((maxtick != -1)&&(tickNr >= maxtick)) nRun = 0;
    smileMutexLock(abortMtx);
    if (abortRequest) {
      nRun = 0;
    }
    smileMutexUnlock(abortMtx);

    if (nRun == 0) nActive--;

    smileMutexUnlock(syncCondMtx);

    smileYield(); // !!!!?
  } while(nRun > 0);

  //  smileMutexLock(syncCondMtx);
  // we must signal, if we exit, in order to wake up other threads, to allow them to check , if they are finished too
  //  smileCondBroadcastRaw(syncCond);
  //  smileMutexUnlock(syncCondMtx);
  if (_data->status == THREAD_WAIT_A) {
    _data->status = THREAD_ACTIVE;
    nWaiting--;
  } else
    if (_data->status == THREAD_WAIT_B) {
      _data->status = THREAD_ACTIVE;
      //nWaiting--; 
      nWaiting2--;
    }

    smileCondBroadcastRaw(syncCond); 
    SMILE_MSG(2,"leaving processing loop of thread %i (ticks: %i)",threadId,tickNr);
    // first part... now enter EOI state, however wait for other threads...???

    return tickNr;
}
#endif

// multi thread tick loop (tick loop for one of many threads)
//long long cComponentManager::tickLoopB(long long maxtick, int threadId)//
//{
//TODO... post EOI processing... (not needed... we will try to use tickLoopA again..)

//  return 0;
//}

//
// !!! THIS function should be always preferred, since it determines single/multi thread from config !!!
//
// multi threaded run: create threads for multiple tick loops and control/synchronize them...
long long cComponentManager::runMultiThreaded(long long maxtick)
{
  if (!ready) return 0;
  int i;

  if (nThreads == 1) {
    return runSingleThreaded(maxtick);
  } else {
    if (nThreads == 0) {
      int j=0;
      // int * threadPrios = (int *) calloc(1,sizeof(int)*lastComponent);
      for (i=0; i<lastComponent; i++) {
        if (componentThreadId[i] != -2) {
          /* TODO : store threadPrio in an array indexed by 1..j 
          threadPrios[j] = componentThreadPrio[i];
          */
          componentThreadId[i] = j++;
        }
      }
      nThreads = j;
    } else {
      for (i=0; i<lastComponent; i++) {
        if (componentThreadId[i] == -1) componentThreadId[i] = 0; // run in first thread
      }
    }
    SMILE_MSG(2,"starting mutli-thread processing with %i threads",nThreads);
    // create thread handles and data structures
    runFlag = (int *)malloc(sizeof(int) * nThreads);
    sThreadData *threadData = (sThreadData*)malloc(sizeof(sThreadData) * nThreads);
    smileThread *threadHandles = (smileThread *)malloc(sizeof(smileThread) * nThreads);
    smileMutexCreate(syncCondMtx);
    smileCondCreate(syncCond);
    smileCondCreate(waitEndCond);
    smileCondCreate(waitControllerCond);

    nActive = nThreads;
    waitEndCnt = 0;
    smileMutexCreate(waitEndMtx);

    // create the controller thread...
    smileCondCreate(controlCond);
    sThreadData control;
    control.obj = this;
    control.threadId = -1;
    smileThread controlThr;
    runStatus = 1; compRunFlag = 0; probeFlag = 0; nProbe = 0; endOfLoop=0; controllerPresent=0;
    if (!(int)smileThreadCreate(controlThr, threadRunnerControl, &control)) {
      SMILE_ERR(1,"error creating controller thread!!",i);
    }
    // create all the threads....
    for (i=0; i<nThreads; i++) {
      threadData[i].obj = this;
      threadData[i].maxtick = maxtick;
      threadData[i].threadId = i;
      threadData[i].status = THREAD_ACTIVE;
      if (!(int)smileThreadCreate(threadHandles[i], threadRunner, &(threadData[i]))) {
        //      if (!((long)(threadHandles[i] = (HANDLE)_beginthread(threadRunner,0,&(threadData[i]))) != -1)) {
        SMILE_ERR(1,"error creating thread with threadId %i!!",i);
        smileMutexLock(syncCondMtx);
        nActive--;
        smileMutexUnlock(syncCondMtx);
      } else {
        /* TODO: set priority of current thread here... */
#ifdef __WINDOWS
        //SetThreadPriority(threadHandles[i] , threadPrios[i] );
#endif
      }
    }
    //free(threadPrios);

    // wait for all threads to finish
    for (i=0; i<nThreads; i++) {
      //      fprintf(stderr,"joining thr %i (nA=%i)\n",i,nActive); fflush(stderr);
      smileThreadJoin(threadHandles[i]);
      threadData[i].status = THREAD_INACTIVE;
    }

    // and the controller thread
    //    fprintf(stderr,"joining controller (nA=%i)\n",nActive); fflush(stderr);
    smileCondSignal(controlCond);
    smileThreadJoin(controlThr);
    //fprintf(stderr,"joined controller\n");

    smileMutexDestroy(waitEndMtx);
    smileMutexDestroy(syncCondMtx);
    smileCondDestroy(syncCond);
    smileCondDestroy(controlCond);
    smileCondDestroy(waitEndCond);
    smileCondDestroy(waitControllerCond);

    // destroy all threads and thread data:
    if (runFlag != NULL) free(runFlag);
    if (threadData != NULL) free(threadData);
    if (threadHandles != NULL) free(threadHandles);

    // do profiling (TODO: correct for multi-threading!?):
    if (profiling) {
      double psum = 0.0;
      double *ps = (double*)calloc(1, sizeof(double) * (lastComponent + 1));
      for (int i=0; i<=lastComponent; i++) {
        if (component[i] != NULL) {
          ps[i] = component[i]->getProfile(1);
          psum += ps[i];
        }
      }
      SMILE_PRINT(" == Component run-time profiling ==");
      SMILE_PRINT("    Total time in component tick() in seconds: %f", psum);
      // normalise to percentages:
      if (psum > 0.0) {
        for (int i=0; i<=lastComponent; i++) {
          if (component[i] != NULL) {
            ps[i] /= psum;
            SMILE_PRINT("  %s:   %.1f %s", component[i]->getInstName(), ps[i] * 100.0, "%");
          }
        }
      }
      free(ps);
    }
  }
  return 1; // TODO: tickNr??
}


cComponentManager::~cComponentManager()
{
  int i;

  resetInstances();

  for (i=0; i<lastComponent; i++) {
    //    unregisterComponentInstance(i);
    if ((componentInstTs!=NULL)&&(componentInstTs[i] != NULL)) free(componentInstTs[i]);
  }
  if (componentThreadId != NULL) free(componentThreadId);
  if (component != NULL) free(component);
  if (compTs != NULL) free(compTs);
  if (componentInstTs != NULL) free(componentInstTs);

  smileMutexDestroy(messageMtx);
  smileMutexDestroy(abortMtx);
  smileCondDestroy(pauseCond);
  smileMutexDestroy(pauseMtx);

#ifdef SMILE_SUPPORT_PLUGINS   // NOTE: the config Manager must be freed before this
#ifndef __STATIC_LINK
  //close dynlibs of plugins and free memory in plugin dlls/dynlibs:
  if ((handlelist != NULL)&&(nPluginHandles>0)) {
    for (i=0; i<nPluginHandles; i++) {
#ifdef __WINDOWS
      // unregister...
      unRegisterFunction unRegFn = (unRegisterFunction) GetProcAddress(handlelist[i], "unRegisterPluginComponent");
      if (unRegFn != NULL) { (*unRegFn)(); }
      FreeLibrary(handlelist[i]);
#else
      // unregister...
      const char *error;
      unRegisterFunction unRegFn = (unRegisterFunction)dlsym(handlelist[i], "unRegisterPluginComponent");
      if (!( ((error = dlerror()) != NULL) || (unRegFn == NULL))) {
        (*unRegFn)(); 
      }
      dlclose(handlelist[i]);
#endif
    }
    free(handlelist);
  }
  if (regFnlist != NULL) free(regFnlist);
#endif
#endif

}
