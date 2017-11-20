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



#include <android/jniMessageInterface.hpp>
#ifdef __ANDROID__
#ifdef BUILD_COMPONENT_JniMessageInterface

#define MODULE "cJniMessageInterface"

SMILECOMPONENT_STATICS(cJniMessageInterface)

SMILECOMPONENT_REGCOMP(cJniMessageInterface)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CJNIMESSAGEINTERFACE;
  sdescription = COMPONENT_DESCRIPTION_CJNIMESSAGEINTERFACE;
  SMILECOMPONENT_CREATE_CONFIGTYPE
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("sendMessagesInTick", "1/0 enable/disable sending of messages synchronized in tick(). If set to 0, a background thread will be started which will send messages from the queue in the background (NOT YET IMPLEMENTED).", 1);
    ct->setField("JNIcallbackClass", "Fully qualified Java name of class in APP which handles callbacks. Must be changed to the app namespace and domain.", "java/com/audeering/testapp01/SmileJNIcallbacks");
    ct->setField("JNIstringReceiverMethod", "Name of method which receives string messages in JNIcallbackClass. Default should not need to be changed, if class wasn't changed.", "receiveText");
    ct->setField("debugPrintJson", "1 = debug print to smile log the formatted json before sending.", 0);
  )
  SMILECOMPONENT_MAKEINFO(cJniMessageInterface);
}


SMILECOMPONENT_CREATE(cJniMessageInterface)

//-----
cJniMessageInterface::cJniMessageInterface(const char *_name):
  cSmileComponent(_name), jvm_(NULL), sendMessagesInTick_(1),
  env_(NULL) { }

void cJniMessageInterface::fetchConfig() {
  sendMessagesInTick_ = getInt("sendMessagesInTick");
  JNIstringReceiverMethod_ = getStr("JNIstringReceiverMethod");
  JNIcallbackClass_ = getStr("JNIcallbackClass");
  jvm_ = (JavaVM *)getExternalPointer("JavaVM");
  gFindClassMethod_ = (jmethodID *)getExternalPointer("FindClassMethod");
  gClassLoader_ = (jobject *)getExternalPointer("ClassLoader");
  debugPrintJson_ = getInt("debugPrintJson");
}

int cJniMessageInterface::myFinaliseInstance() {
  if (!sendMessagesInTick_) {
    // TODO: create background thread!
    SMILE_IERR(1, "background message thread not yet supported! Message sending through JNI disabled.");
  }
  return 1;
}

// JNI calls for exchange of smile messages

JNIEnv * cJniMessageInterface::AttachToThreadAndGetEnv() {
 JNIEnv * env = NULL;
 if (jvm_ == NULL) {
   SMILE_IERR(2, "no JVM pointer! Cannot access Java VM to get environment and exchange JNI data.");
   return env;
 }
 // double check it's all ok
 int getEnvStat = jvm_->GetEnv((void **)&env, JNI_VERSION_1_6);
 if (getEnvStat == JNI_EDETACHED) {
   SMILE_IMSG(4, "GetEnv: not attached");
   if (jvm_->AttachCurrentThread((JNIEnv **)&env, NULL) != 0) {
     SMILE_IERR(2, "GetEnv: failed to attach to current thread!");
   }
 } else if (getEnvStat == JNI_OK) {
   //
 } else if (getEnvStat == JNI_EVERSION) {
   SMILE_IERR(2, "GetEnv: version not supported");
 }
 return env;
}

void cJniMessageInterface::DetachFromThread(JNIEnv * env) {
  if (env == NULL || jvm_ == NULL)
   return;
  if (env->ExceptionCheck()) {
    env->ExceptionDescribe();
  }
  jvm_->DetachCurrentThread();
}

/*
 *
 * JNIEnv * env = getEnv();
    //replace with one of your classes in the line below
    jclass randomClass = env->FindClass("com/audeering/testapp01/SmileJNI");
    jclass classClass = env->GetObjectClass(randomClass);
    jclass classLoaderClass = env->FindClass("java/lang/ClassLoader");
    jmethodID getClassLoaderMethod = env->GetMethodID(classClass, "getClassLoader",
                                             "()Ljava/lang/ClassLoader;");
    gClassLoader = env->CallObjectMethod(randomClass, getClassLoaderMethod);
    gFindClassMethod = env->GetMethodID(classLoaderClass, "findClass",
                                    "(Ljava/lang/String;)Ljava/lang/Class;");
    return JNI_VERSION_1_6;
}

 *
 */

/*
jclass cJniMessageInterface::findClass(JNIEnv *env, const char* name) {
    return static_cast<jclass>(env->CallObjectMethod(gClassLoader, gFindClassMethod, getEnv()->NewStringUTF(name)));
}*/

