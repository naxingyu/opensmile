#!/bin/sh

if [ "$1" = "" ]; then
  echo "Usage: plotchroma.sh <input csv file>";
  exit;
fi

perl transpose.pl $1 .tmp.plot.dat
echo "Starting gnuplot, type 'quit' to exit the gnuplot prompt!"
gnuplot -persist plotmatrix_chroma.gp
rm .tmp.plot.dat

