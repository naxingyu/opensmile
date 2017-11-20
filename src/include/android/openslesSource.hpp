/*F***************************************************************************
 * 
 * openSMILE - the Munich open source Multimedia Interpretation by 
 * Large-scale Extraction toolkit
 * 
 * This file is part of openSMILE.
 * 
 * openSMILE is copyright (c) by audEERING GmbH. All rights reserved.
 * 
 * See file "COPYING" for details on usage rights and licensing terms.
 * By using, copying, editing, compiling, modifying, reading, etc. this
 * file, you agree to the licensing terms in the file COPYING.
 * If you do not agree to the licensing terms,
 * you must immediately destroy all copies of this file.
 * 
 * THIS SOFTWARE COMES "AS IS", WITH NO WARRANTIES. THIS MEANS NO EXPRESS,
 * IMPLIED OR STATUTORY WARRANTY, INCLUDING WITHOUT LIMITATION, WARRANTIES OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ANY WARRANTY AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE SOFTWARE OR ANY WARRANTY OF TITLE
 * OR NON-INFRINGEMENT. THERE IS NO WARRANTY THAT THIS SOFTWARE WILL FULFILL
 * ANY OF YOUR PARTICULAR PURPOSES OR NEEDS. ALSO, YOU MUST PASS THIS
 * DISCLAIMER ON WHENEVER YOU DISTRIBUTE THE SOFTWARE OR DERIVATIVE WORKS.
 * NEITHER TUM NOR ANY CONTRIBUTOR TO THE SOFTWARE WILL BE LIABLE FOR ANY
 * DAMAGES RELATED TO THE SOFTWARE OR THIS LICENSE AGREEMENT, INCLUDING
 * DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL OR INCIDENTAL DAMAGES, TO THE
 * MAXIMUM EXTENT THE LAW PERMITS, NO MATTER WHAT LEGAL THEORY IT IS BASED ON.
 * ALSO, YOU MUST PASS THIS LIMITATION OF LIABILITY ON WHENEVER YOU DISTRIBUTE
 * THE SOFTWARE OR DERIVATIVE WORKS.
 * 
 * Main authors: Florian Eyben, Felix Weninger, 
 * 	      Martin Woellmer, Bjoern Schuller
 * 
 * Copyright (c) 2008-2013, 
 *   Institute for Human-Machine Communication,
 *   Technische Universitaet Muenchen, Germany
 * 
 * Copyright (c) 2013-2015, 
 *   audEERING UG (haftungsbeschraenkt),
 *   Gilching, Germany
 * 
 * Copyright (c) 2016,	 
 *   audEERING GmbH,
 *   Gilching Germany
 ***************************************************************************E*/


/*  openSMILE component:
***
***(c) 2014-2015 audEERING UG. All rights reserved.
***
***Fixed and updated by Florian Eyben, fe@audeering.com
***
***by Bernd Huber, 2014
***   berndbhuber@gmail.com

reads openSL audio buffer input into datamemory

*/

#ifndef __COPENSLESSOURCE_HPP
#define __COPENSLESSOURCE_HPP

#include <core/smileCommon.hpp>
#include <core/dataSource.hpp>

#ifdef __ANDROID__
#ifndef __STATIC_LINK
#ifdef HAVE_OPENSLES

#define BUILD_COMPONENT_OpenslesSource

#define COMPONENT_DESCRIPTION_COPENSLESSOURCE "This component reads opensl audio buffer input into datamemory."
#define COMPONENT_NAME_COPENSLESSOURCE "cOpenslesSource"

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
//#include <pthread.h>   // check if this is really required, as we use smileThread pthread layer!

/*typedef struct threadLock_{
  pthread_mutex_t m;
  pthread_cond_t  c;
  unsigned char   s;
} threadLock;
*/

using namespace std;
#define BUFFERFRAMES 1024
#define VECSAMPS_MONO 64
#define VECSAMPS_STEREO 128
#define SR 44100
#define CONV16BIT 32768
#define CONVMYFLT (1./32768.)

#define NUM_RECORDING_BUFFERS  2
#define NUM_PLAYBACK_BUFFERS   2

typedef struct opensl_stream22 {
  // engine interfaces
  SLObjectItf engineObject;
  SLEngineItf engineEngine;
  // output mix interfaces
  SLObjectItf outputMixObject;
  // buffer queue player interfaces
  SLObjectItf bqPlayerObject;
  SLPlayItf bqPlayerPlay;
  SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
  SLEffectSendItf bqPlayerEffectSend;
  // recorder interfaces
  SLObjectItf recorderObject;
  SLRecordItf recorderRecord;
  SLAndroidSimpleBufferQueueItf recorderBufferQueue;
  // buffer indexes
  int currentInputIndex;
  int currentOutputIndex;
  // current buffer half (0, 1)
  int currentOutputBuffer;
  int currentInputBuffer;
  // buffers
  int16_t * outputBuffer[NUM_RECORDING_BUFFERS];
  int16_t * inputBuffer[NUM_PLAYBACK_BUFFERS];
  // size of buffers
  int outBufSamples;
  int inBufSamples;
  double inBufMSeconds;
  // locks
  smileCond  recorderCondition_;
  smileCond  playerCondition_;
  smileMutex newDataMutex_;
  int hasNewInputData_;
  bool doNotEnqueue_;
  double time;
  int inchannels;
  int outchannels;
  int sr;  // sampling rate
} opensl_stream2;


