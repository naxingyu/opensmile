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

waveSource : reads PCM WAVE files (RIFF format)

Support for a negative start index was added by Benedikt Gollan (TUM).


TODO:
multi filename support (EOI signalling after every filename? Or turnFrameTimeMessage? make it configurable!)
segmentList support ()  (EOI signalling or frametime message after each segment? make it configurable!)
*/


#include <iocore/waveSource.hpp>
#define MODULE "cWaveSource"

SMILECOMPONENT_STATICS(cWaveSource)

SMILECOMPONENT_REGCOMP(cWaveSource)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CWAVESOURCE;
  sdescription = COMPONENT_DESCRIPTION_CWAVESOURCE;

  // we inherit cDataSource configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSource")
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->makeMandatory(ct->setField("filename","The filename of the PCM wave file to load. Only uncompressed RIFF files are supported. Use a suitable converter (mplayer, for example) to convert other formats to wave.","input.wav"));
    ct->setField("properTimestamps", "1 = set smileTime for every input sample based on sample time (a bit inefficient); otherwise the default is to use the current system time at which the samples are read from the file.", 0);
    ct->setField("monoMixdown","Mix down all channels to 1 mono channel (1=on, 0=off)",1);
    ct->setField("segmentList", "List of segments with start end times. (NOT IMPLEMENTED YET).", (const char*)NULL, ARRAY_TYPE);
    ct->setField("start","The read start point in seconds from the beginning of the file",0.0);
    ct->setField("end","The read end point in seconds from the beginning of file (-1 = read to EoF)",-1.0);
    ct->setField("endrel","The read end point in seconds from the END of file (only if 'end' = -1, or not set)",0.0);
    ct->setField("startSamples","The read start in samples from the beginning of the file (this overwrites 'start')",0,0,0);
    ct->setField("endSamples","The read end in samples from the beginning of the file (this overwrites 'end' and 'endrelSamples')",-1,0,0);
    ct->setField("endrelSamples","The read end in samples from the END of file (this overwrites 'endrel')",0,0,0);
    ct->setField("noHeader","1 = treat the input file as 'raw' format, i.e. don't read the RIFF header. You must specify the parameters 'sampleRate', 'channels', and possibly 'sampleSize' if the defaults don't match your file format",0);
    ct->setField("sampleRate","Set/force the sampling rate that is assigned to the input data (required for reading raw files)",16000,0,0);
    ct->setField("sampleSize","Set/force the samplesize (in bytes) (required for reading raw files)",2,0,0);
    ct->setField("channels","Set/force the number of channels (required for reading raw files)",1,0,0);
    ct->setField("outFieldName", "Set the name of the output field, containing the pcm data", "pcm");
    // overwrite cDataSource's default:
    ct->setField("blocksize_sec", NULL , 1.0);
    // TODO: check if this default will be used....!!
  )

  SMILECOMPONENT_MAKEINFO(cWaveSource);
}

SMILECOMPONENT_CREATE(cWaveSource)

//-----

cWaveSource::cWaveSource(const char *_name) :
  cDataSource(_name),
  filehandle(NULL),
  filename(NULL),
  //buffersize(2000),
  pcmDataBegin(0),
  curReadPos(0),
  eof(0),
  monoMixdown(0),
  properTimestamps_(0)
{
  // ...
}

void cWaveSource::fetchConfig()
{
  cDataSource::fetchConfig();
  
  filename = getStr("filename");
  SMILE_IDBG(2,"filename = '%s'",filename);
  if (filename == NULL) COMP_ERR("fetchConfig: getStr(filename) returned NULL! missing option in config file?");

  /*
  buffersize = getInt("buffersize");
  if (buffersize < 1) buffersize=1;
  SMILE_DBG(2,"buffersize = %i",buffersize);
*/

  monoMixdown = getInt("monoMixdown");
  if (monoMixdown) { SMILE_IDBG(2,"monoMixdown enabled!"); }

  start = getDouble("start");
  endrel = getDouble("endrel");
  end = getDouble("end");

  outFieldName = getStr("outFieldName");

  if(outFieldName == NULL) COMP_ERR("fetchConfig: getStr(outFieldName) returned NULL! missing option in config file?");

  properTimestamps_ = getInt("properTimestamps");
}

