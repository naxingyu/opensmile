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


#ifndef __CJNIMESSAGEINTERFACE_HPP
#define __CJNIMESSAGEINTERFACE_HPP

#include <core/smileCommon.hpp>
#ifdef __ANDROID__
#define BUILD_COMPONENT_JniMessageInterface
#ifdef BUILD_COMPONENT_JniMessageInterface
#include <list>
#include <jni.h>
#include <core/smileComponent.hpp>

#define COMPONENT_DESCRIPTION_CJNIMESSAGEINTERFACE "Component for transferring smile component messages from native C to Java via JNI interface.\n"
#define COMPONENT_NAME_CJNIMESSAGEINTERFACE "cJniMessageInterface"

#undef class
class DLLEXPORT cJniMessageInterface : public cSmileComponent {
private:

protected:
  SMILECOMPONENT_STATIC_DECL_PR

  int debugPrintJson_;
  int sendMessagesInTick_;
  jstring JNIcallbackClassJstring_;
  JNIEnv *env_;
  JavaVM *jvm_;
  jobject * gClassLoader_;
  jmethodID * gFindClassMethod_;
  const char * JNIcallbackClass_;
  const char * JNIstringReceiverMethod_;
  std::list<cComponentMessage> in_buffer_;

  virtual void fetchConfig();

  JNIEnv * AttachToThreadAndGetEnv();
  void DetachFromThread(JNIEnv * env);
  int sendTextToJava(JNIEnv * env, const char * str);
  int sendMessageToJava(cComponentMessage &msg, JNIEnv * env);

  virtual int sendMessagesFromQueue();

public:
  SMILECOMPONENT_STATIC_DECL

  cJniMessageInterface(const char *_name);
  void setJavaVmPointer(JavaVM * jvm) {
    jvm_ = jvm;
  }
  virtual int processComponentMessage(cComponentMessage *_msg);
  virtual int myFinaliseInstance();
  virtual int myTick(long long t);
  virtual ~cJniMessageInterface();
};

#endif  // BUILD_COMPONENT
#endif  // __ANDROID__
#endif  // __CJNIMESSAGEINTERFACE_HPP
