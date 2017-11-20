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
 * written by Bernd Huber, 2014
 * berndbhuber@gmail.com
---
reads openSL audio buffer input into datamemory
Still under development, data can be corrupt depending on platform!!!
*/


#include <android/openslesSource.hpp>
#define MODULE "cOpenslesSource"

#ifdef __ANDROID__
#ifndef __STATIC_LINK
#ifdef HAVE_OPENSLES
SMILECOMPONENT_STATICS(cOpenslesSource)

SMILECOMPONENT_REGCOMP(cOpenslesSource)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_COPENSLESSOURCE;
  sdescription = COMPONENT_DESCRIPTION_COPENSLESSOURCE;

  // we inherit cDataSource configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSource")

  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("monoMixdown", "Mix down all channels to 1 mono channel (1=on, 0=off)", 1);
    ct->setField("sampleRate","Set/force the sampling rate that is assigned to the input data.",16000,0,0);
    ct->setField("channels","Set/force the number of input (recording) channels",1,0,0);
	  ct->setField("outFieldName", "Set the name of the output field, containing the pcm data", "pcm");
    //ct->setField("selectChannel","Select only the specified channel from 'channels' that are recorded. Set to -1 to grab all channels.",-1);
    ct->setField("nBits","The number of bits per sample and channel",16);
    ct->setField("nBPS","The number of bytes per sample and channel (0=determine automatically from nBits)",0,0,0);
    ct->setField("audioBuffersize","The size of the portaudio recording buffer in samples (overwrites audioBuffersize_sec, if set)",0,0,0);
    ct->setField("audioBuffersize_sec","size of the portaudio recording buffer in seconds. This value has influence on the system latency. Setting it too high might introduce a high latency. A too low value might lead to dropped samples and reduced performance.",0.05);
    ct->setField("agcEnabled", "1/0 = enable automatic gain control for input audio", 0);
    ct->setField("agcTarget", "Target peak level for AGC, if enabled.", 0.5);
    ct->setField("agcDebug", "1/0 = enable agc debugging output to log.", 0);
  )

  SMILECOMPONENT_MAKEINFO(cOpenslesSource);
}

SMILECOMPONENT_CREATE(cOpenslesSource)

//-----

cOpenslesSource::cOpenslesSource(const char *_name) :
  cDataSource(_name),
  audioBuffersize_(0),
  audioBuffersize_sec_(0),
  audioDevice_(NULL),
  sampleRate_(0),
  warned_(false),
  threadStarted_(false),
  threadActive_(false),
  threadMatrix_(NULL),
  outFieldName_(NULL),
  blockSizeReader_(0),
  agcGainMin_(0.02), agcGainMax_(20.0),
  agcGain_(1.0), agcPeakLp_(0.0),
  agcErrorIntegrated_(0.0),
  agcTarget_(0.5),
  agcDebugCounter_(0),
  agcDebug_(false)
{
  smileCondCreate(threadCondition_);
  smileMutexCreate(dataFlagMutex_);
  smileMutexCreate(threadMutex_);
}

void cOpenslesSource::fetchConfig()
{
  cDataSource::fetchConfig();
  agcTarget_ = getDouble("agcTarget");
  agcDebug_ = (getInt("agcDebug") == 1);
  monoMixdown_ = getInt("monoMixdown");
  if (isSet("audioBuffersize")) {
    audioBuffersize_ = getInt("audioBuffersize");
  }
  if (isSet("audioBuffersize_sec")) {
    audioBuffersize_sec_ = getDouble("audioBuffersize_sec");
  }
  sampleRate_ = getInt("sampleRate");
  if (sampleRate_ <= 0) {
    SMILE_IERR(1, "sampleRate (%i) must be > 0! Setting to 16000.", sampleRate_);
    sampleRate_ = 16000;
  }
  nChannels_ = getInt("channels");
  if (nChannels_ < 1) {
    SMILE_IERR(1, "channels (%i) must be >= 1. Setting to 1.", nChannels_);
    nChannels_ = 1;
  }
  agcEnabled_ = (getInt("agcEnabled") == 1);
/*
  selectChannel_ = getInt("selectChannel");
  if (selectChannel_ >= 0) {
    SMILE_MSG(2, "only selected %i. channel.", selectChannel_);
  }*/
  nBits_ = getInt("nBits");
  nBPS_ = getInt("nBPS");
  if (nBPS_ == 0) {
    switch (nBits_) {
    case 8: nBPS_=1; break;
    case 16: nBPS_=2; break;
    case 24: nBPS_=4; break;
    case 32: nBPS_=4; break;
    case 33: nBPS_=4; break;
    case 0:  nBPS_=4;
             nBits_=32;
             break;
    default:
      SMILE_ERR(1, "invalid number of bits requested: %i (allowed: 8, 16, 24, 32, 33(for 23-bit float))\n   Setting number of bits to default (16)", nBits_);
      nBits_ = 16;
    }
  }
  if (nBPS_ != 2) {
    SMILE_IERR(1, "This openSLES interface currently only supports 16-bit (2-byte) per sample PCM formats!");
    COMP_ERR("aborting");
  }
  outFieldName_ = getStr("outFieldName");
}

