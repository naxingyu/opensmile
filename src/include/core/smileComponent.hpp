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


#ifndef __SMILE_COMPONENT_HPP
#define __SMILE_COMPONENT_HPP

#include <core/smileCommon.hpp>
#include <core/configManager.hpp>

#define COMPONENT_DESCRIPTION_XXXX  "example description"
#define COMPONENT_NAME_XXXX         "exampleName"

#undef class
class DLLEXPORT cComponentManager;
class DLLEXPORT cSmileComponent;

#define CMSG_textLen      64
#define CMSG_typenameLen  32
#define CMSG_nUserData     8

typedef enum  {
  CUSTDATA_BINARY = 0, // unknown binary data
  CUSTDATA_TEXT = 100,   // null terminated string
  CUSTDATA_CHAR = 110,   // char array (size given in bytes by custDataSize)
  CUSTDATA_INT  = 200,    // array of ints (size given in bytes by custDataSize)
  CUSTDATA_FLOAT = 300,   // array of floats (size given in bytes by custDataSize)
  CUSTDATA_DOUBLE = 400,   // array of doubles (size given in bytes by custDataSize)
  CUSTDATA_FLOAT_DMEM = 500,   // array of float_dmem (size given in bytes by custDataSize)
} eSmileMessageCustDataType;

class DLLEXPORT cComponentMessage {
public:
  char msgtype[CMSG_typenameLen];     // message type name (used by receiver to identify message), set by constructor
  char msgname[CMSG_typenameLen];     // custom message name
  const char * sender;  // name of sender component (filled in by sendComponentMessage in cSmileComponent)
  double smileTime;  // seconds (accurate up to milliseconds) since componentManager startup (filled in by componentManager, TODO!)
  double userTime1;  // user defined time
  double userTime2;  // user defined time
  double readerTime; // readerTime in seconds (derived from vec->tmeta->vIdx and reader period in the tick the message ist sent), can also be -1.0 if not used!
  long msgid;           // custom message id
  // -- message data --
  double floatData[CMSG_nUserData];  // 8 freely usable doubles for message data, initialized with 0
  long intData[CMSG_nUserData];      // 8 freely usable ints
  char msgtext[CMSG_textLen];     // 64 characters for message text, freely usable, initialized with all 0
  int userflag1, userflag2, userflag3;
  void * custData;      // pointer to custom message data (allocated by sender, to be freed by sender after call to sendComponentMessage)
  void * custData2;     // pointer to custom message data (allocated by sender, to be freed by sender after call to sendComponentMessage)
  long custDataSize, custData2Size;   // size (in bytes) of custData arrays. Used for network message senders (FIXME: implment this properly in all components that allocate custData!)
  eSmileMessageCustDataType custDataType, custData2Type;   // Type of custData arrays. Used for network message senders (FIXME: implment this properly in all components that allocate custData!)

  cComponentMessage(const char *_type=NULL, const char *_name=NULL) :  msgid(-1), sender(NULL), smileTime(0.0), readerTime(-1.0), userflag1(0), userflag2(0), userflag3(0), 
    custData(NULL), custData2(NULL), custDataSize(0), custData2Size(0),
    custDataType(CUSTDATA_BINARY), custData2Type(CUSTDATA_BINARY)
  {
    if (_type==NULL) memset(msgtype, 0, sizeof(char)*CMSG_typenameLen);
    else strncpy(msgtype, _type, CMSG_typenameLen);
    if (_name==NULL) memset(msgname, 0, sizeof(char)*CMSG_typenameLen);
    else strncpy(msgname, _name, CMSG_typenameLen);
    memset(floatData, 0, sizeof(double)*CMSG_nUserData);
    memset(intData, 0, sizeof(long)*CMSG_nUserData);
    memset(msgtext, 0, sizeof(char)*CMSG_textLen);
  }
};

