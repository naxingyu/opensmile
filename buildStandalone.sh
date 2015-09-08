#!/bin/bash

# fix permissions
chmod +x configure
chmod +x autogen.sh

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
  #STATIC_BIN="no"
else
  OPENCV_OPT="--enable-static --enable-shared=no"
  #STATIC_BIN="yes"
fi

rm SMILExtract 2> /dev/null
./autogen.sh ;
./autogen.sh ;
mkdir inst 2> /dev/null
## DEBUG version:
#export CXXFLAGS="-O0"
#export CFLAGS="-O0"
#export CPPFLAGS="-D_DEBUG -g -D__STATIC_LINK -DBUILD_LIBSVM -DBUILD_RNN -DBUILD_WITHOUT_EXPERIMENTAL"
## non-debug flags:
export CXXFLAGS="-O2 -mfpmath=sse -msse2"
export CFLAGS="-O2 -mfpmath=sse -msse2"
export CPPFLAGS="-D__STATIC_LINK -DBUILD_SVMSMO -DBUILD_LIBSVM -DBUILD_RNN -DBUILD_WITHOUT_EXPERIMENTAL"

echo ./configure --prefix=$INSTPREFIX $OPENCV_OPT
./configure --prefix=$INSTPREFIX $OPENCV_OPT
if [ $? != 0 ]; then
  echo "failed to configure openSMILE!";
  exit -1;
fi

chmod +x update_build_version.sh
./update_build_version.sh

make clean &&
make -j8 ; make install 
if [ $? != 0 ]; then
  echo "failed to build or install openSMILE to '$INSTPREFIX'!";
  exit -1;
fi

## fully static binary:
if [ "$STATIC_BIN" = "yes" ]; then
  rm SMILExtract
  g++ $CFLAGS -static -o SMILExtract progsrc/smilextract/SMILExtract-SMILExtract.o  -lm -pthread .libs/libopensmile.a
  cp SMILExtract inst/bin/SMILExtract
fi
####

echo ""
echo "build successfull. You can now use the inst/bin/SMILExtract binary."
echo ""