bool cOpenslesSource::setupAudioDevice() {
  // Opensl initialization for recording
  SMILE_IMSG(4, "calling setup audio sr %i, nCh %i, abfs %i", sampleRate_, nChannels_, audioBuffersize_);
  if (audioDevice_ == NULL) {
    audioDevice_ = android_OpenAudioDevice(sampleRate_,
        nChannels_, 2, audioBuffersize_,
        false /* playback */, true /* record */);
    if (audioDevice_ != NULL) {
      SMILE_IMSG(3, "Opened OpenSLES audio device.");
      return true;
    } else {
      SMILE_IERR(1, "Failed to open OpenSLES audio device! Check your audio settings, sample rate etc.");
      return false;
    }
  } else {
    return true;
  }
}

bool cOpenslesSource::startRecordingThread() {
  if (!threadStarted_) {
    // thread should only be created once, configureWrite could be called multiple times
    threadStarted_ = true;
    smileThreadCreate(recordingThread_, recordingThreadEntry, this);
  }
  return true;
}

int cOpenslesSource::configureWriter(sDmLevelConfig &c)
{
  double TT = 1.0;
  c.T = 1.0/(double)sampleRate_;
  if (c.T != 0.0)
    TT = c.T;
  if (audioBuffersize_ > 0) {
    c.blocksizeWriter = audioBuffersize_;
    audioBuffersize_sec_ = (double)audioBuffersize_ * TT;
  } else if (audioBuffersize_sec_ > 0.0) {
    c.blocksizeWriter = blocksizeW_ = audioBuffersize_ = (long)ceil(audioBuffersize_sec_ / TT);
  } else {
    if (audioBuffersize_ != 1024) {
      SMILE_IMSG(3,"using default audioBuffersize (1024 samples) since no option was given in config file");
      audioBuffersize_ = 1024;
    }
    c.blocksizeWriter = audioBuffersize_;
    audioBuffersize_sec_ = (double)audioBuffersize_ * TT;
  }
  blocksizeW_ = audioBuffersize_;
  blocksizeW_sec = audioBuffersize_sec_;
  if (setupAudioDevice()) {
    nChannelsEffective_ = (monoMixdown_ || selectChannel_ ? 1 : nChannels_);
    c.N = nChannelsEffective_;
    SMILE_IMSG(3, "nChannelsEffective = %i", nChannelsEffective_);
    return 1;
  } else {
    return 0;
  }
}

int cOpenslesSource::myConfigureInstance()
{
  return cDataSource::myConfigureInstance();
}

int cOpenslesSource::setupNewNames(long nEl)
{
  SMILE_IMSG(4, "calling setup new names: name = %s ; matrix rows: %i", outFieldName_, nChannelsEffective_);
  writer_->addField(outFieldName_, nChannelsEffective_);
  namesAreSet_ = 1;
  return nChannelsEffective_;
}

int cOpenslesSource::myFinaliseInstance() {
  int ret = cDataSource::myFinaliseInstance();
  if (ret) {
    if (threadMatrix_ == NULL)
      threadMatrix_ = new cMatrix(nChannelsEffective_, audioBuffersize_ / nChannelsEffective_);
  }
  return ret;
}

/*
int cOpenslesSource::pauseEvent()
{
  stopRecording();
  return 1;
}

void cOpenslesSource::resumeEvent()
{
  startRecording();
}
*/

int cOpenslesSource::myTick(long long t)
{
  if (isPaused() || isEOI())
    return 0;
  if (!startRecordingThread())
    return 0;
// TODO: need to return 0 if recording thread was stoppped!

  if (blockSizeReader_ <= 0) {
    const sDmLevelConfig *c = writer_->getLevelConfig();
    if (c != NULL) {
      blockSizeReader_ = c->blocksizeReader;
      SMILE_IMSG(4, "blockSizeReader = %i, blockSizeWriter = %i", c->blocksizeReader, c->blocksizeWriter);
    }
  }
  //if (t < 10000)
    //SMILE_IMSG(4, "in tick. avail = %i, total = %i  bsReader=%i", writer_->getNAvail(), writer_->getBuffersize(), blockSizeReader_);
  // this will always try to fill the buffer as much as possible... for multiple threads this is absolutely ok,
  //   for single threads the buffer might overflow... thus for single threads we might also want to check for (writer->getNAvail() < audioBuffersize )
  if ((writer_->checkWrite(audioBuffersize_)) && (writer_->getNAvail() < blockSizeReader_)) {
    //SMILE_IMSG(4, "tick wait()");
    //smileCondTimedWait(threadCondition_, 1);  // timeout must be greater (smaller?) as expected audio buffersize?
    // TODO: The logic of timeout depends on the buffersize....
    //  if the reader only reads "1" sample in each tick
    //   and we receive 1000 samples when threadCond is signalled,
    //   then something will eventually block!
    //  in the example config this is due to the waveSink component (500 samples vs. 1024 samples)!

    //smileMutexLock(dataFlagMutex_);
    //dataFlag_ = false;
    //smileMutexUnlock(dataFlagMutex_);
    //SMILE_IMSG(4, "XXXX tick done");   // with this in there the sync seems to work.. find out why!
    return 1;
  } else {
    //printf("no wait! %i  %i  %i bs=%i\n",t,writer->getNAvail(), (writer->checkWrite(audioBuffersize)), audioBuffersize);
    if (writer_->getNFree() < audioBuffersize_ * 2) {
      if (!warned_) {
        SMILE_IWRN(3, "Audio buffer FULL, processing seems to be too slow. Audio data possibly lost!\n NOTE: this warning will appear only once!");
        warned_ = true;
      }
    } else {
      if (warned_) {
        SMILE_IWRN(3, "Audio buffer has space again. Processing back to normal, not slow anymore..\n");
      }
      warned_ = false;
    }
    // this does not work, processing in multi-thread hangs after a while
  }
  //SMILE_IMSG(4, "tick done");
  smileMutexLock(dataFlagMutex_);
  if (dataFlag_) {
    dataFlag_ = false;
    smileMutexUnlock(dataFlagMutex_);
    return 1;
  }
  smileMutexUnlock(dataFlagMutex_);
  return 0;
}

