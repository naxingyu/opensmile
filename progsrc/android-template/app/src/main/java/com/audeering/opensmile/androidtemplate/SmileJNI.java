/*
 Copyright (c) 2015 audEERING UG. All rights reserved.

 Date: 17.08.2015
 Author(s): Florian Eyben
 E-mail:  fe@audeering.com

 This is the interface between the Android app and the openSMILE binary.
 openSMILE is called via SMILExtractJNI()
 Messages from openSMILE are received by implementing the SmileJNI.Listener interface.
*/

package com.audeering.opensmile.androidtemplate;

public class SmileJNI {

    /**
     * load the JNI interface
     */
    static {
        System.loadLibrary("smile_jni");
    }

    /**
     * method to execute openSMILE binary from the Android app activity, see smile_jni.cpp.
     * @param configfile
     * @param externalStoragePath
     * @param updateProfile
     * @return
     */
    public static native String SMILExtractJNI(String configfile, int updateProfile);
    public static native String SMILEndJNI();

    /**
     * process the messages from openSMILE (redirect to app activity etc.)
     */
    public interface Listener {
        void onSmileMessageReceived(String text);
    }
    private static Listener listener_;
    public static void registerListener (Listener listener) {
        listener_ = listener;
    }

    /**
     * this is the first method called by openSMILE binary. it redirects the call to the Android
     * app activity.
     * @param text JSON encoded string
     */
    static void receiveText(String text) {
        if (listener_ != null)
            listener_.onSmileMessageReceived(text);
    }
}
