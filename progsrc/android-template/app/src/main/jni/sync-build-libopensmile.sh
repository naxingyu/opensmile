#!/usr/bin/env bash

# This file is part of openSMILE.
#
# Copyright (c) audEERING. All rights reserved.
# See the file COPYING for details on license terms.

ARCH="armeabi";

############ you might need to change this if you move the
############ android studio project somewhere else, i.e.
############ outside of the opensmile root folder
OPENSMILE_DIR="../../../../../../"
##########################################################

PWDD=`pwd`;
cd $OPENSMILE_DIR

## IMPORTANT:
##   run buildAndroid.sh in the opensmile trunk directory
##   once first to set up the build environment, 
##   or comment out the line below:
# sh buildAndroid.sh
## NOTE: leaving this enabled, will cause a full re-build, every time you run
##  this script to include a change in openSMILE in the compiled
##  android library.

# Alternatively, just use make to re-build the changed parts:
make -j8
make install

cd $PWDD
mkdir -p $ARCH
cp -f $OPENSMILE_DIR/inst/android/lib/libopensmile.a ./$ARCH

# not needed...
# cp -af $OPENSMILE_DIR/inst/android/lib/libopensmile.so* ./$ARCH

