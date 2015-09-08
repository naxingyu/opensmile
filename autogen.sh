#!/bin/bash

# Copyright (C) 2008 Florian Eyben
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

rm -f config.cache
echo "- aclocal."
aclocal -I m4
echo "- autoconf."
autoconf
echo "- autoheader."
autoheader
echo "- automake."
automake -a

# on Mac libtoolize is called glibtoolize
LIBTOOLIZE=libtoolize
if [ `uname -s` = Darwin ]; then
    LIBTOOLIZE=glibtoolize
fi
echo "- $LIBTOOLIZE"
$LIBTOOLIZE

exit