// TODO: must be called if terminated via component manager! on = 0.. ?
void cOpenslesSource::terminateAudioRecordingThread() {
  //   1. set threadActive_ = 0 to terminate the thread (while having mutex threadMutex locked, but unlock after)
  smileMutexLock(threadMutex_);
  int threadActive = threadActive_;
  threadActive_ = 0;
  smileMutexUnlock(threadMutex_);
  if (threadActive) {  // old state. Do not signal and wait if thread was not active
    //   2. notify the audioDevice_->recorderCondition_ to unlock the recorder thread waiting for new audio
    smileCondSignal(audioDevice_->recorderCondition_);
    // join thread: wait until it terminates
    smileThreadJoin(recordingThread_);
  }
}

cOpenslesSource::~cOpenslesSource() {
  SMILE_IMSG(3, "Waiting for audio recording thread to finish.");
  terminateAudioRecordingThread();
  if (threadMatrix_ != NULL)
    delete threadMatrix_;
  android_CloseAudioDevice(audioDevice_);
  smileCondDestroy(threadCondition_);
  smileMutexDestroy(dataFlagMutex_);
  smileMutexDestroy(threadMutex_);
  SMILE_IMSG(3, "cleanup successful");
}

//--------------------------------------------------------------------------------openSLES code from here

SMILE_THREAD_RETVAL cOpenslesSource::recordingThreadEntry(void *param)
{
	//SMILE_IMSG(4,"Starting Audio Source Thread in OpenSL");
  ((cOpenslesSource*)param)->recordingThreadLoop();
  SMILE_THREAD_RET;
}

// FLOAT_DMEM peakLp = 0;
// FLOAT_DMEM agcGain = 1;
// FLOAT_DMEM agcGainMin = 0.02;
// FLOAT_DMEM agcGainMax = 20.0;

void cOpenslesSource::performAgc(cMatrix * samples)
{
  if (samples == NULL)
    return;
  // apply gain to audio buffer
  for (int i = 0; i < samples->nT; i++) {
    samples->dataF[i] *= agcGain_;
  }
  // get frame peak
  FLOAT_DMEM peak = 0.0;
  for (int i = 0; i < samples->nT; i++) {
    if (fabs(samples->dataF[i]) > peak) {
      peak = fabs(samples->dataF[i]);
    }
  }
  // lowpass of peak and delta
  FLOAT_DMEM peakLpLast = agcPeakLp_;
  //agcPeakLp_ = 0.995 * agcPeakLp_ + 0.005 * peak;
  if (peak > agcPeakLp_) {
    agcPeakLp_ = 0.4 * agcPeakLp_ + 0.6 * peak;
  } else {
    agcPeakLp_ = 0.995 * agcPeakLp_ + 0.005 * peak;
  }
  // compute current error and integrate error if not outside gain bounds
  FLOAT_DMEM error = agcTarget_ - agcPeakLp_ ;
  if (agcGain_ < agcGainMax_ && agcGain_ > agcGainMin_)
    agcErrorIntegrated_ += error;
  FLOAT_DMEM errorDelta = error - (agcTarget_ - peakLpLast);
  // compute PID output
  FLOAT_DMEM Kp = 0.1;
  FLOAT_DMEM Ki = 0.0;
  FLOAT_DMEM Kd = 0.1;
  FLOAT_DMEM update = Kp * error + Ki * agcErrorIntegrated_ + Kd * errorDelta;
  // peakTarget = update + peakCurrent;
  // == >  peakTarget = peakCurrent * (1 + update/peakCurrent);
  //       peakTarget = peakCurrent * gain
  agcGain_ = 1.0 + update/agcPeakLp_;
  // limit gain
  if (agcGain_ > agcGainMax_)
    agcGain_ = agcGainMax_;
  if (agcGain_ < agcGainMin_)
    agcGain_ = agcGainMin_;
  if (agcDebug_) {
    agcDebugCounter_++;
    if (agcDebugCounter_ == 100) {
      SMILE_IMSG(1, "agcGain: %.4f, agcPeakLp: %.4f, update: %.4f, error: %.4f, errorIntegrated: %.4f",
          agcGain_, agcPeakLp_, update, error, agcErrorIntegrated_);
      agcDebugCounter_ = 0;
    }
  }
}

