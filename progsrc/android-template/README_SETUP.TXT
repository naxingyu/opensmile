
After checking out from SVN, the following steps might be necessary:

1. edit app/src/main/jni/sync-build-libopensmile.sh
   and adapt OPENSMILE_DIR to point to your trunk of opensmile
2. if you have not yet run buildAndroid.sh in the opensmile trunk,
   do so now to fully build the android libs
3. To sync the libs with the app project, do:
     cd app/src/main/jni
     sh sync-build-libopensmile.sh
4. open project in android studio
5. Change 
    sdk.dir=
    ndk.dir=
   in GradleScripts -> local.properties
   to your SDK and NDK locations.
   (take a NDK version _NOT_ higher than ndk-r10e-rc4)
6. Edit the OPENSMILE path in 
     app/src/main/jni/Android.mk
7. Edit the ndk-build command (and path) in 
     app/build.gradle
   or better: add the NDK directory to your system path
   and then use 'ndk-build' as command

8. Copy openSMILE configs and models to app/src/assets.
   Load these by specifying them in app/src/plugins/Config.scala.

9. For a new GUI application, create a new SmilePlugin within app/src/scala/.../plugins
   See the existing example and config code and respective documentation there.

10. If you want to do something different than just showing some live openSMILE output,
    take a look at the following classes and respective methods:
    - src/scala/.../MainActivity.scala -> SmileThread & onSmileMessageReceived()
    - src/java/.../SmileJNI.java -> interface Listener & registerListener()