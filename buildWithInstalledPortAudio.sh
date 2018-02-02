#!/bin/sh

# This file is part of openSMILE.
#
# Copyright (c) audEERING. All rights reserved.
# See the file COPYING for details on license terms.

#Usage buildPA.sh [install-prefix]
# Default install prefix is $Pwd/inst

# EXPERIMENTAL AND UNSUPPORTED!!!!   Update to latest build code in buildWithPortaudio.sh to make it work!

Pwd=`pwd`;
if [ "$1" != "" ]; then
  INSTPREFIX=$1;  
else
  INSTPREFIX="$Pwd/inst";
fi


echo "++++++++++++++ Compiling openSMILE...."

./autogen.sh ;
./autogen.sh ;
if [ $? != 0 ]; then
  echo "autogen.sh failed!";
  exit -1;
fi

mkdir inst
export CXXFLAGS="-O2 -mfpmath=sse -march=native"
export CFLAGS="-O2 -mfpmath=sse -march=native"
#export CXXFLAGS="-O2 -mfpmath=sse -mtune=athlon64"
#export CFLAGS="-O2 -mfpmath=sse -mtune=athlon64"
#./configure --prefix=$Pwd/inst --enable-static --disable-shared &&
./configure --prefix=$INSTPREFIX --with-portaudio="yes" 
if [ $? != 0 ]; then
  echo "failed to configure openSMILE!";
  exit -1;
fi


make clean &&
make -j8 ; make install 
if [ $? != 0 ]; then
  echo "failed to build or install openSMILE!";
  exit -1;
fi


mv $INSTPREFIX/bin/SMILExtract $INSTPREFIX/bin/SMILExtract.bin
echo "#!/bin/sh" > $INSTPREFIX/bin/SMILExtract
echo "export LD_LIBRARY_PATH=\"`dirname \$0`/../lib\" ; \$0.bin \$* " >> $INSTPREFIX/bin/SMILExtract
chmod +x $INSTPREFIX/bin/SMILExtract

$INSTPREFIX/bin/SMILExtract -H cPortaudio 2> tmp
TMP=`cat tmp | grep cPortaudio `;
rm tmp

if [ "$TMP" = "" ]; then
  echo "ERROR: PortAudio was not found on your system, please check if you have the development headers installed! If unsure, use the script buildWithPortAudio.sh! openSMILE was built, however without portaudio support!";
else 

echo ""
echo "build successfull."
echo "  You can now use the $Pwd/inst/bin/SMILExtract executable,"
echo "  and add $Pwd/inst/bin to your path."
echo ""
echo "  Please note that $Pwd/inst/bin/SMILExtract is a wrapper script"
echo "  which sets up the library path and calls SMILExtract.bin"
echo ""


fi
