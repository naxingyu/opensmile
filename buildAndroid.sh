#!/bin/sh

#### UPDATE NDK path, if your NDK is not in the thirdparty folder
Pwd=`pwd`;
NDKPATH="$Pwd/../thirdparty/android-ndk";
LIBSTDC_VERSION="4.8";

##cross compiler variables
./autogen.sh ;
./autogen.sh ;
platform=android-9
export NDK="$NDKPATH"
export NDK_TOOLCHAIN="${NDK}/toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86/bin"
if [ ! -e "$NDK_TOOLCHAIN" ]; then
  export NDK_TOOLCHAIN="${NDK}/toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86_64/bin"
fi
export CROSS_COMPILE=arm-linux-androideabi
export CC=${NDK_TOOLCHAIN}/${CROSS_COMPILE}-gcc
export CXX=${NDK_TOOLCHAIN}/${CROSS_COMPILE}-g++
export LD=${NDK_TOOLCHAIN}/${CROSS_COMPILE}-ld
export AR=${NDK_TOOLCHAIN}/${CROSS_COMPILE}-ar
export RANLIB=${NDK_TOOLCHAIN}/${CROSS_COMPILE}-ranlib
export STRIP=${NDK_TOOLCHAIN}/${CROSS_COMPILE}-strip
export ac_cv_func_malloc_0_nonnull=yes
export ac_cv_func_realloc_0_nonnull=yes

##flags
export LDFLAGS="-Wl,-rpath-link=${NDK}/platforms/${platform}/arch-arm/usr/lib -L${NDK}/platforms/${platform}/arch-arm/usr/lib -L${NDK}/sources/cxx-stl/gnu-libstdc++/$LIBSTDC_VERSION/libs/armeabi -llog"
export SYSROOT=${NDK}/platforms/${platform}/arch-arm
export CXXFLAGS="-I${NDK}/platforms/${platform}/arch-arm/usr/include -I${NDK}/sources/cxx-stl/gnu-libstdc++/$LIBSTDC_VERSION/include -I${NDK}/sources/cxx-stl/gnu-libstdc++/$LIBSTDC_VERSION/libs/armeabi/include --sysroot=${SYSROOT} -O3"
export CPPFLAGS="--sysroot=${SYSROOT}"
export CFLAGS="--sysroot=${SYSROOT} -nostdlib -O3"
###add compiler options 

##configure opensmile
./configure --enable-shared --host=arm-linux-androideabi --target=arm-linux-androideabi LIBS="-lc -lm -lgcc -lgnustl_static -lsupc++"

if [ $? != 0 ]; then
	echo "failed to configure openSMILE!";
	exit -1;
fi

make clean &&
make ; make 
if [ $? != 0 ]; then
	echo "failed to build or install openSMILE!";
	exit -1;
fi

chmod 777 SMILExtract

echo ""
echo "build successfull. You can now use the SMILExtract binary for Android. For linking with Android project, use builds in ./libs folder."
echo ""
