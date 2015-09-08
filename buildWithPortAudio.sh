#!/bin/sh

#Usage buildWithPortAudio.sh -p [install-prefix] -o [opencv path]
# Default install prefix is $Pwd/inst

# fix permissions
chmod +x configure
chmod +x autogen.sh

OPENCV_INST=""
INSTPREFIX=""

while getopts "o:p:" options; do
	case $options in
	o ) OPENCV_INST="$OPTARG";;
	p ) INSTPREFIX="$OPTARG";;
	\?) echo "Usage: $0 [-o <path_to_opencv>] [-p <inst_prefix>]";;
	esac
done


Pwd=`pwd`;
if [ "$INSTPREFIX" = "" ]; then
  INSTPREFIX="$Pwd/inst"
fi

if [ "$OPENCV_INST" != "" ]; then
	OPENCV_OPT="--with-opencv=$OPENCV_INST"
fi




echo "++++++++++++++ Compiling PortAudio...."

if [ -e "../thirdparty/portaudio.tgz" ]; then
  cd ../thirdparty ;
else
  cd thirdparty ;
fi
if [ ! -e "portaudio.tgz" ]; then
  echo "ERROR: No portaudio snapshot (portaudio.tgz) found in ../thirdparty or ./thirdparty.";
  echo "Download the latest snapshot from www.portaudio.com and rename it and place it in that location.";
  return
fi
tar -zxvf portaudio.tgz ;
cd portaudio ;
./configure --prefix=$INSTPREFIX  ;
if [ $? != 0 ]; then
  echo "failed to configure PortAudio!";
  exit -1;
fi
make ;
if [ $? != 0 ]; then
  echo "failed to build PortAudio!";
  exit -1;
fi
make install ;
if [ $? != 0 ]; then
  echo "failed to install PortAudio to '$Pwd/inst'!";
  exit -1;
fi

cd $Pwd ;

echo "++++++++++++++ Compiling openSMILE...."

rm SMILExtract 2> /dev/null
./autogen.sh ;
./autogen.sh ;
if [ $? != 0 ]; then
  echo "autogen.sh failed!";
  exit -1;
fi

mkdir inst 2> /dev/null
export CXXFLAGS="-O2"
export CFLAGS="-O2"
export CPPFLAGS="-DBUILD_SVMSMO -DBUILD_LIBSVM -DBUILD_RNN -DBUILD_WITHOUT_EXPERIMENTAL"

echo ./configure --prefix=$INSTPREFIX --with-portaudio="$INSTPREFIX"  "$OPENCV_OPT"
./configure --prefix=$INSTPREFIX --with-portaudio="$INSTPREFIX"  "$OPENCV_OPT"

if [ $? != 0 ]; then
  echo "failed to configure openSMILE!";
  exit -1;
fi

sh update_build_version.sh
make clean &&
make -j8 ; make install 
if [ $? != 0 ]; then
  echo "failed to build or install openSMILE!";
  exit -1;
fi


mv $INSTPREFIX/bin/SMILExtract $INSTPREFIX/bin/SMILExtract.bin
echo "#!/bin/sh" > $INSTPREFIX/bin/SMILExtract
echo "export LD_LIBRARY_PATH=\"\`dirname \$0\`/../lib\" ; \$0.bin \$* " >> $INSTPREFIX/bin/SMILExtract
chmod +x $INSTPREFIX/bin/SMILExtract


echo ""
echo "build successfull."
echo "  You can now use the $Pwd/inst/bin/SMILExtract executable,"
echo "  and add $Pwd/inst/bin to your path."
echo ""
echo "  Please note that $Pwd/inst/bin/SMILExtract is a wrapper script"
echo "  which sets up the library path and calls SMILExtract.bin"
echo ""

