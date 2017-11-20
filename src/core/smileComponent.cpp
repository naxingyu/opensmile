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
openSMILE component hierarchy:

cSmileComponent  (basic cmdline and config functionality)
 |
 v
cProcessorComponent  - contains: Reader, Writer, ref to data mem, one or more compute components, plus interconnection logic
cComputeComponent    - implements a specific algo, clearly defined io interface
                       I: vector, O: vector
                       I: matrix, O: vector
                       (O: matrix ?? )
cReaderComponent     - reads data from dataMemory and provides frame or matrix (also registers read request)
cWriterComponent     - writes data to dataMemory (takes vector, (or matrix??)), also registers Level (data memory at the end will check, if all dependencies are fullfilled..?)
*/


/*
   component base class,
   
   with register and config functionality
   
   
 //
   
 */

// enable this for debugging to enable profiling support for each component
#define DO_PROFILING    0
// enable this to output the profiling result after each tick via SMILE_IMSG(2,...)
#define PRINT_PROFILING 0

/* good practice for dynamic component library compatibility (will be added soon..):

  define a global register method:

  sComponentInfo * registerMe(cConfigManager *_confman) {
    sMyComponent::registerComponent(_confman);
  }

  also: add a pointer to this method to global component list...
*/

#include <core/smileComponent.hpp>
#include <core/componentManager.hpp>

#define MODULE "cSmileComponent"

SMILECOMPONENT_STATICS(cSmileComponent)

// static, must be overriden by derived component class, these are only examples...!!!!

// register component will return NULL, if the component could not be registered (fatal error..)
// it will return a valid struct, with rA=1, if some information, e.g. sub-configTypes are still missing
SMILECOMPONENT_REGCOMP(cSmileComponent)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_XXXX;
  sdescription = COMPONENT_DESCRIPTION_XXXX;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
  
  //ConfigType *ct = new ConfigType(scname); if (ct == NULL) OUT_OF_MEMORY;

  ct->setField("f1", "this is an example int", 0);
  if (ct->setField("subconf", "this is config of sub-component",
                  sconfman->getTypeObj("nameOfSubCompType"), NO_ARRAY, DONT_FREE) == -1) {
     rA=1; // if subtype not yet found, request , re-register in the next iteration
  }

  // ...
  SMILECOMPONENT_IFNOTREGAGAIN( {} )

    // change cSmileComponent to cMyComponent !
  SMILECOMPONENT_MAKEINFO(cSmileComponent);
  //return makeInfo(sconfman, scname, sdescription, cSmileComponent::create, rA);
}


SMILECOMPONENT_CREATE_ABSTRACT(cSmileComponent)

// staic, internal....
sComponentInfo * cSmileComponent::makeInfo(cConfigManager *_confman, const char *_name, const char *_description, cSmileComponent * (*create) (const char *_instname), int regAgain, int _abstract, int _nodmem)
{
  sComponentInfo *info = new sComponentInfo;
  if (info == NULL) OUT_OF_MEMORY;
  info->componentName = _name;
  info->description = _description;
  info->create = create;
  info->registerAgain = regAgain;
  info->abstract = _abstract;
  info->noDmem = _nodmem;
  info->next = NULL;
  
  return info;
}


//--------------------- dynamic:

cSmileComponent * cSmileComponent::getComponentInstance(const char * name) {
  if (compman_ != NULL)
    return compman_->getComponentInstance(name);
  else
    return NULL;
}

const char * cSmileComponent::getComponentInstanceType(const char * name) {
  if (compman_ != NULL)
    return compman_->getComponentInstanceType(name);
  else
    return NULL;
}

cSmileComponent * cSmileComponent::createComponent(const char *name, const char *component_type)
{
  if (compman_ != NULL) {
    return compman_->createComponent(name, component_type);
  } else {
    return NULL;
  }
}

cSmileComponent::cSmileComponent(const char *instname) :
  iname_(NULL),
  id_(-1),
  compman_(NULL),
  parent_(NULL),
  cfname_(NULL),
  isRegistered_(0),
  isConfigured_(0),
  isFinalised_(0),
  isReady_(0),
  confman_(NULL),
  override_(0),
  manualConfig_(0),
  cname_(NULL),
  EOIlevel_(0),
  EOI_(0),
  EOIcondition_(0),
  paused_(0),
  runMe_(1),
  doProfile_(DO_PROFILING),
  printProfile_(PRINT_PROFILING),
  profileCur_(0.0), profileSum_(0.0)
{
  smileMutexCreate(messageMtx_);
  if (instname == NULL) COMP_ERR("cannot create cSmileComponent with instanceName == NULL!");
  iname_ = strdup(instname);
  cfname_ = iname_;
}

void cSmileComponent::setComponentEnvironment(cComponentManager *compman, int id, cSmileComponent *parent)
{
  if (compman != NULL) {
    compman_ = compman;
    id_ = id;
  } else {
    SMILE_IERR(3, "setting NULL componentManager in cSmileComponent::setComponentEnvironment!");
  }
  parent_ = parent;
  mySetEnvironment();
}

int cSmileComponent::sendComponentMessage(const char *recepient, cComponentMessage *msg)
{
  int ret=0;
  if (compman_ != NULL) {
    if (msg != NULL) msg->sender = getInstName();
    ret = compman_->sendComponentMessage(recepient, msg);
  }
  return ret;
}

double cSmileComponent::getSmileTime() 
{ 
  if (compman_ != NULL) {
    return compman_->getSmileTime();
  }
  return 0.0;
}

int cSmileComponent::isAbort() 
{ 
  if (compman_ != NULL) return compman_->isAbort();
  else return 0; 
}

int cSmileComponent::finaliseInstance() 
{
  if (!isConfigured_) {
    SMILE_DBG(7,"finaliseInstance called on a not yet successfully configured instance '%s'",getInstName());
    return 0;
  }
  if (isFinalised_) return 1;
  isFinalised_ = myFinaliseInstance();
  isReady_ = isFinalised_;
  return isFinalised_;
}

void cSmileComponent::startProfile(long long t, int EOI)
{
  gettimeofday( &startTime_, NULL );
}

void cSmileComponent::endProfile(long long t, int EOI)
{
  gettimeofday( &endTime_, NULL );
  profileCur_ = ( (double)(endTime_.tv_sec - startTime_.tv_sec) + (double)(endTime_.tv_usec - startTime_.tv_usec)/1000000.0 );
  profileSum_ += profileCur_;
  if (printProfile_) {
    SMILE_IMSG(2, "~~~~profile~~~~ cur=%f  sum=%f  tick=%i\n", getProfile(0), getProfile(1), t);
  }

}

cSmileComponent::~cSmileComponent()
{
  if ((iname_ != cfname_)&&(cfname_!=NULL)) free (cfname_);
  if (iname_ != NULL) free(iname_);
  smileMutexDestroy(messageMtx_);
}

// signal EOI to componentManager (theoretically only useful for dataSource components, however we make it accessible to all smile components)
// NOTE: you do not need to do this explicitely.. if all components fail, EOI is assumed, then a new tickLoop is started by the component manager
void cSmileComponent::signalEOI()
{
  if (compman_ != NULL)
    compman_->setEOI();
}

void cSmileComponent::abort_processing() {
  compman_->requestAbort();
}




