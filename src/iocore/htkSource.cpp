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


/*  openSMILE component: cHtkSource
-----------------------------------

HTK Source:

Reads data from an HTK parameter file.

-----------------------------------

11/2009 - Written by Florian Eyben
*/

#include <iocore/htkSource.hpp>
#define MODULE "cHtkSource"

/*Library:
sComponentInfo * registerMe(cConfigManager *_confman) {
  cDataSource::registerComponent(_confman);
}
*/
#define N_ALLOC_BLOCK 50

SMILECOMPONENT_STATICS(cHtkSource)

SMILECOMPONENT_REGCOMP(cHtkSource)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CHTKSOURCE;
  sdescription = COMPONENT_DESCRIPTION_CHTKSOURCE;

  // we inherit cDataSource configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSource")
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("filename","HTK parameter file to read","input.htk");
    ct->setField("featureName","The name of the array-field which is to be created in the data memory output level for the data array read from the HTK file","htkpara");
    ct->setField("featureFrameSize","The size of the feature frames in seconds.",0.0);
    ct->setField("forceSampleRate","Set a given sample rate for the output level. Typically the base period of the input level will be used for this purpose, but when reading frame-based data from feature files, for example, this information is not available. This option overwrites the input level base period, if it is set.",16000.0);
    ct->setField("blocksize", "The size of data blocks to write at once (to data memory) in frames", 10);
//    ct->setField("featureNames","array of feature names to apply (must match the vector size in the HTK parameter file!)","htkpara",ARRAY_TYPE);
  )

  SMILECOMPONENT_MAKEINFO(cHtkSource);
}

SMILECOMPONENT_CREATE(cHtkSource)

//-----

cHtkSource::cHtkSource(const char *_name) :
  cDataSource(_name),
  eof(0),
  featureName(NULL),
  tmpvec(NULL)
{
  vax = smileHtk_IsVAXOrder();
}

void cHtkSource::fetchConfig()
{
  cDataSource::fetchConfig();
  
  filename = getStr("filename");
  SMILE_IDBG(2,"filename = '%s'",filename);
  featureName = getStr("featureName");
  SMILE_IDBG(2,"featureName = '%s'",featureName);
}


int cHtkSource::myConfigureInstance()
{
  int ret = 1;
  filehandle = fopen(filename, "rb");
  if (filehandle == NULL) {
    COMP_ERR("Error opening file '%s' for reading (component instance '%s', type '%s')",filename, getInstName(), getTypeName());
  }

  if (!readHeader()) {
    ret = 0;
  } else {
    ret *= cDataSource::myConfigureInstance();
  }

  if (ret == 0) {
    fclose(filehandle); filehandle = NULL;
  }
  return ret;
}

/*
int cHtkSource::readHeader()
{
  if (filehandle==NULL) return 0;
  if (!fread(&head, sizeof(sHTKheader), 1, filehandle)) {
    SMILE_IERR(1,"error reading header from file '%s'",filename);
    return 0;
  }
  prepareHeader(&head); // convert to host byte order
  return 1;
}
*/

int cHtkSource::configureWriter(sDmLevelConfig &c)
{
  c.T = ( (double)head.samplePeriod ) * 0.000000100;  // convert HTK 100ns units..

  if (isSet("forceSampleRate")) {
    double sr = getDouble("forceSampleRate");
    if (sr > 0.0) {
      c.basePeriod = 1.0/sr;
    } else {
      c.basePeriod = 1.0;
      SMILE_IERR(1,"sample rate (forceSampleRate) must be > 0! (it is: %f)",sr);
    }
  }
  
  if (isSet("featureFrameSize")) {
    c.frameSizeSec = getDouble("featureFrameSize");
    c.lastFrameSizeSec = c.frameSizeSec;
  } 

  

  return 1;
}

int cHtkSource::setupNewNames(long nEl)
{
  N = head.sampleSize/sizeof(float);
  writer_->addField(featureName,N);

  allocVec(N);
  tmpvec = (float *)malloc(sizeof(float)*N);

  namesAreSet_=1;
  return 1;
}

int cHtkSource::myFinaliseInstance()
{
  int ret = cDataSource::myFinaliseInstance();
  return ret;
}


int cHtkSource::myTick(long long t)
{
  if (isEOI()) return 0;

  SMILE_IDBG(4,"tick # %i, reading value vector from HTK parameter file",t);
  if (eof) {
    SMILE_IDBG(4,"EOF, no more data to read");
    return 0;
  }

  long n;
  for (n=0; n<blocksizeW_; n++) {

    // check if there is enough space in the data memory
    if (!(writer_->checkWrite(1))) return 0;


    if (fread(tmpvec, head.sampleSize, 1, filehandle)) {
      long i;
      if (vax) {
        for (i=0; i<vec_->N; i++) {
          smileHtk_SwapFloat ( (tmpvec+i) );
          vec_->dataF[i] = (FLOAT_DMEM)tmpvec[i];
        }
      } else {
        for (i=0; i<vec_->N; i++) {
          vec_->dataF[i] = (FLOAT_DMEM)tmpvec[i];
        }
      }
    } else {
      // EOF ??
      eof = 1;
    } 


    if (!eof) {
      writer_->setNextFrame(vec_);
      return 1;
    } else {
      return 0;
    }
  }
  return 0;
}


cHtkSource::~cHtkSource()
{
  if (filehandle!=NULL) fclose(filehandle);
  if (tmpvec != NULL) free(tmpvec);
}