void cOpenslesSource::recordingThreadLoop()
{
  int16_t * inbuffer = (int16_t *)calloc(1, sizeof(int16_t) * audioBuffersize_);
  smileMutexLock(threadMutex_);
	threadActive_ = 1;
  int threadActive = threadActive_;
	smileMutexUnlock(threadMutex_);
	SMILE_IMSG(3, "OpenSLES audio recording thread initialized");
	if (audioDevice_ == NULL) {
	  SMILE_IERR(2, "No openSLES audio device opened, terminating openSLES audio recording thread.");
	  free(inbuffer);
	  return;
	}
	if (openSLstartRecording(audioDevice_) != SL_RESULT_SUCCESS) {
	  SMILE_IERR(1, "Failed to start recording, terminating openSLES audio recording thread.");
	  free(inbuffer);
	  return;
	}
	sWaveParameters pcmParam;
	pcmParam.nBits = nBits_;
	pcmParam.nBPS = nBPS_;
	pcmParam.nChan = nChannels_;
  // NOTE: we open device before tick-loop to know whether the settings are supported
  // However we need to check what happens to audio that is recorded while configuration is still going
  // buffers discarded?
	// TERMINATING:
	//   1. set threadActive_ = 0 to terminate the thread (while having mutex threadMutex locked, but unlock after)
	//   2. notify the audioDevice_->recorderCondition_ to unlock the recorder thread waiting for new audio
	//   3.
	while (threadActive) {
		int nSamples = android_AudioIn(audioDevice_, inbuffer, audioBuffersize_);
		//SMILE_IMSG(4, "read nSamples = %i in thread.", nSamples);
		// Convert samples from binary buffer to float array, given the wave parameters
		//  *buf : the raw data buffer
		//  *pcmParam : parameter struct specifying the sample format
		//  *a : The array where the converted buffer will be stored in. It must be pre-allocated to the right size! [nChan * nSamples * sizeof(float)]
		//  nChan : number of audio channels,
		//  nSamples : number of audio samples in the buffers
		//  monoMixdown : 1 = convert from multi-channel recording to mono (1 channel)
		//  Return value: number of samples processed (= nSamples)
		//DLLEXPORT int smilePcm_convertSamples(uint8_t *buf, sWaveParameters *pcmParam, float *a, int nChan, int nSamples, int monoMixdown);

		// TODO: check that nT will never exceed the allocated size of the matrix!
		threadMatrix_->nT = smilePcm_convertSamples((uint8_t *)inbuffer, (sWaveParameters *)&pcmParam,
		    threadMatrix_->dataF, nChannels_, nSamples / nChannels_, monoMixdown_/*, selectChannels_*/);
		if (agcEnabled_)
		  performAgc(threadMatrix_);
		// TODO: check for lost data here due to full levels!
		writer_->setNextMatrix(threadMatrix_);
		smileMutexLock(dataFlagMutex_);
		dataFlag_ = true;
		smileMutexUnlock(dataFlagMutex_);
		// signal the threadCondition_ to show that there is new data.
		smileCondSignal(threadCondition_);  // TODO: what happens if we signal twice??
    // update the thread active status
		smileMutexLock(threadMutex_);
    threadActive = threadActive_;
	  smileMutexUnlock(threadMutex_);
	}
	SMILE_IMSG(3, "OpenSLES thread was signaled to end.");
	openSLstopRecording(audioDevice_);
	SMILE_IMSG(4, "Recording stopped.");
	free(inbuffer);
  smileMutexLock(threadMutex_);
  threadActive_ = 0;
  threadStarted_ = 0;
  smileMutexUnlock(threadMutex_);
}



