#!/bin/sh

if [ "$1" = "" ]; then
  echo "Usage: plotcontour.sh <column to extract> <input csv file>";
  exit;
fi

perl gpconvert.pl $1 $2 .tmp.plot.dat
echo "Starting gnuplot, type 'quit' to exit the gnuplot prompt!"
gnuplot -persist plotcontour.gp
## to run gnuplot an remain in interactive mode:
## gnuplot plotcontour.gp -
rm .tmp.plot.dat

