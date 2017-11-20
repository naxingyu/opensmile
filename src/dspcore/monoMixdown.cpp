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

simple mixer, which adds multiple channels (elements) to a single channel (element)

*/


#include <dspcore/monoMixdown.hpp>

#define MODULE "cMonoMixdown"

SMILECOMPONENT_STATICS(cMonoMixdown)

SMILECOMPONENT_REGCOMP(cMonoMixdown)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CMONOMIXDOWN;
  sdescription = COMPONENT_DESCRIPTION_CMONOMIXDOWN;

  // we inherit cDataProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")
  
  SMILECOMPONENT_IFNOTREGAGAIN( 
    ct->setField("normalise","1/0 = yes/no : divide by the number of channels when adding samples from multiple channels.",1);
    ct->setField("bufsize","number of samples to process at once. Choose a number >> 1 for optimal performance. Too large buffer sizes may influence the latency!",1024);
  )
  
  SMILECOMPONENT_MAKEINFO(cMonoMixdown);
}


SMILECOMPONENT_CREATE(cMonoMixdown)

//-----

cMonoMixdown::cMonoMixdown(const char *_name) :
  cDataProcessor(_name), matout(NULL)
{
}

void cMonoMixdown::fetchConfig()
{
  cDataProcessor::fetchConfig();

  normalise = getInt("normalise");
  SMILE_IDBG(2,"normalise = %i",normalise);
  bufsize = getInt("bufsize");
  SMILE_IDBG(2,"bufsize = %i",bufsize);

}

int cMonoMixdown::configureWriter(sDmLevelConfig &c) 
{
  reader_->setupSequentialMatrixReading(bufsize,bufsize);
  return 1; /* success */
}


int cMonoMixdown::setupNamesForField(int i, const char *name, long nEl)
{
  // add a field with a single element for each input array field
  writer_->addField(name,1);
  return 1;
}


int cMonoMixdown::myFinaliseInstance()
{
  int ret = cDataProcessor::myFinaliseInstance();
  return ret;
}


int cMonoMixdown::myTick(long long t)
{
  /* actually process data... */

  // get next matrix from dataMemory
  cMatrix *mat = reader_->getNextMatrix();
  if (mat == NULL) return 0;

  if (matout == NULL) matout = new cMatrix(mat->fmeta->N, mat->nT, mat->type);

  // sum up channels
  long i,j,c;
  for (i=0; i<mat->nT; i++) {
    for (j=0; j<matout->N; j++) {
      matout->dataF[i*matout->N+j] = 0.0; long st = mat->fmeta->field[j].Nstart;
      for (c=0; c<mat->fmeta->field[j].N; c++) 
        matout->dataF[i*matout->N+j] += mat->dataF[i*mat->N+st+c];
      if ((mat->fmeta->field[j].N > 0)&&(normalise))
        matout->dataF[i*matout->N+j] /= (FLOAT_DMEM)(mat->fmeta->field[j].N);
    }
  }

  // if you create a new vector here and pass it to setNextFrame(),
  // then be sure to assign a valid tmeta info for correct timing info:
  // e.g.:
  matout->tmetaReplace(mat->tmeta);


  // save to dataMemory
  writer_->setNextMatrix(matout);

  return 1;
}


cMonoMixdown::~cMonoMixdown()
{
  // cleanup...
  if (matout != NULL) delete matout;
}

