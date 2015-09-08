
AC_INIT([.],1)
AM_INIT_AUTOMAKE(testplugin,0.0.0)


AC_PROG_CC
AC_PROG_CXX
# AC_PROG_RANLIB
AC_PROG_LIBTOOL
AM_PROG_CC_C_O

ACX_PTHREAD

#CHECK_HAVE_SEMAINEAPI
CHECK_HAVE_OPENSMILELIB

# CONFIGURE_TARGETS

if test x`uname | grep mingw` != x ; then
  AC_DEFINE([WIN32],[1], [Compile on MinGW32-Msys])
  AC_DEFINE([__WINDOWS],[1], [Compile on MinGW32-Msys])
  AC_DEFINE([__MINGW32],[1], [Compile on MinGW32-Msys])
  AC_DEFINE([__MINGW],[1], [Compile on MinGW32-Msys])
fi 

if test x`uname | grep MINGW` != x ; then
  AC_DEFINE([WIN32],[1], [Compile on MinGW32-Msys])
  AC_DEFINE([__WINDOWS],[1], [Compile on MinGW32-Msys])
  AC_DEFINE([__MINGW32],[1], [Compile on MinGW32-Msys])
  AC_DEFINE([__MINGW],[1], [Compile on MinGW32-Msys])
fi 


AM_CONFIG_HEADER(config.h)
AC_OUTPUT(Makefile)

