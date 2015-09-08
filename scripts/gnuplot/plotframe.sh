#!/bin/sh

if [ "$1" = "" ]; then
  echo "Usage: plotframe.sh <input csv file> [index of frame to extract, default: the first (0)]";
  exit;
fi

if [ "$2" = "" ]; then
  perl gpframeconvert.pl 0 $1 .tmp.plot.dat
else
  perl gpframeconvert.pl $2 $1 .tmp.plot.dat
fi

echo "Starting gnuplot, type 'quit' to exit the gnuplot prompt!"
gnuplot -persist plotframe.gp
## to run gnuplot an remain in interactive mode:
## gnuplot plotframe.gp -
rm .tmp.plot.dat

