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

waveSink:
writes data to an uncompressed PCM WAVE file

*/


#ifndef __CWAVESINK_HPP
#define __CWAVESINK_HPP

#include <core/smileCommon.hpp>
#include <core/dataSink.hpp>
#include <iocore/waveSource.hpp>

#define COMPONENT_DESCRIPTION_CWAVESINK "This component saves data to an uncompressed PCM WAVE file"
#define COMPONENT_NAME_CWAVESINK "cWaveSink"

#undef class
class DLLEXPORT cWaveSink : public cDataSink {
private:
  const char *filename;
  FILE * fHandle;
  //int lag;
  int frameRead;
  int buffersize;
  int flushData;
  void *sampleBuffer; long sampleBufferLen;

  int nBitsPerSample;
  int nBytesPerSample;
  int sampleFormat;
  int nChannels;

  //double start, end, endrel;
  //long startSamples, endSamples, endrelSamples;

  long curWritePos;   // in samples??
  long nBlocks;
  //int eof;

  int writeWaveHeader();
  int writeData(cMatrix *m=NULL);

protected:
  SMILECOMPONENT_STATIC_DECL_PR

  virtual void fetchConfig();
  //virtual int myConfigureInstance();
  virtual int myFinaliseInstance();
  virtual int myTick(long long t);

  virtual int configureReader();

public:
  SMILECOMPONENT_STATIC_DECL

    cWaveSink(const char *_name);

  virtual ~cWaveSink();
};




#endif // __CWAVESINK_HPP