int cJniMessageInterface::sendTextToJava(JNIEnv *env, const char * str) {
  if (str == NULL || env == NULL)
    return 0;
  SMILE_IDBG(2, "resolving:0 JNIcallbackClass_ %s", JNIcallbackClass_);
  jclass app = static_cast<jclass>(env->CallObjectMethod(*gClassLoader_, *gFindClassMethod_, JNIcallbackClassJstring_));
//  jclass app = env->FindClass(JNIcallbackClass_); //"com/audeering/testapp01/SmileJNIcallbacks");
  jmethodID sendDataToJava = env->GetStaticMethodID(app, JNIstringReceiverMethod_, "(Ljava/lang/String;)V");  // "([B)V"
  jstring jstr = env->NewStringUTF(str);
//  jbyteArray bArray = env->NewByteArray(len);
//  char *bytes = (char *)env->GetByteArrayElements(bArray, 0);
//  memcpy( bytes, data, len);
  //env->ReleaseByteArrayElements(bArray, bytes, JNI_ABORT);
  //env->CallStaticVoidMethod(app, sendDataToJava, bArray);
  env->CallStaticVoidMethod(app, sendDataToJava, jstr);
  env->DeleteLocalRef(jstr);
  env->DeleteLocalRef(app);
  return 1;
}

int cJniMessageInterface::sendMessageToJava(cComponentMessage &msg, JNIEnv *env) {
  char * json = msg.serializeToJson();
  if (debugPrintJson_) {
    SMILE_IMSG(1, "JSON message:\n %s\n", json);
  }
  return sendTextToJava(env, json);
}

int cJniMessageInterface::sendMessagesFromQueue() {
  // Sends out all messages in the queue, and remove from queue.
  int ret = 0;
  for (std::list<cComponentMessage>::iterator it = in_buffer_.begin(); it != in_buffer_.end(); ) {
    if (env_ != NULL) {
      sendMessageToJava(*it, env_);
      ret = 1;
    }
    if (it->custData != NULL)
      free(it->custData);
    if (it->custData2 != NULL)
      free(it->custData2);
    it = in_buffer_.erase(it);
  }
  return ret;
}

int cJniMessageInterface::myTick(long long t) {
  if (isEOI()) {
    SMILE_IMSG(2, "detaching from thread due to EOI");
    if (env_ != NULL) {
      DetachFromThread(env_);
      env_ = NULL;
    }
  } else {
    if (env_ == NULL) {
      SMILE_IMSG(2, "attaching to thread (env is NULL)");
      env_ = AttachToThreadAndGetEnv();
      if (env_ != NULL) {
        JNIcallbackClassJstring_ = env_->NewStringUTF(JNIcallbackClass_);
      }
    }
    if (sendMessagesInTick_) {
      return sendMessagesFromQueue();
    }
  }
  return 0;
}

int cJniMessageInterface::processComponentMessage(cComponentMessage *msg) {
  // We copy the component message, as all memory referenced in the
  // message will be invalid after we call "return" here. 
  cComponentMessage c;
  memcpy(&c, msg, sizeof(cComponentMessage));
  if (msg->custData != NULL) {
    if (c.custDataType == CUSTDATA_TEXT) {
      c.custData = malloc(sizeof(char) * (strlen((char*)msg->custData) + 1));
      strcpy((char *)c.custData, (char *)msg->custData);
    } else {
      c.custData = malloc(msg->custDataSize + 1);
      memcpy(c.custData, msg->custData, msg->custDataSize);
    }
  }
  if (msg->custData2 != NULL) {
    if (c.custDataType == CUSTDATA_TEXT) {
      c.custData2 = malloc(sizeof(char) * (strlen((char*)msg->custData2) + 1));
      strcpy((char *)c.custData2, (char *)msg->custData2);
    } else {
      c.custData2 = malloc(msg->custData2Size + 1);
      memcpy(c.custData2, msg->custData2, msg->custData2Size);
    }
  }
  // Now we save the message in the FIFO queue.
  in_buffer_.push_back(c);
  return 1;
}

cJniMessageInterface::~cJniMessageInterface() {
  // Free the message queue:
  for (std::list<cComponentMessage>::iterator it = in_buffer_.begin(); it != in_buffer_.end(); ) {
    if (it->custData != NULL)
      free(it->custData);
    if (it->custData2 != NULL)
      free(it->custData2);
    it = in_buffer_.erase(it);
  }
//  JNIEnv *env = AttachToThreadAndGetEnv();
  if (env_ != NULL) {
    //env_->DeleteLocalRef(JNIcallbackClassJstring_);
    //DetachFromThread(env_);
  }
}

#endif  // BUILD_COMPONENT
#endif  // __ANDROID__

