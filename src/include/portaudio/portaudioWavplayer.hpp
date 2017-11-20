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

portAudio dataSink for live audio playback
(?? known to work on windows, linux, and mac)

*/


#ifndef __CPORTAUDIOWAVPLAYER_HPP
#define __CPORTAUDIOWAVPLAYER_HPP

#include <core/smileCommon.hpp>
#include <core/smileComponent.hpp>

#ifdef HAVE_PORTAUDIO

#include <portaudio.h>

#ifdef HAVE_JULIUSLIB
#include <classifiers/julius/kwsjKresult.h>
#endif 

#define COMPONENT_DESCRIPTION_CPORTAUDIOWAVPLAYER "This component can play wavefiles directly to a portaudio output device. Multiple wave files can be specified as inputs and they can be played via smile messages containing the index of the sample to play. Concurrent play requests will be discarded (TODO: mix the output). All wave files must have the same sampling rate."
#define COMPONENT_NAME_CPORTAUDIOWAVPLAYER "cPortaudioWavplayer"


#define PA_STREAM_STOPPED 0
#define PA_STREAM_STARTED 1

#undef class
class DLLEXPORT cPortaudioWavplayer : public cSmileComponent {
  private:
    PaStream *stream;
    long paFrames;
    int deviceId;
    int streamStatus;
    int listDevices;
    int numDevices;
    int isPaInit;
    
    long audioBuffersize;
    double audioBuffersize_sec;
    
    int monoMixdown;    // if set to 1, multi-channel files will be mixed down to 1 channel

    int eof, abort; // NOTE : when setting abort, first lock the callbackMtx!!!
    int inputChannels, channels, sampleRate, nBits, nBPS;
    
    const char *semaineCallbackRcpt;
    int isFirst;

    int numKw;
    const char ** keywords;
    int numInd;
    int * indices;


    int setupDevice();

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    //virtual int configureReader();

    virtual int processComponentMessage( cComponentMessage *_msg );

#ifdef HAVE_JULIUSLIB
    int getPlayIdxFromKeyword(juliusResult *k);
#endif
    int getPlayIdxFromClassname(const char *cls);
    int getPlayIdxFromClassidx(float idx);
    int triggerPlayback(int idx);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    // variables for the callback method:
    int wavsLoaded;
    int nWavs;
    float **wavedata;
    long *wavelength;

    int playback, playIndex;
    long playPtr;

    smileMutex dataFlagMtx;
    smileMutex callbackMtx;
    smileCond  callbackCond;
    int dataFlag;
        
    cPortaudioWavplayer(const char *_name);

    void printDeviceList();
    
    void sendEndMessage();

	  int startPlayback();
    int stopPlayback();
    int stopPlaybackWait();

    int getNBPS() { return nBPS; }
    int getNBits() { return nBits; }
    int getChannels() { return channels; }
    int getInputChannels() { return inputChannels; }
    int getSampleRate() { return sampleRate; }

    int isAbort() { return abort; }
    int isMonoMixdown() { return monoMixdown; }

    virtual ~cPortaudioWavplayer();
};


#endif // HAVE_PORTAUDIO


#endif // __CPORTAUDIOWAVPLAYER_HPP
