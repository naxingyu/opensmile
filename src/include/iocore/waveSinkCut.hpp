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


#ifndef __CWAVESINKCUT_HPP
#define __CWAVESINKCUT_HPP

#include <core/smileCommon.hpp>
#include <core/dataSink.hpp>
#include <iocore/waveSource.hpp>

#define COMPONENT_DESCRIPTION_CWAVESINKCUT "This component writes data to uncompressed PCM WAVE files. Only chunks, based on timings received via smile messages are written to files. The files may have consecutive numbers appended to the file name."
#define COMPONENT_NAME_CWAVESINKCUT "cWaveSinkCut"

#undef class
class DLLEXPORT cWaveSinkCut : public cDataSink {
  private:
    const char *fileExtension;
    const char *filebase;
    const char *fileNameFormatString;
    int multiOut;
    long curFileNr;
    
    long forceSampleRate;
    long preSil, postSil;
    int turnStart, turnEnd, nPre, nPost, vIdxStart, vIdxEnd, curVidx;
    float startSec0_, startSec_, endSec_;
    long curStart, curEnd;
    int isTurn, endWait;

    FILE * fHandle;
    void *sampleBuffer; long sampleBufferLen;

  	int nBitsPerSample;
	  int nBytesPerSample;
	  int sampleFormat;
    int nChannels;
    long fieldSize;
	
    long nOvl;

  	long curWritePos;   // in samples??
	  long nBlocks;
	  int showSegmentTimes_;
	  const char * saveSegmentTimes_;

    int writeWaveHeader();
    int writeDataFrame(cVector *m=NULL);
    char * getCurFileName();
    void saveAndPrintSegmentData(long n);

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);
    virtual int processComponentMessage( cComponentMessage *_msg );

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cWaveSinkCut(const char *_name);

    virtual ~cWaveSinkCut();
};




#endif // __CWAVESINKCUT_HPP