class DLLEXPORT sComponentInfo { public:
  int registerAgain;
  const char *componentName;
  const char *description;
  int abstract;   // flag that indicates whether component is 'abstract' only (i.e. without practical functionality)
  int noDmem;
  int builtIn;   // 1= built in component ; 0= dynamically linked component
  cSmileComponent * (*create) (const char *_instname);
  sComponentInfo * next;
};

// create for a real class (which implements fetchConfig() )
#define SMILECOMPONENT_CREATE(TP) cSmileComponent * TP::create(const char*_instname) { \
                                                  cSmileComponent *c = new TP(_instname); \
                                                  if (c!=NULL) c->setComponentInfo(sconfman,scname,sdescription); \
                                                  return c; \
                                                  }
                                                  
// create for an abstract class
#define SMILECOMPONENT_CREATE_ABSTRACT(TP) cSmileComponent * TP::create(const char*_instname) { return NULL; }

// static declaration in Cpp file of derived class
#define SMILECOMPONENT_STATICS(TP)  cConfigManager *TP::sconfman; \
                                    const char *TP::scname; \
                                    const char *TP::sdescription;
                                    
// static declarations in derived class (public)
#define SMILECOMPONENT_STATIC_DECL  static sComponentInfo * registerComponent(cConfigManager *_confman, cComponentManager *_compman);    \
                                    static cSmileComponent * create(const char *_instname);
                                    
// static declarations in derived class (protected)
#define SMILECOMPONENT_STATIC_DECL_PR    static cConfigManager *sconfman;    \
                                         static const char *scname;  \
                                         static const char *sdescription;

#define SMILECOMPONENT_REGCOMP(TP)  sComponentInfo * TP::registerComponent(cConfigManager *_confman, cComponentManager *_compman)
#define SMILECOMPONENT_REGCOMP_INIT  if (_confman == NULL) return NULL; \
                                     int rA = 0; \
									 sconfman = _confman;

#define SMILECOMPONENT_CREATE_CONFIGTYPE    ConfigType *ct = new ConfigType(scname); \
	                                        if (ct == NULL) OUT_OF_MEMORY;
#define SMILECOMPONENT_INHERIT_CONFIGTYPE(configtype)  ConfigType *ct = NULL; \
	                                                   const ConfigType *r = sconfman->getTypeObj(configtype); \
													   if (r == NULL) {  \
													     SMILE_WRN(4,"%s config Type not found!","configtype"); \
														 rA=1; \
													   } else { \
													     ct = new ConfigType( *(r) , scname ); \
													   }

#define SMILECOMPONENT_REGISTER_CONFIGTYPE     if (rA==0) { \
	                                             ConfigInstance *Tdflt = new ConfigInstance( scname, ct, 1 );  \
												 sconfman->registerType(Tdflt); \
                                               }

#define SMILECOMPONENT_IFNOTREGAGAIN_BEGIN  if (rA==0) {
#define SMILECOMPONENT_IFNOTREGAGAIN_END  SMILECOMPONENT_REGISTER_CONFIGTYPE }
#define SMILECOMPONENT_IFNOTREGAGAIN(__code__) SMILECOMPONENT_IFNOTREGAGAIN_BEGIN __code__ ; SMILECOMPONENT_IFNOTREGAGAIN_END

#define SMILECOMPONENT_MAKEINFO(TP)  if ((rA!=0)&&(ct!=NULL)) delete ct; \
	                                 return makeInfo(sconfman, scname, sdescription, TP::create, rA)
#define SMILECOMPONENT_MAKEINFO_ABSTRACT(TP) if ((rA!=0)&&(ct!=NULL)) delete ct; \
	                                         return makeInfo(sconfman, scname, sdescription, TP::create, rA, 1, 1)

// make info for non-datamemory components (e.g. the cFunctionalXXXX components), e.g. components that are sub-components of other components
#define SMILECOMPONENT_MAKEINFO_NODMEM(TP) if ((rA!=0)&&(ct!=NULL)) delete ct; \
	                                       return makeInfo(sconfman, scname, sdescription, TP::create, rA, 0, 1)

