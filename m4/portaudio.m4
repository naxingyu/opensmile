dnl  m4 macro to check for existance of PortAudio library
dnl      written by Florian Eyben, 11/2008
dnl
dnl  usage:  CHECK_HAVE_PORTAUDIO( [default path] )
dnl  if no default path is given, the default is /usr
dnl
dnl  The following defines are set, if the library is found:
dnl    #define HAVE_PORTAUDIO 1
dnl    #define HAVE_PORTAUDIO_V19 1    (only if installed portaudio has newer V19 API)
dnl 
dnl  The following Makefile substitutions are added:
dnl    PORTAUDIO_CPPFLAGS  (for includes)
dnl    PORTAUDIO_LDFLAGS   (for libraries)
dnl    PORTAUDIO_ROOT      (points to the c++/ directory in the semaine folder)
dnl


AC_DEFUN([CHECK_HAVE_PORTAUDIO],
[
   PORTAUDIO='no'
   PORTAUDIO_V19='no'
   PORTAUDIO_ROOT=''
   PORTAUDIO_CPPFLAGS=''
   PORTAUDIO_LDFLAGS=''
   have_portaudio='no'
   have_portaudio_v19='no'

   if test "x$1" = "x" ; then
     DEFAULT='/usr'
   else
     DEFAULT=$1
   fi

   dnl Define the root directory for the PortAudio installation.
   AC_ARG_WITH(portaudio,
               [  --with-portaudio=<PATH> PortAudio installation dir. default=$1],
               PORTAUDIO_ROOT="$withval", PORTAUDIO_ROOT=$DEFAULT)

   if test "x$PORTAUDIO_ROOT" != "xno" ; then
      dnl Save these values in case they need to be restored later.
      save_CFLAGS="$CFLAGS"
      save_CPPFLAGS="$CPPFLAGS"
      save_LDFLAGS="$LDFLAGS"

      dnl Add the user-specified PortAudio installation directory to these
      dnl paths.  Ensure that /usr/include and /usr/lib are not included
      dnl multiple times if $PORTAUDIO_ROOT is "/usr".
      if test "x$PORTAUDIO_ROOT" != "x/usr" ; then
         CPPFLAGS="$CPPFLAGS -I$PORTAUDIO_ROOT/include"
         #CFLAGS="$CFLAGS -I$PORTAUDIO_ROOT/include"

         if test -d "$PORTAUDIO_ROOT/lib" ; then
            pa_ldflags="-L$PORTAUDIO_ROOT/lib"
         else
            pa_ldflags="-L$PORTAUDIO_ROOT/lib"
         fi

         LDFLAGS="$pa_ldflags $LDFLAGS"
      fi


      save_LIBS="$LIBS"
      AC_CHECK_LIB([portaudio], [Pa_Initialize],
            [AC_CHECK_HEADER([portaudio.h], [have_portaudio='yes'])])
      AC_CHECK_LIB([portaudio], [Pa_GetHostApiCount], [have_portaudio_v19='yes'])
      LIBS="$save_LIBS"

      dnl test if portaudio is latest version... i.e. has Pa_streamStatus
      dnl TODO....

      dnl If PortAudio API files were found, define this extra stuff that may
      dnl be helpful in some Makefiles.
      if test "x$have_portaudio" = "xyes" ; then
         export PKG_CONFIG_PATH="$PORTAUDIO_ROOT:$PORTAUDIO_ROOT/include:$PORTAUDIO_ROOT/build"
	 PORTAUDIO_LDFLAGS=`pkg-config --silence-errors --libs portaudio-2.0`
	 PORTAUDIO_CPPFLAGS=`pkg-config --silence-errors --cflags portaudio-2.0`
#echo "PACPP: $PORTAUDIO_CPPFLAGS";
PORTAUDIO_CPPFLAGS="-I$PORTAUDIO_ROOT/include"
         unset PKG_CONFIG_PATH
	 if test "x$PORTAUDIO_LDFLAGS" = "x" ; then
           PORTAUDIO_LDFLAGS="-lportaudio";
           if test "x$PORTAUDIO_ROOT" != "x/usr" ; then
             PORTAUDIO_LDFLAGS="$PORTAUDIO_LDFLAGS -L$PORTAUDIO_ROOT/lib"
             PORTAUDIO_CPPFLAGS="-I$PORTAUDIO_ROOT/include"
           fi
         fi
         AC_DEFINE([HAVE_PORTAUDIO],1,[Compile with portaudio support])
         if test "x$have_portaudio_v19" = "xyes" ; then
           AC_DEFINE([HAVE_PORTAUDIO_V19],1,[Detected portaudio has newer V19 api])
           PORTAUDIO_V19='yes'
         fi
         PORTAUDIO='yes'
      else
         AC_MSG_WARN([Compiling WITHOUT PortAudio support!! Sound recording will probably be broken!])
      fi

      dnl Restore all the variables now that we are done testing.
      CFLAGS="$save_CFLAGS"
      CPPFLAGS="$save_CPPFLAGS"
      LDFLAGS="$save_LDFLAGS"
   fi

   dnl Export all of the output vars for use by makefiles and configure script.
   AC_SUBST(PORTAUDIO_ROOT)
   AC_SUBST(PORTAUDIO_CPPFLAGS)
   AC_SUBST(PORTAUDIO_LDFLAGS)
])
