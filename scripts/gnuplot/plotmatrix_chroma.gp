
set pm3d map
set palette gray
set view map

set xlabel 'Time/frames'
set ylabel 'Semitone bin (0-11)'
set zlabel 'Intensity'

## change the name to your filename...
splot '.tmp.plot.dat' matrix title 'Chromagram'


