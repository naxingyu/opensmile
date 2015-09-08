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
  build_semainextract='yes'
  build_smilerec='no'
  build_smilesync='no'
  build_audiorecorder='no'
  build_vinylsplit='no'

  # program features::::

  AC_ARG_ENABLE([liverec],[  --enable-liverec  enable live input feature.  default=yes],
          enable_liverec="$enableval", enable_liverec="$enable_liverec")

  if test x$enable_liverec = xyes; then
    AC_DEFINE([WITH_LIVEREC],[1],[Support live input from soundcard/microphone])
  fi
 
  # build targets:::

  AC_ARG_ENABLE([SMILExtract],[  --enable-SMILExtract  build standalone commandline feature extractor.  default=yes],
           build_smilextract="$enableval", build_featum="$build_smilextract")

  AC_ARG_ENABLE([SEMAINExtract],[  --enable-SEMAINExtract  build semaine component (feature extractor).  default=yes],
            build_semainextract="$enableval", build_semainextract="$build_semainextract")

  AC_ARG_ENABLE([SMILErec],[  --enable-SMILErec  build commandline audio recorder (older c version).  default=no],
            build_smilerec="$enableval", build_smilerec="$build_smilerec")

  AC_ARG_ENABLE([SMILEsync],[  --enable-SMILEsync  build semaine audio/video sync tool.  default=no],
            build_smilesync="$enableval", build_smilesync="$build_smilesync")

  AC_ARG_ENABLE([audiorecorder],[  --enable-audiorecorder  build simple commandline audio-recorder (c++ implementation).  default=no],
            build_audiorecorder="$enableval", build_audiorecorder="$build_audiorecorder")

  AC_ARG_ENABLE([audiosplit],[  --enable-audiosplit  build intelligent commandline audio splitter.  default=no],
            build_audiosplit="$enableval", build_audiosplit="$build_audiosoplit")

  dnl disable all targets that cannot be built due to missing dependencies
  if test "x$SEMAINEAPI" != "xyes" ; then
    if test "x$build_semainextract" = "xyes" ; then
      AC_MSG_WARN(Disabling target SEMAINExtract beacuse libsemaineapi was not found or is not useable)
    fi
    build_semainextract='no'
  fi

  if test "x$PORTAUDIO" != "xyes" ; then
    if test "x$build_smilerec" = "xyes" ; then
      AC_MSG_WARN(Disabling target SMILErec because libportaudio was not found or is not useable)
    fi
    if test "x$build_audiorecorder" = "xyes" ; then
      AC_MSG_WARN(Disabling target audiorecorder beacuse libportaudio was not found or is not useable)
    fi
    build_smilerec='no'
    build_audiorecorder='no'
  fi

  AM_CONDITIONAL([BUILD_SMILExtract],[test x$build_smilextract = xyes])
  AM_CONDITIONAL([BUILD_SEMAINExtract],[test x$build_semainextract = xyes])
  AM_CONDITIONAL([BUILD_SMILErec],[test x$build_smilerec = xyes])
  AM_CONDITIONAL([BUILD_SMILEsync],[test x$build_smilesync = xyes])
  AM_CONDITIONAL([BUILD_audiorecorder],[test x$build_audiorecorder = xyes])
  AM_CONDITIONAL([BUILD_audiosplit],[test x$build_audiosplit = xyes])

])
