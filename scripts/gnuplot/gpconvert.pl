#!/usr/bin/perl

# Usage: gpconvert.pl <input file> [output file , default: same as input]
#
# this script converts a CSV file (; delimiter)
# and saves it as a space separated file for gnuplot
#

$col = $ARGV[0];

unless ($ARGV[1]) {
  print "Usage: gpconvert.pl <column> <input file> [output file, default: overwrite input file]\n";
  exit -1;
}

$j=0;
open(F,"<$ARGV[1]");
while(<F>) {
  $_=~s/\r?\n//;
  if ($_ !~ /;/) { print "no column delimiter ; found !"; exit -1; }
  if ($_ !~ /[a-df-zA-DF-Z]/) { # skip a header line with alphanumeric characters
  split(/;/);  # <<- the delimiter ;
  for ($i=0;$i<=$#_;$i++) { $d[$i][$j] = $_[$i]; }
  $j++;
  }
}
close(F);
if ($ARGV[2]) {
  open(F,">$ARGV[2]");
} else {
  open(F,">$ARGV[1]");
}
for ($n=0; $n<$j; $n++) {
  $s="$n ";
#  for ($i=0; $i<$#d; $i++) {
    $s.=$d[$col][$n]." ";  
#  }
  chop($s);
  print F $s."\n";
}
close(F);

