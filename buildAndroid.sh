#!/bin/sh

# This file is part of openSMILE.
#
# Copyright (c) audEERING. All rights reserved.
# See the file COPYING for details on license terms.

STATIC_BIN="no";
Pwdd=`pwd`;

if [ "$STATIC_BIN" = "yes" ]; then
  STATIC_DEFINE="-D__STATIC_LINK";
  DYN_LIBS="";
  ENABLE_SHARED="no";
else
  DYN_LIBS="-llog -lOpenSLES";
  ENABLE_SHARED="yes";
  STATIC_DEFINE="";
fi

##cross compiler variables
./autogen.sh ;
./autogen.sh ;

## cross compiler variables
platform=android-21

############## NOTE: Update the path to your Android NDK.
##############       Version 10e is required!
export NDK=`realpath ~/android/ndk-r10e`;
#########################################################

export CROSS_COMPILE=arm-linux-androideabi
STL_VERSION=4.8

export NDK_TOOLCHAIN=${NDK}/toolchains/${CROSS_COMPILE}-4.9/prebuilt/linux-x86_64/bin
export CC=${NDK_TOOLCHAIN}/${CROSS_COMPILE}-gcc
export CXX=${NDK_TOOLCHAIN}/${CROSS_COMPILE}-g++
export LD=${NDK_TOOLCHAIN}/${CROSS_COMPILE}-ld
export AR=${NDK_TOOLCHAIN}/${CROSS_COMPILE}-ar
export RANLIB=${NDK_TOOLCHAIN}/${CROSS_COMPILE}-ranlib
export STRIP=${NDK_TOOLCHAIN}/${CROSS_COMPILE}-strip
export ac_cv_func_malloc_0_nonnull=yes
export ac_cv_func_realloc_0_nonnull=yes

##flags
export OPTIMISE="-fPIE -pie -ffast-math -ftree-vectorize -march=armv7-a -mthumb -mfloat-abi=softfp -mfpu=vfp -mfpu=neon -funsafe-math-optimizations";
export LDFLAGS="-fPIE -pie -fexceptions -Wl,-rpath-link=${NDK}/platforms/${platform}/arch-arm/usr/lib -L${NDK}/platforms/${platform}/arch-arm/usr/lib -L${NDK}/sources/cxx-stl/gnu-libstdc++/${STL_VERSION}/libs/armeabi"
export LDFLAGS_CORE="-fPIE -pie -fexceptions -L${NDK}/platforms/${platform}/arch-arm/usr/lib -L${NDK}/sources/cxx-stl/gnu-libstdc++/${STL_VERSION}/libs/armeabi"
export SYSROOT=${NDK}/platforms/${platform}/arch-arm
export CXXFLAGS="-I${NDK}/platforms/${platform}/arch-arm/usr/include -I${NDK}/sources/cxx-stl/gnu-libstdc++/${STL_VERSION}/include -I${NDK}/sources/cxx-stl/gnu-libstdc++/${STL_VERSION}/libs/armeabi/include --sysroot=${SYSROOT} -g -O3 $OPTIMISE"
DEFINES="-DOPENSMILE_BUILD -DBUILD_LIBSVM -DBUILD_SVMSMO -DBUILD_RNNFAST -DBUILD_WITHOUT_EXPERIMENTAL"

export CPPFLAGS="--sysroot=${SYSROOT} -D__ANDROID__ ${DEFINES} -DHAVE_OPENSLES $STATIC_DEFINE"
export CFLAGS="--sysroot=${SYSROOT} -nostdlib -g -O3 ${OPTIMISE}"
###add compiler options 
#export LIBS="-lc -lm -lgcc -lgnustl_shared -lsupc++ -llog -lOpenSLES"
export LIBS="-lc -lm -lgcc -lgnustl_static -lsupc++ $DYN_LIBS"

##configure opensmile
mkdir -p "$Pwdd/inst/android"
./configure --enable-static --enable-shared=no --host=arm-linux-androideabi --target=arm-linux-androideabi LIBS="$LIBS" --prefix="$Pwdd/inst/android";

if [ $? != 0 ]; then
	echo "failed to configure openSMILE!";
	exit -1;
fi

make clean &&
make -j8 
make install
if [ $? != 0 ]; then
	echo "failed to build openSMILE!";
	exit -1;
fi

# fully static binary:
if [ "$STATIC_BIN" = "yes" ]; then
  rm SMILExtract
  $CXX $CXXFLAGS -o $Pwdd/inst/android/SMILExtract-static progsrc/smilextract/SMILExtract-SMILExtract.o .libs/libopensmile.a -static $LDFLAGS $LIBS
fi
####

chmod 777 SMILExtract
chmod 777 inst/android/bin/SMILExtract

echo ""
if [ "$STATIC_BIN" = "yes" ]; then
  echo "build successfull. You can now use the inst/android/bin/SMILExtract-static binary for Android. For linking with Android project, use builds in inst/android/lib folder."
else
  echo "build successfull. You can now use the inst/android/bin/SMILExtract binary for Android. For linking with Android project, use builds in inst/android/lib folder."
fi
echo ""

