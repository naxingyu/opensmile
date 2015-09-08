#!/bin/sh

if [ "$1" = "" ]; then
  echo "Usage: plotaudspec.sh <input csv file>";
  exit;
fi

perl transpose_lin.pl $1 .tmp.plot.dat
echo "Starting gnuplot, type 'quit' to exit the gnuplot prompt!"
gnuplot -persist plotmatrix_spec.gp
rm .tmp.plot.dat

