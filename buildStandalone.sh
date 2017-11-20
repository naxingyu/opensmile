#!/bin/bash

# This file is part of openSMILE.
#
# Copyright (c) audEERING. All rights reserved.
# See the file COPYING for details on license terms.


OPENCV_INST=""
INSTPREFIX=""
STATIC_BIN="no"

while getopts "o:p:h" options; do
	case $options in
	o ) OPENCV_INST="$OPTARG";;
	p ) INSTPREFIX="$OPTARG";;
	h ) echo "Usage: $0 [-o <path_to_opencv>] [-p <inst_prefix>]"; exit;;
	esac
done

Pwd=`pwd`;
if [ "$INSTPREFIX" = "" ]; then
  INSTPREFIX="$Pwd/inst"
fi

if [ "$OPENCV_INST" != "" ]; then
  OPENCV_OPT="--with-opencv=$OPENCV_INST"
  STATIC_BIN="no"
else
  OPENCV_OPT="--enable-static --enable-shared=no"
#  STATIC_BIN="yes"
fi

rm SMILExtract 2> /dev/null
chmod +x autogen.sh
./autogen.sh ;
./autogen.sh ;
chmod +x configure
mkdir inst 2> /dev/null
##### DEBUG version:
#export CXXFLAGS="-O0"
#export CFLAGS="-O0"
#export CPPFLAGS="-D_DEBUG -g -D__STATIC_LINK -DOPENSMILE_BUILD -DBUILD_SVMSMO -DBUILD_SVMSMO -DBUILD_LIBSVM -DBUILD_RNN -DBUILD_WITHOUT_EXPERIMENTAL"
#### non-debug flags:
export CXXFLAGS="-O2 -mfpmath=sse -msse2"
export CFLAGS="-O2 -mfpmath=sse -msse2"
export CPPFLAGS="-D__STATIC_LINK -DOPENSMILE_BUILD -DBUILD_SVMSMO -DBUILD_SVMSMO -DBUILD_LIBSVM -DBUILD_RNN -DBUILD_WITHOUT_EXPERIMENTAL"
export LDFLAGS="-lrt -lm -lpthread -lc"
#####

echo ./configure --without-portaudio --prefix=$INSTPREFIX $OPENCV_OPT
./configure --without-portaudio --prefix=$INSTPREFIX $OPENCV_OPT
if [ $? != 0 ]; then
  echo "failed to configure openSMILE!";
  exit -1;
fi
chmod +x update_build_version.sh
./update_build_version.sh

make clean &&
make -j8; make install 
if [ $? != 0 ]; then
  echo "failed to build or install openSMILE to '$INSTPREFIX'!";
  exit -1;
fi

# fully static binary:
if [ "$STATIC_BIN" = "yes" ]; then
  rm SMILExtract
  g++ $CXXFLAGS -static -o SMILExtract progsrc/smilextract/SMILExtract-SMILExtract.o .libs/libopensmile.a $LDFLAGS 
  #-D__STDC_CONSTANT_MACROS 
  cp SMILExtract inst/bin/SMILExtract
fi
####

echo ""
echo "build successfull. You can now use the inst/bin/SMILExtract binary."
echo ""