// make info for non-datamemory components (e.g. the cFunctionalXXXX components), e.g. components that are sub-components of other components
#define SMILECOMPONENT_MAKEINFO_NODMEM_ABSTRACT(TP) if ((rA!=0)&&(ct!=NULL)) delete ct; \
	                                                return makeInfo(sconfman, scname, sdescription, TP::create, rA, 1, 1)


#undef class
class DLLEXPORT cSmileComponent {
  private:
    int id_;           // component ID in componentManager
    int EOI_;          // EOI counter, 0 only in first loop, then +1 for every nonEOI/EOI loop pair
    int EOIcondition_; // flag that indicates end of input
                      // i.e. if EOI is 1, myTick should show a different behaviour
                      //  esp. dataReaders should return right padded matrices, in getMatrix , getNextMatrix etc..
    int paused_; // flag that indicates whether processing (the tick loop) has been paused or is active
    
    smileMutex  messageMtx_;

    cComponentManager *compman_;  // pointer to component manager this component instance belongs to
    cSmileComponent *parent_;     // pointer to parent component (for dataReaders, etc.)
    char *iname_;   // name of component instance
    char *cfname_;  // name of config instance associated with this component instance

    // variables used for component profiling
    int doProfile_, printProfile_;
    double profileCur_, profileSum_; // exec. time of last tick, exec time total
    struct timeval startTime_;
    struct timeval endTime_;

    long lastNrun_;   // the number of Nrun in the last tickloop?

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    cConfigManager *confman_;  // pointer to configManager
    const char *cname_;        // name of the component (type)
    const char *description_;  // component description and usage information

    // component state variables
    int isRegistered_, isConfigured_, isFinalised_, isReady_;
    int runMe_;

    int manualConfig_;
    int override_;
    
    static sComponentInfo * makeInfo(cConfigManager *_confman,
                                     const char *_name, const char *_description,
                                     cSmileComponent * (*create) (const char *_instname),
                                     int regAgain=0, int _abstract=0, int _nodmem=0);

    // Gets a pointer to the component instance *name via the component manager.
    cSmileComponent * getComponentInstance(const char * name);

    // Gets the component instance type of the instance *name as string via the component manager.
    const char * getComponentInstanceType(const char * name);

    // Create a component instance of given type with instance name "name" in the component manager.
    cSmileComponent * createComponent(const char *name, const char *component_type);
    
    // Gets the number of components ran during the last tick.
    long getLastNrun() { return lastNrun_; }

    // Functions to get config values from the config manager from our config instance.
    // The _f functions internally free the string *name. Use these in conjunction with myvprint()...
    // NOTE: Yes, this is ineffective. TODO: smile memory manager, and fixed length text buffer which can be reused (can also grow if needed).
    double getDouble(const char*name) {
      return confman_->getDouble_f(myvprint("%s.%s",cfname_,name));
    }
    double getDouble_f(char*name) {
      double d = getDouble(name);
      if (name!=NULL) free(name);
      return d;
    }

    int getInt(const char*name) {
      return confman_->getInt_f(myvprint("%s.%s",cfname_,name));
    }
    int getInt_f(char*name) {
      int d = getInt(name);
      if (name!=NULL) free(name);
      return d;
    }

    const char *getStr(const char*name){
      return confman_->getStr_f(myvprint("%s.%s",cfname_,name));
    }
    const char * getStr_f(char*name) {
      const char * s = getStr(name);
      if (name!=NULL) free(name);
      return s;
    }

    char getChar(const char*name) {
      return confman_->getChar_f(myvprint("%s.%s",cfname_,name));
    }
    const char getChar_f(char*name) {
      const char c = getChar(name);
      if (name!=NULL) free(name);
      return c;
    }

    const ConfigValue *getValue(const char*name) {
      return confman_->getValue_f(myvprint("%s.%s",cfname_,name));
    }
    const ConfigValue * getValue_f(char*name) {
      const ConfigValue * v = getValue(name);
      if (name!=NULL) free(name);
      return v;
    }