// open the android audio device for input and/or output
opensl_stream2 * cOpenslesSource::android_OpenAudioDevice(int sr,
    int inchannels, int outchannels, int bufferSize,
    bool openPlay, bool openRecord) {
  opensl_stream2 *p;
  p = (opensl_stream2 *) calloc(sizeof(opensl_stream2), 1);
  p->doNotEnqueue_ = false;
  p->hasNewInputData_ = 0;
  if (bufferSize <= 0) {
    SMILE_IERR(1, "bufferSize must be > 0 in android_OpenAudioDevice()");
    android_CloseAudioDevice(p);
    return NULL;
  }
  smileCondCreate(p->recorderCondition_);
  smileCondCreate(p->playerCondition_);
  smileMutexCreate(p->newDataMutex_);
  p->sr = sr;  // sampling rate assignment
  if (openRecord) {
    p->inchannels = inchannels;
    p->inBufSamples = bufferSize * inchannels;
    p->inBufMSeconds = (int)floor(audioBuffersize_sec_ * 1000.0);  // TODO: compute from bufferSize and sample period!
    if (p->inBufSamples != 0) {
      for (int i = 0; i < NUM_RECORDING_BUFFERS; ++i) {
        p->inputBuffer[i] = (int16_t *)calloc(p->inBufSamples, sizeof(int16_t));
        if (p->inputBuffer[i] == NULL) {
          SMILE_IERR(1, "Failed to allocate audio recording buffer memory # %i.", i);
          android_CloseAudioDevice(p);
          return NULL;
        }
      }
    }
    p->currentInputIndex = p->inBufSamples;  // this will ensure that we:
        // a) enqueue the last buffer and wait for the first one in audioIn
  }
  if (openPlay) {
    p->outchannels = outchannels;
    p->outBufSamples = bufferSize * outchannels;
    if (p->outBufSamples != 0) {
      for (int i = 0; i < NUM_PLAYBACK_BUFFERS; ++i) {
        p->outputBuffer[i] = (int16_t *)calloc(p->outBufSamples, sizeof(int16_t));
        if (p->outputBuffer[i] == NULL) {
          SMILE_IERR(1, "Failed to allocate audio playback buffer memory # %i.", i);
          android_CloseAudioDevice(p);
          return NULL;
        }
      }
    }
    p->currentOutputBuffer = 0;
  }

  if (openSLCreateEngine(p) != SL_RESULT_SUCCESS) {
    SMILE_IERR(2, "Failed to create openSL engine.");
    android_CloseAudioDevice(p);
    return NULL;
  }
  if (openRecord) {
    if (openSLRecOpen(p) != SL_RESULT_SUCCESS) {
      SMILE_IERR(2, "Failed to open openSL audio recording.");
      android_CloseAudioDevice(p);
      return NULL;
    }
  }
  if (openPlay) {
    if (openSLPlayOpen(p) != SL_RESULT_SUCCESS) {
      SMILE_IERR(2, "Failed to open openSL audio playback.");
      android_CloseAudioDevice(p);
      return NULL;
    }
  }
  p->time = 0.0;
  return p;
}

// close the android audio device
void cOpenslesSource::android_CloseAudioDevice(opensl_stream2 *p) {
  if (p == NULL)
    return;
  openSLDestroyEngine(p);  // destroys player, recorder, mix, and engine
  smileCondDestroy(p->recorderCondition_);
  smileCondDestroy(p->playerCondition_);
  smileMutexDestroy(p->newDataMutex_);
  for (int i = 0; i < NUM_RECORDING_BUFFERS; ++i) {
    if (p->inputBuffer[i] != NULL) {
      free(p->inputBuffer[i]);
      p->inputBuffer[i] = NULL;
    }
  }
  for (int i = 0; i < NUM_PLAYBACK_BUFFERS; ++i) {
    if (p->outputBuffer[i] != NULL) {
      free(p->outputBuffer[i]);
      p->outputBuffer[i] = NULL;
    }
  }
  free(p);
}

// returns timestamp of the processed stream
double cOpenslesSource::android_GetTimestamp(opensl_stream2 *p){
  return p->time;
}

// gets a buffer of size samples from the device
int cOpenslesSource::android_AudioIn(opensl_stream2 *p, int16_t * buffer, int size){
  int16_t * inBuffer = NULL;
  //SMILE_IMSG(5, "waiting for a frame of size %d", size);
  int nSamples = p->inBufSamples;
  if (p == NULL || nSamples ==  0 || size == 0)
    return 0;
  int nSamplesRead = 0;
  inBuffer = p->inputBuffer[p->currentInputBuffer];
  for (nSamplesRead = 0; nSamplesRead < size; ++nSamplesRead) {
    if (p->currentInputIndex >= nSamples) {  // jump to next buffer
      // enqueue buffer and update current buffer to the next one
      if (!p->doNotEnqueue_) {
        (*p->recorderBufferQueue)->Enqueue(p->recorderBufferQueue,
            p->inputBuffer[p->currentInputBuffer], nSamples * sizeof(int16_t));
        p->currentInputBuffer = (p->currentInputBuffer + 1) % NUM_RECORDING_BUFFERS;
      }
      //smileCondWait(p->recorderCondition_);   // TODO: set timeout according to audioBuffersize_sec
      smileCondTimedWait(p->recorderCondition_, (p->inBufMSeconds * 3) / 2);
      bool newData = false;
      int bufferNum = 0;
      smileMutexLock(p->newDataMutex_);
      if (p->hasNewInputData_) {
        newData = true;
        bufferNum = p->hasNewInputData_;
        p->hasNewInputData_--;
      }
      smileMutexUnlock(p->newDataMutex_);
      if (newData) {
        //SMILE_IMSG(4, "  got buffer (%i)", bufferNum);
        // update inBuffer and index
        // flag buffer for enqueue (after data has been copied!)
        p->doNotEnqueue_ = false;
        p->currentInputIndex = 0;
        inBuffer = p->inputBuffer[p->currentInputBuffer];
      } else {
        SMILE_IMSG(4, "  got timeout");
        // timeout reading from device or no more data
        break;
      }
    }
    buffer[nSamplesRead] = inBuffer[p->currentInputIndex++];
  }
  // if at the end of the above loop we have copied all data from the buffer
  // enqueue right away, to allow audio recording to continue
  // in case the copying of the data to the datamemory in the thread takes
  // longer than expected (i.e. thread gets interrupted by scheduler, etc.)
  if (p->currentInputIndex >= nSamples) {
    if (!p->doNotEnqueue_) {
      (*p->recorderBufferQueue)->Enqueue(p->recorderBufferQueue,
          p->inputBuffer[p->currentInputBuffer], nSamples * sizeof(int16_t));
      p->currentInputBuffer = (p->currentInputBuffer + 1) % NUM_RECORDING_BUFFERS;
    }
    p->doNotEnqueue_ = true;
  }
  //if (p->outchannels == 0)  // ???
    p->time += (double)nSamplesRead / (double)(p->sr * p->inchannels);
  //SMILE_IMSG(4, "audioIn: # %i", nSamplesRead);
  return nSamplesRead;
}


