///////////////////////////////////////////////////////////////////////////////////////
///////// > openSMILE LSTM-RNN voice activity detector<              //////////////////
/////////                                                            //////////////////
///////// (c) audEERING UG (haftungsbeschränkt),                     //////////////////
/////////     All rights reserverd.                                  //////////////////
///////////////////////////////////////////////////////////////////////////////////////


[componentInstances:cComponentManager]
instance[framer].type = cFramer
instance[vectorPreemphasis].type = cVectorPreemphasis
instance[windower].type = cWindower
 ; magnitude FFT spectrum
instance[fft].type=cTransformFFT
instance[fftmag].type=cFFTmagphase
 ; compute critical-bands from power spectrum
instance[melspec_VAD].type=cMelspec
 ; compute PLP 1-12 from critical-band spectrum
instance[plp_VAD].type=cPlp
 ; compute log-energy from raw signal frames 
 ; (not windowed, not pre-emphasised: that's the way HTK does it)
;instance[energy_VAD].type=cEnergy
 ; compute delta coefficients from PLP and energy
instance[delta_VAD].type=cDeltaRegression
 ; compute acceleration coefficients from delta coefficients of PLP and energy
;instance[accel_VAD].type=cDeltaRegression
 ; MVN (2x)
instance[mvn_VAD].type = cVectorMVN
;; experimental MVN adaption, uncomment to enable
;; and also change reader.dmLevel of lstm_vad component
;instance[mn2_VAD].type = cVectorMVN
 ; LSTM for VAD
instance[lstm_vad].type=cRnnProcessor
instance[dataSelector].type = cDataSelector

[framer:cFramer]
reader.dmLevel = wave
writer.dmLevel = frames
// nameAppend = 
copyInputName = 1
frameMode = fixed
// frameListFile = 
// frameList = 
frameSize = 0.025000
frameStep = 0.01
frameCenterSpecial = left
buffersize = 100000
noPostEOIprocessing = 1
 
[vectorPreemphasis:cVectorPreemphasis]
reader.dmLevel = frames
writer.dmLevel = prframes
// nameAppend = 
copyInputName = 1
processArrayFields = 1
k = 0.970000
de = 0
   
[windower:cWindower]
reader.dmLevel = prframes
writer.dmLevel = winframes
// nameAppend = 
copyInputName = 1
processArrayFields = 1
gain = 1
offset = 0
winFunc = ham
;sigma = 0.400000
;alpha = 0.160000

[fft:cTransformFFT]
reader.dmLevel=winframes
writer.dmLevel=fft

[fftmag:cFFTmagphase]
reader.dmLevel=fft
writer.dmLevel=fftmag

[melspec_VAD:cMelspec]
reader.dmLevel=fftmag
writer.dmLevel=melspec_power
; no htk compatible sample value scaling
htkcompatible = 0
nBands = 26
; use power spectrum instead of magnitude spectrum
usePower = 1
lofreq = 0
hifreq = 8000
specScale = mel

[plp_VAD:cPlp]
reader.dmLevel=melspec_power
writer.dmLevel=plp_VAD
buffersize=100
firstCC = 1
lpOrder = 18
cepLifter = 22
compression = 0.33
htkcompatible = 0
newRASTA = 1
RASTA = 0
rastaUpperCutoff = 29.0
rastaLowerCutoff = 0.9
doIDFT = 1
doLpToCeps = 1
doLP = 1
doInvLog = 1
doAud = 1
doLog = 1

[energy_VAD:cEnergy]
reader.dmLevel=winframes
writer.dmLevel=energy_VAD
htkcompatible=1
rms = 0
log = 1

[delta_VAD:cDeltaRegression]
reader.dmLevel=plp_VAD
writer.dmLevel=plpde_VAD
deltawin=2
blocksize=1

[accel_VAD:cDeltaRegression]
reader.dmLevel=plpde_VAD
writer.dmLevel=plpdede_VAD
deltawin=2
blocksize=1

[mvn_VAD:cVectorMVN]
reader.dmLevel = plp_VAD;plpde_VAD
writer.dmLevel = plpmvn_VAD
// nameAppend =
copyInputName = 1
processArrayFields = 0
mode = transform
initFile = rplp18d_norm.dat
htkcompatible = 0
meanEnable = 1
stdEnable = 1
normEnable = 0

; experimental incremental MVN adaptation
; disabled by default
[mn2_VAD:cVectorMVN]
reader.dmLevel = plpmvn_VAD
writer.dmLevel = plpmvn2_VAD
processArrayFields = 0
mode = incremental
updateMethod = fix
fixedBuffer = 30.0
meanEnable = 1
stdEnable = 0
normEnable = 0

[lstm_vad:cRnnProcessor]
reader.dmLevel = plpmvn_VAD
;; enable this, if you enable the 
;; experimental incremental adaptaion
; reader.dmLevel = plpmvn2_VAD
writer.dmLevel = vad_VAD
netfile = lstmvad_rplp18d_12.net

[dataSelector:cDataSelector]
reader.dmLevel = vad_VAD
writer.dmLevel = vad_VAD_voice
nameAppend = vadBin
copyInputName = 1
selectedRange = 0
elementMode = 1

[turn:cTurnDetector]
reader.dmLevel=vad_VAD_voice
writer.dmLevel=isTurn
readVad=1
threshold = -0.1
threshold2 = -0.1
writer.levelconf.noHang=1
msgInterval = 0
messageRecp = waveSinkCut
eventRecp = waveSinkCut
statusRecp = waveSinkCut
debug=\cm[turndebug{4}:set this to 1 to see turn/speaking debug messages]
maxTurnLength=0
maxTurnLengthGrace=1
nPre = 8
nPost = 35


