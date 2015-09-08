
AC_INIT(openSMILE,2.0.0)
AC_CONFIG_SRCDIR([.])
AM_INIT_AUTOMAKE

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AC_PROG_CXX
# AC_PROG_RANLIB
LT_INIT
AM_PROG_CC_C_O

CHECK_HAVE_SEMAINEAPI
CHECK_HAVE_RTNNLLIB
#CHECK_HAVE_QTLIB
CHECK_HAVE_PORTAUDIO
#CHECK_HAVE_FFMPEG
CHECK_HAVE_ANDROID
#CHECK_HAVE_ATKLIB
CHECK_HAVE_OPENCV
ACX_PTHREAD
CONFIGURE_TARGETS

DLIB="-ldl"
if test x`uname | grep mingw` != x ; then
#  AC_DEFINE([WIN32],[1], [Compile on MinGW32-Msys])
  AC_DEFINE([__WINDOWS],[1], [Compile on MinGW32-Msys])
  AC_DEFINE([__MINGW32],[1], [Compile on MinGW32-Msys])
  AC_DEFINE([__MINGW],[1], [Compile on MinGW32-Msys])
  DLIB=""
fi 

if test x`uname | grep MINGW` != x ; then
#  AC_DEFINE([WIN32],[1], [Compile on MinGW32-Msys])
  AC_DEFINE([__WINDOWS],[1], [Compile on MinGW32-Msys])
  AC_DEFINE([__MINGW32],[1], [Compile on MinGW32-Msys])
  AC_DEFINE([__MINGW],[1], [Compile on MinGW32-Msys])
  AC_DEFINE([__HAVENT_GNULIBS],[1], [No getline implementation on Win])
  DLIB=""
fi

if test x`uname | grep Darwin` != x ; then
  AC_DEFINE([__MACOS],[1], [Compile on Macs])
  AC_DEFINE([__HAVENT_GNULIBS],[1], [No getline implementation on Mac])
fi
 
AC_SUBST(DLIB)

AC_DEFINE([HAVE_AUTOCONF],[1],[Build with autoconf])

AC_CONFIG_HEADERS(config.h)
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

