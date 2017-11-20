
ACLOCAL_AMFLAGS = -I m4

AUTOMAKE_OPTIONS = subdir-objects
  
bin_PROGRAMS = SMILExtract 
#bin_PROGRAMS += SEMAINExtract

SMILELIB_SOURCES=\
	src/classifiers/julius/juliusSink.cpp \
	src/classifiers/libsvm/svm.cpp \
	src/classifiers/libsvmliveSink.cpp \
	src/classifiers/svmSink.cpp \
	src/core/commandlineParser.cpp \
	src/core/componentManager.cpp \
	src/core/configManager.cpp \
	src/core/dataMemory.cpp \
	src/core/dataProcessor.cpp \
	src/core/dataReader.cpp \
	src/core/dataSelector.cpp \
	src/core/dataSink.cpp \
	src/core/dataSource.cpp \
	src/core/dataWriter.cpp \
	src/core/exceptions.cpp \
	src/core/nullSink.cpp \
	src/core/smileCommon.cpp \
	src/core/smileComponent.cpp \
	src/core/smileLogger.cpp \
	src/core/vecToWinProcessor.cpp \
	src/core/vectorProcessor.cpp \
	src/core/vectorTransform.cpp \
	src/core/winToVecProcessor.cpp \
	src/core/windowProcessor.cpp \
	src/dsp/dbA.cpp \
	src/dsp/signalGenerator.cpp \
	src/dsp/smileResample.cpp \
	src/dsp/specResample.cpp \
	src/dsp/vadV1.cpp \
	src/dspcore/acf.cpp \
	src/dspcore/amdf.cpp \
	src/dspcore/contourSmoother.cpp \
	src/dspcore/deltaRegression.cpp \
	src/dspcore/fftmagphase.cpp \
	src/dspcore/fftsg.c \
	src/dspcore/framer.cpp \
	src/dspcore/fullinputMean.cpp \
	src/dspcore/fullturnMean.cpp \
	src/dspcore/monoMixdown.cpp \
	src/dspcore/preemphasis.cpp \
	src/dspcore/transformFft.cpp \
	src/dspcore/turnDetector.cpp \
	src/dspcore/vectorMVN.cpp \
	src/dspcore/vectorPreemphasis.cpp \
	src/dspcore/windower.cpp \
	src/examples/exampleSink.cpp \
	src/examples/exampleSource.cpp \
        src/examples/simpleMessageSender.cpp \
	src/functionals/functionalComponent.cpp \
	src/functionals/functionalCrossings.cpp \
	src/functionals/functionalDCT.cpp \
	src/functionals/functionalExtremes.cpp \
	src/functionals/functionalLpc.cpp \
	src/functionals/functionalMeans.cpp \
	src/functionals/functionalMoments.cpp \
	src/functionals/functionalOnset.cpp \
	src/functionals/functionalPeaks.cpp \
	src/functionals/functionalPeaks2.cpp \
	src/functionals/functionalPercentiles.cpp \
	src/functionals/functionalRegression.cpp \
	src/functionals/functionalSamples.cpp \
	src/functionals/functionalSegments.cpp \
	src/functionals/functionalTimes.cpp \
	src/functionals/functionals.cpp \
	src/lld/lsp.cpp \
	src/dsp/specScale.cpp \
	src/io/libsvmSink.cpp \
	src/iocore/arffSink.cpp \
	src/iocore/arffSource.cpp \
	src/iocore/csvSink.cpp \
	src/iocore/csvSource.cpp \
	src/iocore/datadumpSink.cpp \
	src/iocore/htkSink.cpp \
	src/iocore/htkSource.cpp \
	src/iocore/waveSink.cpp \
	src/iocore/waveSinkCut.cpp \
	src/iocore/waveSource.cpp \
	src/lld/cens.cpp \
	src/lld/chroma.cpp \
	src/lld/harmonics.cpp \
	src/lld/formantLpc.cpp \
	src/lld/formantSmoother.cpp \
	src/lld/lpc.cpp \
	src/lld/pitchDirection.cpp \
	src/lld/pitchJitter.cpp \
	src/lld/pitchShs.cpp \
	src/lld/pitchSmootherViterbi.cpp \
	src/lld/tonefilt.cpp \
	src/lld/tonespec.cpp \
	src/lldcore/energy.cpp \
	src/lldcore/intensity.cpp \
	src/lldcore/melspec.cpp \
	src/lldcore/mfcc.cpp \
	src/lldcore/mzcr.cpp \
	src/lldcore/pitchACF.cpp \
	src/lldcore/pitchBase.cpp \
	src/lldcore/pitchSmoother.cpp \
	src/lldcore/plp.cpp \
	src/lldcore/spectral.cpp \
	src/other/bowProducer.cpp \
	src/other/maxIndex.cpp \
	src/other/valbasedSelector.cpp \
	src/other/vectorConcat.cpp \
	src/other/vectorOperation.cpp \
	src/portaudio/portaudioDuplex.cpp \
	src/portaudio/portaudioSink.cpp \
	src/portaudio/portaudioSource.cpp \
	src/portaudio/portaudioWavplayer.cpp \
        src/android/openslesSource.cpp \
        src/android/jniMessageInterface.cpp \
	src/rnn/rnn.cpp \
	src/rnn/rnnProcessor.cpp \
	src/rnn/rnnSink.cpp \
	src/rnn/rnnVad2.cpp \
	src/smileutil/smileUtil.c \
	src/smileutil/smileUtilSpline.c \
	src/smileutil/zerosolve.cpp \
	src/video/openCVSource.cpp
	


  SMILE_SOURCES=$(SMILELIB_SOURCES)

  SMILE_CPPFLAGS = -Isrc/include -Iprogsrc/include $(SEMAINEAPI_CPPFLAGS) $(PORTAUDIO_CPPFLAGS) $(OPENCV_CPPFLAGS) -ggdb $(PTHREAD_CFLAGS)
  SMILE_LIBS =  $(PORTAUDIO_LDFLAGS) $(OPENCV_LDFLAGS) -lm $(SEMAINEAPI_LDFLAGS) $(PTHREAD_LIBS) $(DLIB)
   
  SMILExtract_SOURCES = progsrc/smilextract/SMILExtract.cpp 
  SMILExtract_CPPFLAGS = $(SMILE_CPPFLAGS)
  SMILExtract_LDADD = $(SMILE_LIBS) -lopensmile

  lib_LTLIBRARIES = libopensmile.la
  libopensmile_la_SOURCES = $(SMILE_SOURCES)
  libopensmile_la_CPPFLAGS = $(SMILE_CPPFLAGS)
  libopensmile_la_LIBADD = $(SMILE_LIBS)
  libopensmile_la_LDFLAGS = --no-undefined
  


# endif 


