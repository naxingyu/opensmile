
set pm3d map
set palette gray
set view map

set xlabel 'Time/frames'
set ylabel 'Spectral bin/band'
set zlabel 'Magnitude/Intensity'

## change the name to your filename...
splot '.tmp.plot.dat' matrix title 'Spectrogram'


