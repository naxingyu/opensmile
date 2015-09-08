#!/usr/bin/perl

# Usage: transpose.pl <input file> [output file , default: same as input]
#
# this script transposes a CSV file (; delimiter)
# and saves it as a space separated file for gnuplot
# the values in the CSV file are squared, which usually
# leads to a better display behaviour for spectrograms, etc.
# Please see the comments in the code below, how to change this.
#

unless ($ARGV[0]) {
  print "Usage: transpose.pl <input file> [output file, default: overwrite input file]\n";
  exit -1;
}

$j=0;
open(F,"<$ARGV[0]");
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
if ($ARGV[1]) {
  open(F,">$ARGV[1]");
} else {
  open(F,">$ARGV[0]");
}
for ($n=$#d; $n>=0; $n--) {
  $s="";
  for ($i=0; $i<$j; $i++) {
    $s.=$d[$n][$i]*$d[$n][$i]." "; ## comment out this line with a "##" when using the line below
##    $s.=$d[$n][$i]." ";  ## uncomment this line, to use the magnitude instead of power
  }
  chop($s);
  print F $s."\n";
}
close(F);