// puts a buffer of size samples to the device
int cOpenslesSource::android_AudioOut(opensl_stream2 *p, float *buffer,int size){

  short *outBuffer;
  int i, bufsamps = p->outBufSamples, index = p->currentOutputIndex;
  if(p == NULL  || bufsamps ==  0)  return 0;
  outBuffer = p->outputBuffer[p->currentOutputBuffer];

  for(i=0; i < size; i++){
    outBuffer[index++] = (short) (buffer[i]*CONV16BIT);
    if (index >= p->outBufSamples) {
      smileCondTimedWait(p->playerCondition_, 500000);  // TODO: set timeout according to audioBuffersize_sec
      (*p->bqPlayerBufferQueue)->Enqueue(p->bqPlayerBufferQueue,
           outBuffer,bufsamps*sizeof(short));
      p->currentOutputBuffer = (p->currentOutputBuffer ?  0 : 1);
      index = 0;
      outBuffer = p->outputBuffer[p->currentOutputBuffer];
    }
  }
  p->currentOutputIndex = index;
  p->time += (double) size/(p->sr*p->outchannels);
  return i;
}


//----------------------------------------------openSL specific code

// creates the OpenSL ES audio engine
SLresult openSLCreateEngine(opensl_stream2 *p) {
  SLresult result;
  // create engine
  result = slCreateEngine(&(p->engineObject), 0, NULL, 0, NULL, NULL);
  if (result != SL_RESULT_SUCCESS) {
    return result;
  }
  // realize the engine
  result = (*p->engineObject)->Realize(p->engineObject, SL_BOOLEAN_FALSE);
  if (result != SL_RESULT_SUCCESS) {
    return result;
  }
  // get the engine interface, which is needed in order to create other objects
  result = (*p->engineObject)->GetInterface(p->engineObject, SL_IID_ENGINE, &(p->engineEngine));
  return result;
}

// opens the OpenSL ES device for output
SLresult openSLPlayOpen(opensl_stream2 *p) {
  SLresult result;
  SLuint32 sr = p->sr;
  SLuint32 channels = p->outchannels;
  if (channels){
    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    switch(sr){
    case 8000:
      sr = SL_SAMPLINGRATE_8;
      break;
    case 11025:
      sr = SL_SAMPLINGRATE_11_025;
      break;
    case 16000:
      sr = SL_SAMPLINGRATE_16;
      break;
    case 22050:
      sr = SL_SAMPLINGRATE_22_05;
      break;
    case 24000:
      sr = SL_SAMPLINGRATE_24;
      break;
    case 32000:
      sr = SL_SAMPLINGRATE_32;
      break;
    case 44100:
      sr = SL_SAMPLINGRATE_44_1;
      break;
    case 48000:
      sr = SL_SAMPLINGRATE_48;
      break;
    case 64000:
      sr = SL_SAMPLINGRATE_64;
      break;
    case 88200:
      sr = SL_SAMPLINGRATE_88_2;
      break;
    case 96000:
      sr = SL_SAMPLINGRATE_96;
      break;
    case 192000:
      sr = SL_SAMPLINGRATE_192;
      break;
    default:
      return -1;
    }
    const SLInterfaceID ids[] = {SL_IID_VOLUME};
    const SLboolean req[] = {SL_BOOLEAN_FALSE};
    result = (*p->engineEngine)->CreateOutputMix(p->engineEngine,
        &(p->outputMixObject), 1, ids, req);
    // realize the output mix
    result = (*p->outputMixObject)->Realize(p->outputMixObject, SL_BOOLEAN_FALSE);
    int speakers = SL_SPEAKER_FRONT_CENTER;
    if(channels > 1)
      speakers = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM,channels, sr,
				   SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
				   speakers, SL_BYTEORDER_LITTLEENDIAN};
    SLDataSource audioSrc = {&loc_bufq, &format_pcm};
    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, p->outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};
    // create audio player
    const SLInterfaceID ids1[] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
    const SLboolean req1[] = {SL_BOOLEAN_TRUE};
    result = (*p->engineEngine)->CreateAudioPlayer(p->engineEngine, &(p->bqPlayerObject),
        &audioSrc, &audioSnk, 1, ids1, req1);
    if(result != SL_RESULT_SUCCESS) goto end_openaudio;
    // realize the player
    result = (*p->bqPlayerObject)->Realize(p->bqPlayerObject, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS) goto end_openaudio;
    // get the play interface
    result = (*p->bqPlayerObject)->GetInterface(p->bqPlayerObject, SL_IID_PLAY, &(p->bqPlayerPlay));
    if(result != SL_RESULT_SUCCESS) goto end_openaudio;
    // get the buffer queue interface
    result = (*p->bqPlayerObject)->GetInterface(p->bqPlayerObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
						&(p->bqPlayerBufferQueue));
    if(result != SL_RESULT_SUCCESS) goto end_openaudio;
    // register callback on the buffer queue
    result = (*p->bqPlayerBufferQueue)->RegisterCallback(p->bqPlayerBufferQueue, bqPlayerCallback, p);
    if(result != SL_RESULT_SUCCESS) goto end_openaudio;
    // set the player's state to playing
    result = (*p->bqPlayerPlay)->SetPlayState(p->bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    SMILE_MSG(3, "playback: set state = playing");
  end_openaudio:
    return result;
  }
  return SL_RESULT_SUCCESS;
}