int cWaveSource::configureWriter(sDmLevelConfig &c)
{
  negativestart = 0;
  int ret = 0;

  int noHeader = getInt("noHeader");

  if (!noHeader) { 
    ret = readWaveHeader();
    if (ret == 0) COMP_ERR("failed reading wave header from file '%s'! Maybe this is not a WAVE file?",filename);
  } else { 
    fseek( filehandle, 0, SEEK_END );
    size_t filesize = ftell(filehandle);
    rewind(filehandle);

    pcmParam.sampleRate = getInt("sampleRate");
    pcmParam.nChan = getInt("channels");
    pcmParam.nBPS = getInt("sampleSize");
    pcmParam.nBits = pcmParam.nBPS*8;
    pcmParam.blockSize = pcmParam.nChan * pcmParam.nBPS;
    pcmParam.nBlocks = (long)(filesize / pcmParam.blockSize);
    curReadPos = 0;
    ret = 1;
  }

  double srate = (double)(pcmParam.sampleRate);
  if (srate==0.0) srate = 1.0;
  long flen  = (long)(pcmParam.nBlocks); // file length in samples ///XXXX TODO!

  if (isSet("startSamples")) {
    startSamples = getInt("startSamples");
  } else {
    SMILE_DBG(2,"start = %f",start);
    startSamples = (long)floor(start * srate);
  }
  if (startSamples < 0)
  {
	  negativestart = 1;
	  negstartoffset = -startSamples;
  }
  if (startSamples > flen) startSamples = flen;///XXXX TODO!
  SMILE_DBG(2,"startSamples = %i",startSamples);

  if (isSet("endSamples")) {
    endSamples = getInt("endSamples");
  } else {
    if (end < 0.0) endSamples = -1; ///XXXX TODO!
    else endSamples = (long)ceil(end * srate); ///XXXX TODO!
  }

  if (endSamples < 0) {
    if (isSet("endrelSamples")) {
      endrelSamples = getInt("endrelSamples");
      if (endrelSamples < 0) endrelSamples = 0;
      SMILE_DBG(2,"endrelSamples = %i",endrelSamples);
      endSamples = flen-endrelSamples; ///XXXX TODO!
      if (endSamples < 0) endSamples = 0;
    } else {
      if (isSet("endrel")) {
        endSamples = flen - (long)floor(endrel * srate); ///XXXX TODO!
        if (endSamples < 0) endSamples = 0;
      } else {
        endSamples = flen; ///XXXX TODO!
      }
    }
  }
  if (endSamples > flen) endSamples = flen; ///XXXX TODO!
  SMILE_DBG(2,"endSamples = %i",endSamples);

  if (startSamples > 0) { // seek to start pos!
    curReadPos = startSamples;
    fseek( filehandle, curReadPos*pcmParam.blockSize, SEEK_CUR ); /* must be: SEEK_CUR ! */
  }

  // more segments
  //if (getArraySize("start")) {


  //}


    // TODO:: AUTO buffersize.. maximum length of wave data to store (depends on config of windower components)
    // so.. we CAN configure windower components first, which then set a config option in the wave source config..?
    //    OR central config unit apart from the configManager? (only for internal config?)
    // the windower comps. must set the buffersize info during their fetchCOnfig phase (i.e. BEFORE configure instance phase)
  c.T = 1.0 / (double)(pcmParam.sampleRate);
  //writer->setConfig(1,2*buffersize,T, 0.0, 0.0 , 0, DMEM_FLOAT);  // lenSec = (double)(2*buffersize)*T
  //c.basePeriod = c.T; // <- this is handled by dataSource already
  // TODO : blocksize...

  return 1;
}

int cWaveSource::myConfigureInstance()
{
  // open wave file.... etc. get header, etc.
  if (filehandle == NULL) {
    filehandle = fopen(filename, "rb");
    if (filehandle == NULL) COMP_ERR("failed to open input file '%s'",filename);
  }

  int ret = cDataSource::myConfigureInstance();
  
  if (!ret) {
    fclose(filehandle); filehandle = NULL;
  }
  return ret;
}

int cWaveSource::setupNewNames(long nEl) 
{
  // configure dataMemory level, names, etc.
  if (monoMixdown) {
    writer_->addField(outFieldName,1);
    //allocMat(1, blocksizeW_);
  } else {
    writer_->addField(outFieldName,pcmParam.nChan);
    //allocMat(pcmParam.nChan, blocksizeW_);
  }

  namesAreSet_ = 1;
  return 1;
}

/*
int cWaveSource::myFinaliseInstance()
{
 
  return cDataSource::myFinaliseInstance();
}
*/

