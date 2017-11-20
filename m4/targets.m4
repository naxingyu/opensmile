dnl m4 macro to configure smile build targets
dnl   written by Florian Eyben, 11/2008
dnl
dnl Configured the targets to be build based on the --enable/--disable options 
dnl Further, disables those targets that cannot be built due to missing dependencies
dnl
dnl IMPORTANT: call this macro AFTER the library checks!
dnl

AC_DEFUN([CONFIGURE_TARGETS],
[
  dnl the defaults:
  enable_liverec='yes'
  build_smilextract='yes'

  # program features::::

  AC_ARG_ENABLE([liverec],[  --enable-liverec  enable live input feature.  default=yes],
          enable_liverec="$enableval", enable_liverec="$enable_liverec")

  if test x$enable_liverec = xyes; then
    AC_DEFINE([WITH_LIVEREC],[1],[Support live input from soundcard/microphone])
  fi
 
  # build targets:::

  AC_ARG_ENABLE([SMILExtract],[  --enable-SMILExtract  build standalone commandline feature extractor.  default=yes],
           build_smilextract="$enableval", build_featum="$build_smilextract")

  AM_CONDITIONAL([BUILD_SMILExtract],[test x$build_smilextract = xyes])

])