// Open the OpenSL ES device for input
SLresult openSLRecOpen(opensl_stream2 *p) {
  SLresult result;
  SLuint32 sr = p->sr;
  SLuint32 channels = p->inchannels;
  SMILE_MSG(3, "recording: slrecopen");
  if (channels){
    switch (sr){
    case 8000:
      sr = SL_SAMPLINGRATE_8;
      break;
    case 11025:
      sr = SL_SAMPLINGRATE_11_025;
      break;
    case 16000:
      sr = SL_SAMPLINGRATE_16;
      break;
    case 22050:
      sr = SL_SAMPLINGRATE_22_05;
      break;
    case 24000:
      sr = SL_SAMPLINGRATE_24;
      break;
    case 32000:
      sr = SL_SAMPLINGRATE_32;
      break;
    case 44100:
      sr = SL_SAMPLINGRATE_44_1;
      break;
    case 48000:
      sr = SL_SAMPLINGRATE_48;
      break;
    case 64000:
      sr = SL_SAMPLINGRATE_64;
      break;
    case 88200:
      sr = SL_SAMPLINGRATE_88_2;
      break;
    case 96000:
      sr = SL_SAMPLINGRATE_96;
      break;
    case 192000:
      sr = SL_SAMPLINGRATE_192;
      break;
    default:
      return -1;
    }
    SMILE_MSG(3, "recording: sr constant = %i", sr);
    // configure audio source
    SLDataLocator_IODevice loc_dev = {SL_DATALOCATOR_IODEVICE, SL_IODEVICE_AUDIOINPUT,
				      SL_DEFAULTDEVICEID_AUDIOINPUT, NULL};
    SLDataSource audioSrc = {&loc_dev, NULL};
    // configure audio sink
    int speakers = SL_SPEAKER_FRONT_CENTER;
    if(channels > 1)
      speakers = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    SLDataLocator_AndroidSimpleBufferQueue loc_bq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, channels, sr,
				   SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
				   speakers, SL_BYTEORDER_LITTLEENDIAN};
    SLDataSink audioSnk = {&loc_bq, &format_pcm};
    // create audio recorder
    // (requires the RECORD_AUDIO permission)
    const SLInterfaceID id[1] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};
    result = (*p->engineEngine)->CreateAudioRecorder(p->engineEngine, &(p->recorderObject),
        &audioSrc, &audioSnk, 1, id, req);
    if (SL_RESULT_SUCCESS != result) goto end_recopen;
    // realize the audio recorder
    result = (*p->recorderObject)->Realize(p->recorderObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) goto end_recopen;
    // get the record interface
    result = (*p->recorderObject)->GetInterface(p->recorderObject, SL_IID_RECORD, &(p->recorderRecord));
    if (SL_RESULT_SUCCESS != result) goto end_recopen;
    // get the buffer queue interface
    result = (*p->recorderObject)->GetInterface(p->recorderObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
						&(p->recorderBufferQueue));
    if (SL_RESULT_SUCCESS != result) goto end_recopen;
    SMILE_MSG(3, "recording: got interface");
    // register callback on the buffer queue
    result = (*p->recorderBufferQueue)->RegisterCallback(p->recorderBufferQueue,
        bqRecorderCallback, p);
    if (SL_RESULT_SUCCESS != result) goto end_recopen;
    SMILE_MSG(3, "recording: registered callback");

  end_recopen:
    return result;
  }
  return SL_RESULT_SUCCESS;
}

