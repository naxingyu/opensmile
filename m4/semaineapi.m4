dnl  m4 macro to check for existance of SemaineAPI build tree
dnl      written by Florian Eyben, 11/2008
dnl
dnl  usage:  CHECK_HAVE_SEMAINEAPI( [default path] )
dnl  if no default path is given, the default is ../../
dnl
dnl  The following defines are set, if the library is found:
dnl    #define HAVE_SEMAINEAPI 1
dnl 
dnl  The following Makefile substitutions are added:
dnl    SEMAINEAPI_CPPFLAGS  (for includes)
dnl    SEMAINEAPI_LDFLAGS   (for libraries)
dnl    SEMAINEAPI_ROOT      (points to the c++/ directory in the semaine folder)
dnl

AC_DEFUN([CHECK_HAVE_SEMAINEAPI],
[
   dnl initialize returned data...
   SEMAINEAPI='no'
   SEMAINEAPI_ROOT=''
   SEMAINEAPI_CPPFLAGS=''
   SEMAINEAPI_LDFLAGS=''
   have_semaineapi='no'

   if test "x$1" = "x" ; then
     mypwd=`pwd`
     cd ../../
     DEFAULT=`pwd`
     cd $mypwd
   else
     dnl get absolute path:
     mypwd=`pwd`
     cd $1
     DEFAULT=`pwd`
     cd $mypwd
   fi

   AC_ARG_WITH(semaineapi,
               [  --with-semaineapi=<PATH> SemaineApi build dir (c++/src). default=../../],
               SEMAINEAPI_ROOT="$withval", SEMAINEAPI_ROOT=$DEFAULT)

   if test "x$SEMAINEAPI_ROOT" != "xno" ; then
      dnl Save these values in case they need to be restored later.
      save_CFLAGS="$CFLAGS"
      save_CPPFLAGS="$CPPFLAGS"
      save_LDFLAGS="$LDFLAGS"

      AC_MSG_CHECKING(for libsemaineapi);
      if test -f "$SEMAINEAPI_ROOT/../libsemaineapi.config" ; then 
        . $SEMAINEAPI_ROOT/../libsemaineapi.config
	have_semaineapi='yes'
      fi

      if test "x$have_semaineapi" = "xyes" ; then
         SEMAINEAPI_LDFLAGS="$SEMAINE_LDFLAGS"
         if test "x$SEMAINE_CPPFLAGS" != "x" ; then
           SEMAINEAPI_CPPFLAGS="$SEMAINE_CPPFLAGS"
         else
           SEMAINEAPI_CPPFLAGS="$SEMAINE_CXXFLAGS"
         fi
         AC_DEFINE([HAVE_SEMAINEAPI],1,[Indicates whether Semaine API library is present])
         AC_MSG_RESULT(yes)
         SEMAINEAPI='yes'
      else 
         AC_MSG_RESULT(no)
	 AC_MSG_WARN(LibSemaineAPI not found!! Configuring only standalone feature extractor)
      fi

      dnl Restore all the variables now that we are done testing.
      CFLAGS="$save_CFLAGS"
      CPPFLAGS="$save_CPPFLAGS"
      LDFLAGS="$save_LDFLAGS"
   fi

   dnl Export all of the output variables for use by makefiles and configure script.
   AC_SUBST([SEMAINEAPI_ROOT])
   AC_SUBST([SEMAINEAPI_CPPFLAGS])
   AC_SUBST([SEMAINEAPI_LDFLAGS])
])
