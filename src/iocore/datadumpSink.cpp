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


/*  openSMILE component: datadumpSink

dump data in raw binary format (float)
the data can easily be loaded into matlab

The first float value will contain the vecsize
The second float value will contain the number of vectors
Then the matrix data follows float by float

*/



#include <iocore/datadumpSink.hpp>

#define MODULE "cDatadumpSink"

/*Library:
sComponentInfo * registerMe(cConfigManager *_confman) {
  cDataSink::registerComponent(_confman);
}
*/

SMILECOMPONENT_STATICS(cDatadumpSink)

//sComponentInfo * cDatadumpSink::registerComponent(cConfigManager *_confman)
SMILECOMPONENT_REGCOMP(cDatadumpSink)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CDATADUMPSINK;
  sdescription = COMPONENT_DESCRIPTION_CDATADUMPSINK;

  // we inherit cDataSink configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSink")

  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("filename","The filename of the output file (if it doesn't exist it will be created).","datadump.dat");
    ct->setField("lag","output data <lag> frames behind",0,0,0);
    ct->setField("append","1 = append to an existing file, or create a new file; 0 = overwrite an existing file, or create a new file",0);
  )

  SMILECOMPONENT_MAKEINFO(cDatadumpSink);
}

SMILECOMPONENT_CREATE(cDatadumpSink)

//-----

cDatadumpSink::cDatadumpSink(const char *_name) :
  cDataSink(_name),
  filehandle(NULL),
  filename(NULL),
  nVec(0),
  vecSize(0)
{
}

void cDatadumpSink::fetchConfig()
{
  cDataSink::fetchConfig();
  
  filename = getStr("filename");
  SMILE_IDBG(3,"filename = '%s'",filename);

  lag = getInt("lag");
  SMILE_IDBG(3,"lag = %i",lag);

  append = getInt("append");
  if (append) { SMILE_IDBG(3,"append to file is enabled"); }
}

/*
int cDatadumpSink::myConfigureInstance()
{
  int ret=1;
  ret *= cDataSink::myConfigureInstance();
  // ....
  //return ret;
}
*/


int cDatadumpSink::myFinaliseInstance()
{
  int ap=0;
  float tmp=0;
  
  int ret = cDataSink::myFinaliseInstance();
  if (ret==0) return 0;
  
  if (append) {
    // check if file exists:
    filehandle = fopen(filename, "rb");
    if (filehandle != NULL) {
      // load vecsize, to see if it matches!
      if (fread(&tmp,sizeof(float),1,filehandle)) vecSize=(long)tmp;
      else vecSize = 0;
      // load initial nVec
      if (fread(&tmp,sizeof(float),1,filehandle)) nVec=(long)tmp;
      else nVec = 0;
      fclose(filehandle);
      filehandle = fopen(filename, "ab");
      ap=1;
    } else {
      filehandle = fopen(filename, "wb");
    }
  } else {
    filehandle = fopen(filename, "wb");
  }
  if (filehandle == NULL) {
    COMP_ERR("Error opening binary file '%s' for writing (component instance '%s', type '%s')",filename, getInstName(), getTypeName());
  }
  
  if (vecSize == 0) vecSize = reader_->getLevelN();

  if (!ap) {
    // write mini dummy header ....
    writeHeader();
  }
  
  return ret;
}


int cDatadumpSink::myTick(long long t)
{
  if (filehandle == NULL) return 0;
  
  SMILE_DBG(4,"tick # %i, writing value vector (lag=%i):",t,lag);
  cVector *vec= reader_->getFrameRel(lag);  //new cVector(nValues+1);
  if (vec == NULL) return 0;

  // now print the vector:
  int i; float *tmp = (float*)malloc(sizeof(float)*vec->N);
  if (tmp==NULL) OUT_OF_MEMORY;
  
  if (vec->type == DMEM_FLOAT) {
    for (i=0; i<vec->N; i++) {
      tmp[i] = (float)(vec->dataF[i]);
    }
  } else if (vec->type == DMEM_INT) {
    for (i=0; i<vec->N; i++) {
      tmp[i] = (float)(vec->dataI[i]);
    }
  } else {
    SMILE_ERR(1,"unknown data type %i",vec->type);
    return 0;
  }

  int ret=1;
  if (!fwrite(tmp,sizeof(float),vec->N,filehandle)) {
    SMILE_ERR(1,"Error writing to raw feature file '%s'!",filename);
    ret = 0;
  } else {
    //reader->nextFrame();
    nVec++;
  }

  free(tmp);

  // tick success
  return ret;
}

// WARNING: write header changes file write pointer to beginning of file (after header)
void cDatadumpSink::writeHeader()
{
  // seek to beginning of file:
  fseek( filehandle, 0, SEEK_SET );
  // write header:
  float tmp;
  tmp = (float)vecSize;
  fwrite(&tmp, sizeof(float), 1, filehandle);
  tmp = (float)nVec;
  fwrite(&tmp, sizeof(float), 1, filehandle);
}

cDatadumpSink::~cDatadumpSink()
{
  // write final header 
  writeHeader();
  // close output file
  fclose(filehandle);
}