int cWaveSource::myTick(long long t)
{
  if (isEOI()) {
    // Check if we successfully reached the end of the current segment,
    // or if processing got interrupted at a different point.
    // If so, give an error:
    if (!eof) {
      SMILE_IERR(1, "Processing aborted before all data was read from the input wave file! There must be something wrong with your config, e.g. a dataReader blocking a dataMemory level. Look for level full error messages in the debug mode output!");
    }
    return 0;
  }
  if (mat_ == NULL) {
    if (monoMixdown) allocMat(1, blocksizeW_);
    else allocMat(pcmParam.nChan, blocksizeW_);
  }
  while (negativestart == 1) {
    if (negstartoffset >= blocksizeW_) {
      if (writer_->checkWrite(negstartoffset)) {
        writer_->setNextMatrix(mat_);
      }
      negstartoffset = negstartoffset - blocksizeW_;
      SMILE_IMSG(1, "Negative Start Offset: = %f", negstartoffset);
    }
    if (negstartoffset > 0 && negstartoffset < blocksizeW_) {
      cMatrix *matout = new cMatrix(mat_->N, negstartoffset, mat_->type);
      if (writer_->checkWrite(negstartoffset)) {
        writer_->setNextMatrix(matout);
      }
      negstartoffset = negstartoffset - blocksizeW_;
      SMILE_IMSG(1, "Negative Start Offset: = %f", negstartoffset);
      negativestart = 0;
    }
    if (negstartoffset == 0) negativestart = 0;
  }
  // TODO: check if there is space in dmLevel for this write...!
  if (writer_->checkWrite(blocksizeW_)) {
    if (readData()) { // read new data from wave file!
      /**/
      if (properTimestamps_) {
        // NOTE: this is experimental, seems to work, but is inefficient.
        // This has to be replaced by the new "simple" layers.
        for (long i = 0; i < mat_->nT; i++) { // <<-- new timestamp assignment, untested!!
          mat_->tmeta[i].smileTime = (double)(curReadPos - mat_->nT + i) 
                                     / (double)pcmParam.sampleRate;
        }/**/
      }
      if (!writer_->setNextMatrix(mat_)) { // save data in dataMemory buffers
        SMILE_IERR(1, "can't write, level full... (strange, level space was checked using checkWrite(bs=%i))", blocksizeW_);
      } else {
        return 1;
      }
    }
  }
  return 0;
}


cWaveSource::~cWaveSource()
{
  if (filehandle != NULL) fclose(filehandle);
}

//--------------------------------------------------  wave specific

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif


// reads data into matrix m, size is determined by m, also performs format conversion to float samples and matrix format
int cWaveSource::readData(cMatrix *m)
{
  if (eof) {
    SMILE_WRN(6,"not reading from file, already EOF");
    return 0;
  }
  
  if (m==NULL) {
    if (mat_ == NULL) {
      if (monoMixdown) allocMat(1, blocksizeW_);
      else allocMat(pcmParam.nChan, blocksizeW_);
    }
    m=mat_;
  }
  if ( (m->N != pcmParam.nChan)&&((!monoMixdown) && (m->N == 1)) ) {
    SMILE_ERR(1,"readData: incompatible read! nChan=%i <-> matrix N=%i (these numbers must match!)\n",pcmParam.nChan,m->N);
    return 0;
  }

  long samplesToRead = m->nT;
  if (endSamples - curReadPos < blocksizeW_) {
    samplesToRead = (endSamples - curReadPos);
  }

  long nChan = 1;
  if (!monoMixdown) {
    nChan = pcmParam.nChan;
  }
  // check for float data type
  // if they match, convert with smilePcm_readSamples();
  long nRead = 0;
#if FLOAT_DMEM_NUM == FLOAT_DMEM_FLOAT
  nRead = smilePcm_readSamples(&filehandle, &pcmParam, m->dataF, nChan, samplesToRead, monoMixdown);
#else
  // TODO: allocate only once, put variable in class object
  float * a = (float *)malloc(sizeof(float) * m->nT);
  nRead = smilePcm_readSamples(&filehandle, &pcmParam, a, nChan, samplesToRead, monoMixdown);
  // convert to matrix
  for (long i = 0; i < nRead && i < m->nT; i++) {
    m->dataF[i] = (FLOAT_DMEM)a[i];
  }
  free(a);
#endif
  if (nRead != blocksizeW_ || nRead < 0) {
    SMILE_IWRN(5,"nRead (%i) < size to read (%i) ==> assuming EOF!", nRead, blocksizeW_);
    eof = 1;
    if (nRead >= 0) {
      m->nT = nRead;
    } else {
      m->nT = 0;
    }
  }
  if (nRead > 0) {
    curReadPos += nRead;
  }
  return (nRead > 0);
}

int cWaveSource::readWaveHeader()
{
  return smilePcm_readWaveHeader(filehandle, &pcmParam, filename);
}