    const ConfigValueArr *getArray(const char*name) {
      return (ConfigValueArr *)(confman_->getValue_f(myvprint("%s.%s",cfname_,name)));
    }
    const ConfigValueArr * getArray_f(char*name) {
      const ConfigValueArr * a = getArray(name);
      if (name!=NULL) free(name);
      return a;
    }

    int getArraySize(const char*name) {
      return (confman_->getArraySize_f(myvprint("%s.%s",cfname_,name)));
    }
    int getArraySize_f(char*name) {
      int s = getArraySize(name);
      if (name!=NULL) free(name);
      return s;
    }

    int isSet(const char*name) {
      return (confman_->isSet_f(myvprint("%s.%s",cfname_,name)));
    }
    int isSet_f(char*name) {
      int s = isSet(name);
      if (name!=NULL) free(name);
      return s;
    }

    // Returns 1 if we are in an abort state (user requested abort).
    int isAbort();

    // Function that is called during initialisation. It should be used to fetch config variables
    // from the config manager and store them in local variables.
    // Each derived class is responsible for fetching configuration from confman or setting it manually BEFORE configureInstance is called!
    virtual void fetchConfig()=0;

    // Functions to be implemented by derived classes:

    // Sets the environment (pointers to parent, component manager, config manager).
    virtual void mySetEnvironment() { }
    
    // Registers the component instance after creation (by the component Manager).
    virtual int myRegisterInstance(int *runMe=NULL) { return 1; }

    // Configures the component instance. Data-memory levels and level parameters such as T are created and set here.
    virtual int myConfigureInstance() { return 1; }

    // Finalises the component instance. Data-memory level names (fields and elements) are set here.
    virtual int myFinaliseInstance() { return 1; }

    // Holds the actual implementation of the tick loop code for the derived component.
    // This function should return 1 if the component did process data, and 0 otherwise.
    // If all components return 0 (nothng to process), the component manager will switch
    // to the EOI (end-of-input) state (or advance to the next tick loop iteration).
    virtual int myTick(long long t) { return 0; }

    // Called by the component manager when the tick loop is to be paused.
    // If the component needs to reject the pausing of the tick loop, it should return 0, otherwise always 1!
    virtual int pauseEvent() { return 1; }

    // Called by the component manager when the tick loop is to be resumed.
    virtual void resumeEvent() { }

    // Signals EOI to componentManager (theoretically only useful for dataSource components, however we make it accessible to all smile components)
    // NOTE: you do not need to do this explicitely.. if all components fail, EOI is assumed, then a new tickLoop is started by the component manager
    // TODO: is this actually used and/or implemented properly?
    void signalEOI();

    // Checks if the given component message is of type "msgtype" (string name).
    int isMessageType(cComponentMessage *msg, const char * msgtype) {
      if (msg != NULL) {
        return (!strncmp(msg->msgtype, msgtype, CMSG_typenameLen));
      } else {
        return 0;
      }
    }

    // Functions for component message handling.
    // Note on data synchronisation for messages: The messages can arrive at any time, if
    // they are sent from another thread. Thus, a mutex is used to prevent collision when
    // accessing variables from processComponentMessage and the rest of the component code
    // (especially myTick). The mutex is automatically locked before processComponentMessage
    // is called, and unlocked afterwards. In the rest of the code, however, lockMessageMemory()
    // and unlockMessageMemory() must be used.

    // This function is called by the component manager, if there is a new message for this component.
    // A derived component must override this to receive and process messages.
    // Do NOT call lockMessageMemory() in this function! The *msg pointer is always valid.
    // Return value: 0, message was not processed; != 0, message was processed.
    // The return value will be passed on to the sender (?).
    // NOTE: the custData pointer in cComponentMessage (as well as the whole cComponentMessage object)
    //       is valid ONLY until processComponentMessage() returns!
    //       Thus, you are advised to copy data to local memory
    // NOTE2: This function is called directly from the context of another component/thread.
    //        Do NOT do anything here that is time consuming. It will block the other thread
    //        or the execution of the tick loop. Esp. don't wait here until your own myTick() is called.
    //        In signle threaded mode this will never happen!
    //        Always, only accept the message data and put it into a buffer. Process the buffer in myTick()
    //        or in a background thread.
    virtual int processComponentMessage(cComponentMessage *msg) { return 0; }
    
