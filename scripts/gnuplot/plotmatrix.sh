#!/bin/sh

if [ "$1" = "" ]; then
  echo "Usage: plotmatrix.sh <input csv file>";
  exit;
fi

perl transpose.pl $1 .tmp.plot.dat
echo "Starting gnuplot, type 'quit' to exit the gnuplot prompt!"
gnuplot -persist plotmatrix.gp
## to run gnuplot an remain in interactive mode:
## gnuplot plotmatrix.gp -
rm .tmp.plot.dat

