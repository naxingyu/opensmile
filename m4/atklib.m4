dnl  m4 macro to check for existance of ATKLib
dnl      written by Florian Eyben, 11/2008
dnl
dnl  usage:  CHECK_HAVE_ATKLIB( [default path] )
dnl  if no default path is given, the default is ../../../../thirdparty/build/atk160
dnl
dnl  The following defines are set, if the library is found:
dnl    #define HAVE_ATKLIB 1
dnl 
dnl  The following Makefile substitutions are added:
dnl    ATKLIB_CPPFLAGS  (for includes)
dnl    ATKLIB_LDFLAGS   (for libraries)
dnl    ATKLIB_ROOT      (points to the atk160 directory)
dnl

AC_DEFUN([CHECK_HAVE_ATKLIB],
[
   dnl initialize returned data...
   ATKLIB='no'
   ATKLIB_ROOT=''
   ATKLIB_CPPFLAGS=''
   ATKLIB_LDFLAGS=''
   have_atklib='no'

   DEFAULT_ROOT='../../../../thirdparty/build/atk160'
   if test "x$1" = "x" ; then
     mypwd=`pwd`
     if test -d $DEFAULT_ROOT ; then
       cd $DEFAULT_ROOT
       DEFAULT=`pwd`
       cd $mypwd
     else
       DEFAULT=''
     fi
   else
     dnl get absolute path:
     mypwd=`pwd`
     cd $1
     DEFAULT=`pwd`
     cd $mypwd
   fi

   AC_ARG_WITH(atklib,
               [  --with-atklib=<PATH> ATKLib top-level build directory. default=$DEFAULT_ROOT],
               ATKLIB_ROOT="$withval", ATKLIB_ROOT=$DEFAULT)

   if test "x$ATKLIB_ROOT" != "xno" ; then
      dnl Save these values in case they need to be restored later.
      save_CFLAGS="$CFLAGS"
      save_CPPFLAGS="$CPPFLAGS"
      save_LDFLAGS="$LDFLAGS"

      AC_MSG_CHECKING(if atklib is present);
      if test -f "$ATKLIB_ROOT/ATKLib/AHTK.h" ; then 
        if test -f "$ATKLIB_ROOT/HTKLib/HModel.h" ; then 
          if test -f "$ATKLIB_ROOT/ATKLib/AHTK.o" ; then
	    have_atklib='yes'
          else
            AC_MSG_ERROR(ATKLib found in $ATKLIB_ROOT, but it seems it was not compiled!)
          fi
        fi
      fi

      if test "x$have_atklib" = "xyes" ; then
         ATKLIB_LDFLAGS="$ATKLIB_ROOT/ATKLib/*.o $ATKLIB_ROOT/HTKLib/*.o -lX11 -lpthread -L/usr/X11R6/lib "
dnl         ATKLIB_LDFLAGS="$ATKLIB_ROOT/ATKLib/ATKLib.$CPU.a $ATKLIB_ROOT/HTKLib/HTKLib.$CPU.a -lX11 -lpthread -L/usr/X11R6/lib "
         ATKLIB_CPPFLAGS="-I$ATKLIB_ROOT/ATKLib -I$ATKLIB_ROOT/HTKLib"

         AC_DEFINE([HAVE_ATKLIB],1,[Indicates whether ATKLib and HTKLib is present])
         AC_MSG_RESULT(yes)
         ATKLIB='yes'
      else 
         AC_MSG_RESULT(no)
      fi

dnl      AC_MSG_CHECKING(if libx11-dev is present [required by atklib]);
dnl      if test ! -f /usr/include/X11/Xlib.h ; then
dnl        AC_MSG_RESULT(no)
dnl        AC_MSG_ERROR(X11 headers not found in /usr/include/X11 !  Install the libx11-dev package [ Debian/Ubuntu: as root: apt-get install libx11-dev ] )
dnl      else
dnl        AC_MSG_RESULT(yes)
dnl      fi

dnl      AC_MSG_CHECKING(if libasound2-dev is present [required by atklib]);
dnl      if test ! -f /usr/include/sound/asound.h ; then
dnl        AC_MSG_RESULT(no)
dnl        AC_MSG_ERROR(Alsasound headers not found in /usr/include/sound !  Install the libasound2-dev package [ Debian/Ubuntu: as root: apt-get install libasound2-dev ] )
dnl      else
dnl        AC_MSG_RESULT(yes)
dnl      fi

      dnl Restore all the variables now that we are done testing.
      CFLAGS="$save_CFLAGS"
      CPPFLAGS="$save_CPPFLAGS"
      LDFLAGS="$save_LDFLAGS"
   else
       AC_MSG_ERROR(ATKLib/HTKLib not found! Cannot build without ATK!)
   fi

   dnl Export all of the output variables for use by makefiles and configure script.
   AC_SUBST([ATKLIB_ROOT])
   AC_SUBST([ATKLIB_CPPFLAGS])
   AC_SUBST([ATKLIB_LDFLAGS])
])