    // this function forwards the message to the componentMananger and sets the *sender pointer correctly
    // Return value: 0, message was not processed; != 0, message was processed.
    // TODO: return value passing for multiple recepients?
    int sendComponentMessage(const char *recepient, cComponentMessage *msg);

    // Locks the "message memory" (variables that are accessed by processComponentMessage).
    // You must call this function prior to accessing variables you are accessing in processComponentMessage().
    // Do not use this function in processComponentMessage() itself, though!
    void lockMessageMemory() { smileMutexLock(messageMtx_); }

    // Unlocks the "message memory" (variables that are accessed by processComponentMessage).
    void unlockMessageMemory() { smileMutexUnlock(messageMtx_); }
    
    // Gets the smile time from the component manager (time since start of the system).
    double getSmileTime();

    // Returns 1 if we are in an end-of-input condition.
    int isEOI() { return EOIcondition_; }

    // Get the EOI counter, i.e. the number of repeated tick loop sequences.
    int getEOIcounter() { return EOI_; }

    // Request stopping of processing in tickLoop of component manager.
    // Calling this will make openSMILE stop the processing.
    virtual void abort_processing();

  public:
    // Statics:
    // these two must be overridden in a base class:
    //static sComponentInfo * registerComponent(cConfigManager *_confman);
    //static cSmileComponent * create(const char *_instname);
    SMILECOMPONENT_STATIC_DECL

    //method for setting config... a base class may implement it with arbirtrary parameters
    // values set will override those obtained by fetchConfig if override=1
    // if override=0, then only those which are not set are updated
    // void setConfig(..., int override=1);
    int isManualConfigSet() { return manualConfig_; }
    
    // Returns whether the tick loop is paused (1) or running (0).
    int isPaused() { return paused_; }

    // Gets pointer to the associated component manager object,
    // i.e. the object that created this smile component instance.
    cComponentManager *getCompMan() { return compman_; }

    // Constructor that creates an instance of this component with instance name "instname".
    cSmileComponent(const char *instname);  
    
    // Sets component info after component creation, and calls fetchConfig.
    //  Pointer to config manager, component name and description
    virtual void setComponentInfo(cConfigManager *cm, const char *cname, const char *description) {
      confman_ = cm;
      cname_= cname;
      description_ = description;
      if (cm != NULL) fetchConfig();
    }
    
    // Sets component manager pointer and the component ID, as used by the component manager.
    // Called by the creator component (parent) or directly by the component manager.
    virtual void setComponentEnvironment(cComponentManager *compman, int id, cSmileComponent *parent=NULL);
    
    // Sets the name of the associated config instance this component instance is linked to.
    void setConfigInstanceName(const char *cfname) {
      if (cfname != NULL) {
        if ((cfname_ != NULL) && (cfname_ != iname_)) {
          free(cfname_);
          cfname_ = NULL;
        }
        cfname_ = strdup(cfname);
      }
    }

    // Gets the name of the associated config instance.
    const char * getConfigInstanceName() {
      return cfname_;
    }

    // Performs component specific register operation, e.g. write/read requests with dataMemory..
    // *runMe return value for component manager : 0, don't call my tick of this component, 1, call myTick
    int registerInstance(int *runMe=NULL) {
      if (runMe != NULL) *runMe = runMe_;
      if (isRegistered_) return 1;
      isRegistered_ = myRegisterInstance(runMe);
      if (runMe != NULL) runMe_ = *runMe;
      return isRegistered_;
    }
    int isRegistered() { return isRegistered_; }

