dnl  m4 macro to check for existance of OpenCV library
dnl      originally written by Florian Eyben, 11/2008
dnl	 amended by Florian Gross, 04/2013
dnl
dnl  usage:  CHECK_HAVE_OPENCV( [default path] )
dnl  if no default path is given, the default is /usr
dnl
dnl  The following defines are set, if the library is found:
dnl    #define HAVE_OPENCV 1 (OpenCV Version 2.2 or higher is required)
dnl 
dnl  The following Makefile substitutions are added:
dnl    OPENCV_CPPFLAGS  (for includes)
dnl    OPENCV_LDFLAGS   (for libraries)
dnl    OPENCV_ROOT      (points to the c++/ directory in the semaine folder)
dnl


AC_DEFUN([CHECK_HAVE_OPENCV],
[
   OPENCV='no'
   OPENCV_ROOT=''
   OPENCV_CPPFLAGS=''
   OPENCV_LDFLAGS=''
   have_opencv='no'

   if test "x$1" = "x" ; then
     DEFAULT='no'
   else
     DEFAULT=$1
   fi

   dnl Define the root directory for the OpenCV installation.
   AC_ARG_WITH(opencv,
               [  --with-opencv=<PATH> OpenCV installation dir. default=$DEFAULT],
               OPENCV_ROOT="$withval", OPENCV_ROOT=$DEFAULT)

   if test "x$OPENCV_ROOT" != "xno" ; then
      dnl Save these values in case they need to be restored later.
      save_CFLAGS="$CFLAGS"
      save_CPPFLAGS="$CPPFLAGS"
      save_LDFLAGS="$LDFLAGS"

      dnl Add the user-specified OpenCV installation directory to these
      dnl paths.  Ensure that /usr/include and /usr/lib are not included
      dnl multiple times if $OPENCV_ROOT is "/usr".
      if test "x$OPENCV_ROOT" != "x/usr" ; then
         CPPFLAGS="$CPPFLAGS -I$OPENCV_ROOT/include"
         #CFLAGS="$CFLAGS -I$OPENCV_ROOT/include"
         cv_ldflags="-L$OPENCV_ROOT/lib"
         LDFLAGS="$cv_ldflags $LDFLAGS"
      fi

      AC_LANG_PUSH([C++])
      save_LIBS="$LIBS"
      AC_SEARCH_LIBS([cvNorm], [opencv_core opencv_imgproc opencv_ml opencv_video opencv_features2d], [AC_CHECK_HEADER([opencv2/video/video.hpp], [have_opencv='yes'])])
      LIBS="$save_LIBS"
      AC_LANG_POP()


      dnl If OpenCV API files were found, define this extra stuff that may
      dnl be helpful in some Makefiles.
      if test "x$have_opencv" = "xyes" ; then
dnl   NOTE: pkg-config returns the wrong commandline in some cases, so we disable it!
dnl         export PKG_CONFIG_PATH="$OPENCV_ROOT/lib/pkgconfig"
dnl	 OPENCV_LDFLAGS=`pkg-config --silence-errors --libs opencv`
dnl	 OPENCV_CPPFLAGS=`pkg-config --silence-errors --cflags opencv`
dnl echo "OPCVCPP: $OPENCV_CPPFLAGS";
dnl OPENCV_CPPFLAGS="-I$OPENCV_ROOT/include"
dnl         unset PKG_CONFIG_PATH
	 if test "x$OPENCV_LDFLAGS" = "x" ; then
           OPENCV_LDFLAGS="-lopencv_core -lopencv_imgproc -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_objdetect -lopencv_highgui";
           if test "x$OPENCV_ROOT" != "x/usr" ; then
             OPENCV_LDFLAGS="-L$OPENCV_ROOT/lib $OPENCV_LDFLAGS"
             OPENCV_CPPFLAGS="-I$OPENCV_ROOT/include"
           fi
         fi
         AC_DEFINE([HAVE_OPENCV],1,[Compile with OpenCV support])
         OPENCV='yes'
      else
         AC_MSG_WARN([Compiling WITHOUT OpenCV support! You won't be able to extract video features.])
      fi

      dnl Restore all the variables now that we are done testing.
      CFLAGS="$save_CFLAGS"
      CPPFLAGS="$save_CPPFLAGS"
      LDFLAGS="$save_LDFLAGS"
   fi

   dnl Export all of the output vars for use by makefiles and configure script.
   AC_SUBST(OPENCV_ROOT)
   AC_SUBST(OPENCV_CPPFLAGS)
   AC_SUBST(OPENCV_LDFLAGS)
])
