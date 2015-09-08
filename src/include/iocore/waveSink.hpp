/*F***************************************************************************
 * openSMILE - the open-Source Multimedia Interpretation by Large-scale
 * feature Extraction toolkit
 * 
 * (c) 2008-2011, Florian Eyben, Martin Woellmer, Bjoern Schuller: TUM-MMK
 * 
 * (c) 2012-2013, Florian Eyben, Felix Weninger, Bjoern Schuller: TUM-MMK
 * 
 * (c) 2013-2014 audEERING UG, haftungsbeschränkt. All rights reserved.
 * 
 * Any form of commercial use and redistribution is prohibited, unless another
 * agreement between you and audEERING exists. See the file LICENSE.txt in the
 * top level source directory for details on your usage rights, copying, and
 * licensing conditions.
 * 
 * See the file CREDITS in the top level directory for information on authors
 * and contributors. 
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
