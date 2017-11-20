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

#include <signal.h>
#include <stdlib.h>
#include <android/log.h>
#include <jni.h>

#include "smilextract.h"
#include "smile_jni.h"

extern "C" {

cComponentManager * myCmanGlob = NULL;
JavaVM *javaVm = NULL;

JavaVM * JNIcallback_getJavaVm() {
    return javaVm;
}

static jobject gClassLoader;
static jmethodID gFindClassMethod;

// http://stackoverflow.com/questions/14765776/jni-error-app-bug-accessed-stale-local-reference-0xbc00021-index-8-in-a-tabl

JNIEnv* getEnv() {
    JNIEnv *env;
    int status = javaVm->GetEnv((void**)&env, JNI_VERSION_1_6);
    if(status < 0) {
        status = javaVm->AttachCurrentThread(&env, NULL);
        if(status < 0) {
            return NULL;
        }
    }
    return env;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *pjvm, void *reserved) {
    javaVm = pjvm;
    JNIEnv * env = getEnv();
    jclass randomClass = env->FindClass("com/audeering/opensmile/androidtemplate/SmileJNI");
    jclass classClass = env->GetObjectClass(randomClass);
    jclass classLoaderClass = env->FindClass("java/lang/ClassLoader");
    jmethodID getClassLoaderMethod = env->GetMethodID(classClass, "getClassLoader",
                                                      "()Ljava/lang/ClassLoader;");
    jobject tmp1 = env->CallObjectMethod(randomClass, getClassLoaderMethod);
    gClassLoader = (jobject)env->NewGlobalRef(tmp1);
    gFindClassMethod = env->GetMethodID(classLoaderClass, "findClass",
                                        "(Ljava/lang/String;)Ljava/lang/Class;");
    return JNI_VERSION_1_6;
}

// Starts openSMILE through JNI.
JNIEXPORT jstring JNICALL Java_com_audeering_opensmile_androidtemplate_SmileJNI_SMILExtractJNI( JNIEnv* env, jobject thiz, jstring conf, jint updateProfile){
    const char SMILExtract[] = "SMILExtract";
    const char * configfile = env->GetStringUTFChars(conf, NULL);
    //"-nologfile" necessary, as we cannot write to the default logfile location!
    // -l <N> sets the debugging level. 5 is most verbose. 0 supresses all messages. 2 is recommended.
    // -O option is a dummy, no actual output file should be written to this location
    const char *cmd[] = {SMILExtract, "-C", configfile, "-nologfile", "-l", "2"};
    // TODO: sdcard location, if needed: Environment.getExternalStorageDirectory()
    int cmd_num = 6; // IMPORTANT: synchronize with the size of the cmd array
    // NOTE: app will seg-fault if cmd_num is larger than the actual number of arguments!

    __android_log_print(ANDROID_LOG_INFO, "opensmile", "starting opensmile...");
    // gets and stores global pointer to java vm
    env->GetJavaVM(&javaVm);
    // executes SMILExtract main function, modified for Android
    int ret = SMILExtractFunction(cmd_num, cmd, javaVm, &gClassLoader, &gFindClassMethod);
    __android_log_print(ANDROID_LOG_INFO, "opensmile", "stopped opensmile.");
    return env->NewStringUTF("openSMILE ran");
}

// Terminates openSMILE upon external/user request.
JNIEXPORT jstring JNICALL Java_com_audeering_opensmile_androidtemplate_SmileJNI_SMILEndJNI(JNIEnv* env, jobject thiz , jlong cmanglob){
    myCmanGlob = getComponentManager();
    if (myCmanGlob != NULL){
        myCmanGlob->requestAbort();
        return env->NewStringUTF("Finished openSMILE");
    } else {
        return env->NewStringUTF("component manager pointer was null");
    }
}


}