SLresult openSLstopRecording(opensl_stream2 *p) {
  SLresult result;
  if (p->recorderRecord != NULL) {
    // in case already recording, stop recording and clear buffer queue
    result = (*p->recorderRecord)->SetRecordState(p->recorderRecord, SL_RECORDSTATE_STOPPED);
    if (result != SL_RESULT_SUCCESS) {
      SMILE_ERR(3, "openSL: failed to set recorder state to stopped.");
      return result;
    }
    //  (void)result;
    result = (*p->recorderBufferQueue)->Clear(p->recorderBufferQueue);
    if (result != SL_RESULT_SUCCESS) {
      SMILE_ERR(3, "openSL: failed to clear recorder buffer queue.");
      return result;
    }
    SMILE_MSG(3, "openSL: recording: set state = stopped, success");
  } else {
    SMILE_ERR(1, "openSL: no recorder present, cannot stop recording");
    return -1;
  }
  return SL_RESULT_SUCCESS;
}

SLresult openSLstartRecording(opensl_stream2 *p) {
  SLresult result;
  if (p->recorderRecord != NULL) {
    // in case already recording, stop recording and clear buffer queue
    result = (*p->recorderRecord)->SetRecordState(p->recorderRecord, SL_RECORDSTATE_STOPPED);
    if (result != SL_RESULT_SUCCESS) {
      SMILE_ERR(3, "openSL: failed to set recorder state to stopped.");
      return result;
    }
    //  (void)result;
    result = (*p->recorderBufferQueue)->Clear(p->recorderBufferQueue);
    if (result != SL_RESULT_SUCCESS) {
      SMILE_ERR(3, "openSL: failed to clear recorder buffer queue.");
      return result;
    }
    // for streaming recording, we enqueue at least 2 empty buffers to start things off)
    for (int i = 0; i < NUM_RECORDING_BUFFERS /* - 1*/; ++i) {
      result = (*p->recorderBufferQueue)->Enqueue(p->recorderBufferQueue,
          p->inputBuffer[i], p->inBufSamples * sizeof(int16_t));
      // the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
      // which for this code example would indicate a programming error
      if (result != SL_RESULT_SUCCESS) {
        SMILE_ERR(3, "openSL: failed to enqueue audio buffer # %i for streaming recording.", i);
        return result;
      }
    }
    // the current one will be the first to be enqueued, before we wait for the first buffer to be filled
    // in audioIn. This is why we don't enqueue it here (NUM_RECORDING_BUFFERS - 1 in above loop).
//    p->currentInputBuffer = NUM_RECORDING_BUFFERS - 1;
    p->currentInputBuffer = 0;
    p->doNotEnqueue_ = true;
    // start recording
    result = (*p->recorderRecord)->SetRecordState(p->recorderRecord, SL_RECORDSTATE_RECORDING);
    if (result != SL_RESULT_SUCCESS) {
      SMILE_ERR(3, "openSL: failed to set recorder state to started.");
      return result;
    }
    SMILE_MSG(3, "openSL: recording: set state = recording, success");
  } else {
    SMILE_ERR(1, "openSL: no recorder present, cannot start recording");
    return -1;
  }
  return SL_RESULT_SUCCESS;
}

// close the OpenSL IO and destroy the audio engine
void openSLDestroyEngine(opensl_stream2 *p) {
  // destroy buffer queue audio player object, and invalidate all associated interfaces
  if (p->bqPlayerObject != NULL) {
    (*p->bqPlayerObject)->Destroy(p->bqPlayerObject);
    p->bqPlayerObject = NULL;
    p->bqPlayerPlay = NULL;
    p->bqPlayerBufferQueue = NULL;
    p->bqPlayerEffectSend = NULL;
  }
  // destroy audio recorder object, and invalidate all associated interfaces
  if (p->recorderObject != NULL) {
    (*p->recorderObject)->Destroy(p->recorderObject);
    p->recorderObject = NULL;
    p->recorderRecord = NULL;
    p->recorderBufferQueue = NULL;
  }
  // destroy output mix object, and invalidate all associated interfaces
  if (p->outputMixObject != NULL) {
    (*p->outputMixObject)->Destroy(p->outputMixObject);
    p->outputMixObject = NULL;
  }
  // destroy engine object, and invalidate all associated interfaces
  if (p->engineObject != NULL) {
    (*p->engineObject)->Destroy(p->engineObject);
    p->engineObject = NULL;
    p->engineEngine = NULL;
  }
}


// TODO: the callbacks should not be class members, because they cannot access class members due to missing this pointer
// this callback handler is called every time a buffer finishes recording
void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
  opensl_stream2 *p = (opensl_stream2 *) context;
  //SMILE_MSG(3, "opensles: recorder callback triggered");
  smileMutexLock(p->newDataMutex_);
  p->hasNewInputData_++;
  smileMutexUnlock(p->newDataMutex_);
  smileCondSignal(p->recorderCondition_);
}

// TODO: the callbacks should not be class members, because they cannot access class members due to missing this pointer
// this callback handler is called every time a buffer finishes playing
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
  opensl_stream2 *p = (opensl_stream2 *) context;
  SMILE_MSG(3, "opensles: player callback triggered");
  smileCondSignal(p->playerCondition_);
}


#endif // HAVE_OPENSLES
#endif // !__STATIC_LINK
#endif // __ANDROID__