    // Performs component configuration (data memory level configuration, creation of readers/writers etc.).
    int configureInstance() {
      if (isConfigured_) return 1;
      isConfigured_ = myConfigureInstance();
      return isConfigured_;
    }
    int isConfigured() { return isConfigured_; }

    // Performs component finalisation (loading of models, opening of files, data memory level names, ...).
    int finaliseInstance();
    int isFinalised() { return isFinalised_; }

    // Returns 1, if component has been finalised and is ready for the tick loop. Returns 0 otherwise.
    int isReady() { return isReady_; }

    // The tick() function. This is called by the component manager, and internally executes myTick(),
    int tick(long long t, int EOIcond=0, long _lastNrun=-1) {
      lastNrun_ = _lastNrun;
      if (EOIcond) {
        if (!isEOI()) {
          setEOI();
          return 1; // one "successful" tick to set up all components in EOI state
        }
      } else {
        unsetEOI();
      }
      if (isReady_) {
        if (doProfile_) {
          startProfile(t, EOIcondition_);
        }
        int ret = myTick(t);
        if (doProfile_) {
          endProfile(t, EOIcondition_);
        }
        return ret;
      }
      else return 0;
    }
    
    // Configures profiling (measuring of the time spent in each tick).
    void setProfiling(int enable=1, int print=0) {
      doProfile_ = enable;
      printProfile_ = print;
    }

    // Starts time measurement (called at the beginning of the tick).
    void startProfile(long long t, int EOI);

    // Ends time measurement (called at the end of the tick).
    void endProfile(long long t, int EOI);

    // Gets the current profiling statistics.
    // If sum == 1, it returns the accumulated run-time in seconds
    // (i.e. the time spent in tick() by this component).
    // If sum == 0, the duration of the last tick() is returned.
    // NOTE: For multi-threaded code this method of profiling is not exact.
    //       The tick() function can be interrupted by other threads, but
    //       time measurement is done via the system timer from start to end of tick().
    double getProfile(int sum=1) { 
      if (!sum) return profileCur_;
      else return profileSum_;
    }

    // Gets the component instance name.
    const char *getInstName() const {
      return iname_;
    }

    // Gets the component type name.
    const char *getTypeName() const {
      return cname_;
    }

    // Sets the EOI counter. Used by the component manager once a new tick loop with EOIcondition=1 starts.
    // NOTE: it would be possible for the components to manage their own EOI counters, however, like this we ensure sync and better performance.
    // If derived components create sub-components internally (without tick function - e.g. dataReaders/Writers),
    // they must override this, in order to properly set the EOI status in the sub-components.
    virtual int setEOIcounter(int cnt) {
      EOI_ = cnt;
      return cnt;
    }

    // Sets and unsets the EOIcondition variable. This is called internally from tick() only.
    // If derived components create sub-components internally (without tick function - e.g. dataReaders/Writers),
    // they must override this, in order to properly set the EOI status in the sub-components.
    virtual void setEOI() {
      EOIcondition_ = 1;
    }
    virtual void unsetEOI() {
      EOIcondition_ = 0;
    }

    // Called by the component manager. Notifies this component about a tick loop pause.
    virtual int pause() {
      if (!paused_) {
        paused_ = 1; 
        return pauseEvent();
      }
      return 1;
    }

    // Called by the component manager. Notifies this component about a tick loop resume.
    virtual void resume() { 
      if (paused_) {
        paused_ = 0; 
        resumeEvent();
      }
    }

    // This function is called externally by the component manager,
    //   if another component calls sendComponentMessage.
    // It receives the message, takes care of memory access synchronisation, and processes the message.
    int receiveComponentMessage(cComponentMessage *msg) {
      int ret = 0;
      if (msg != NULL)  {
        lockMessageMemory();
        ret = processComponentMessage(msg);
        unlockMessageMemory();
      }
      return ret;
    }

    virtual ~cSmileComponent();
};

#endif //__SMILE_COMPONENT_HPP