#undef class
class DLLEXPORT cOpenslesSource : public cDataSource {
private:

  opensl_stream2 * audioDevice_;

      // if set to 1, multi-channel files will be mixed down to 1 channel
  //long curReadPos;   // in samples
  //int eof, abort; // NOTE : when setting abort, first lock the callbackMtx!!!

  long blockSizeReader_;

  // audio settings
  int monoMixdown_;
  int nChannels_;
  int nChannelsEffective_;   // nChannels or 1 if monoMixdown
  int sampleRate_;
  int nBits_;
  int nBPS_;
  int selectChannel_;
  long audioBuffersize_;
  double audioBuffersize_sec_;

  bool agcDebug_;
  int agcDebugCounter_;
  FLOAT_DMEM agcTarget_;
  FLOAT_DMEM agcGain_;
  FLOAT_DMEM agcPeakLp_;
  FLOAT_DMEM agcGainMin_;
  FLOAT_DMEM agcGainMax_;
  FLOAT_DMEM agcErrorIntegrated_;

  // variables for the background thread sync:
  smileMutex dataFlagMutex_;
  smileMutex threadMutex_;
  smileCond  threadCondition_;
  cMatrix *threadMatrix_;
  bool dataFlag_;
  bool threadActive_;
  bool threadStarted_;
  //int isInTickLoop_;
  bool agcEnabled_;

  // other
  bool warned_;
  const char *outFieldName_;

  // Audio recording queue thread
  smileThread recordingThread_;
  static SMILE_THREAD_RETVAL recordingThreadEntry(void *param);
  void recordingThreadLoop();


protected:
  SMILECOMPONENT_STATIC_DECL_PR

  virtual void fetchConfig();
  virtual int myConfigureInstance();
  virtual int myTick(long long t);

  virtual int configureWriter(sDmLevelConfig &c);
  virtual int setupNewNames(long nEl);
  virtual int myFinaliseInstance();

public:
  SMILECOMPONENT_STATIC_DECL

  cOpenslesSource(const char *_name);
  void performAgc(cMatrix * samples);
  /*
  void setNewDataFlag() {
    smileMutexLock(dataFlagMtx);
    dataFlag = 1;
    lastDataCount=0;
    smileMutexUnlock(dataFlagMtx);
  }
*/

  bool setupAudioDevice();
  bool startRecordingThread();
  void terminateAudioRecordingThread();

  //socket flags
  //int isReaderThreadStarted,isComponentConfigured,isReaderConfigured;
  //int isStillReading;
  //bool isReaderSet;

  //datamemory parameters
  //int frameN,frameSizeSec,fieldN,nChannels;
  //double blockSizeR,frameT;






  /*
	Open the audio device with a given sampling rate (sr), input and output channels and IO buffer size
	in frames. Returns a handle to the OpenSL stream
   */
  opensl_stream2 * android_OpenAudioDevice(int sr,
      int inchannels, int outchannels, int bufferframes,
      bool openPlayer, bool openRecording);
  /*
	Close the audio device
   */
  void android_CloseAudioDevice(opensl_stream2 *p);
  /*
	Read a buffer from the OpenSL stream *p, of size samples. Returns the number of samples read.
   */
  int android_AudioIn(opensl_stream2 *p, int16_t * buffer, int size);
  /*
	Write a buffer to the OpenSL stream *p, of size samples. Returns the number of samples written.
   */
  int android_AudioOut(opensl_stream2 *p, float *buffer,int size);
  /*
	Get the current IO block time in seconds
   */
  double android_GetTimestamp(opensl_stream2 *p);

  virtual ~cOpenslesSource();
};

void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);
void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

SLresult openSLCreateEngine(opensl_stream2 *p);
SLresult openSLPlayOpen(opensl_stream2 *p);
SLresult openSLRecOpen(opensl_stream2 *p);
SLresult openSLstartRecording(opensl_stream2 *p);
SLresult openSLstopRecording(opensl_stream2 *p);
void openSLDestroyEngine(opensl_stream2 *p);

#endif // HAVE_OPENSLES
#endif // !__STATIC_LINK
#endif // __ANDROID__

#endif // __COPENSLESSOURCE_HPP
