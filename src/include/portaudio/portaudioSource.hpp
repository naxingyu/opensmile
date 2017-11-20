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

portAudio dataSource for live recording from sound device
known to work on windows, linux, and mac

*/


#ifndef __CPORTAUDIOSOURCE_HPP
#define __CPORTAUDIOSOURCE_HPP

#include <core/smileCommon.hpp>
#include <core/dataSource.hpp>

#ifdef HAVE_PORTAUDIO

//#ifdef HAVE_PTHREAD
//#include <pthread.h>
//#endif

#define COMPONENT_DESCRIPTION_CPORTAUDIOSOURCE "This component handles live audio recording from the soundcard via the PortAudio library"
#define COMPONENT_NAME_CPORTAUDIOSOURCE "cPortaudioSource"


#define PA_STREAM_STOPPED 0
#define PA_STREAM_STARTED 1

/*
typedef struct {
  long sampleRate;
  int sampleType;
  int nChan;
  int blockSize;
  int nBPS;       // actual bytes per sample
  int nBits;       // bits per sample (precision)
  int byteOrder;  // BYTEORDER_LE or BYTEORDER_BE
  int memOrga;    // MEMORGA_INTERLV  or MEMORGA_SEPCH
  long nBlocks;  // nBlocks in buffer
} sWaveParameters;
*/

typedef void PaStream;

#undef class
class DLLEXPORT cPortaudioSource : public cDataSource {
  private:
    PaStream *stream;
    int byteSwap;
    long paFrames;
    int deviceId;
    int streamStatus;
    int listDevices;
    int numDevices;
    int lastDataCount;
    int isPaInit;
    int warned;

    long audioBuffersize;
    double audioBuffersize_sec;
    //long mBuffersize;
    
    //sWaveParameters pcmParam;

    int monoMixdown;    // if set to 1, multi-channel files will be mixed down to 1 channel

    long curReadPos;   // in samples
    int eof, abort; // NOTE : when setting abort, first lock the callbackMtx!!!
    int channels, sampleRate, nBits, nBPS, selectChannel;
    
    int setupDevice();

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    virtual int pauseEvent();
    virtual void resumeEvent(); 

    virtual int configureWriter(sDmLevelConfig &c);
    virtual int setupNewNames(long nEl);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    // variables for the callback method:
    smileMutex dataFlagMtx;
    smileMutex callbackMtx;
    smileCond  callbackCond;
    int dataFlag;
    cMatrix *callbackMatrix;
    
    cPortaudioSource(const char *_name);

    void printDeviceList();
    int startRecording();
    int stopRecording();
    int stopRecordingWait();
    
    cDataWriter * getWriter() { return writer_; }

    void setNewDataFlag() {
      smileMutexLock(dataFlagMtx);
      dataFlag = 1;
      lastDataCount=0;
      smileMutexUnlock(dataFlagMtx);
    }
    int getNBPS() { return nBPS; }
    int getNBits() { return nBits; }
    int getChannels() { return channels; }
    int getSelectedChannel() { return selectChannel; }
    int getSampleRate() { return sampleRate; }
    int isAbort() { return abort; }
    int isMonoMixdown() { return monoMixdown; }
    
    virtual ~cPortaudioSource();
};


#endif // HAVE_PORTAUDIO


#endif // __CPORTAUDIOSOURCE_HPP
