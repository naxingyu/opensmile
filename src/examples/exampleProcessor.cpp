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

example dataProcessor
writes data to data memory...

** you can use this as a template for custom dataProcessor components **

*/


#include <examples/exampleProcessor.hpp>

#define MODULE "cExampleProcessor"

SMILECOMPONENT_STATICS(cExampleProcessor)

SMILECOMPONENT_REGCOMP(cExampleProcessor)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CEXAMPLEPROCESSOR;
  sdescription = COMPONENT_DESCRIPTION_CEXAMPLEPROCESSOR;

  // we inherit cDataProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("offset","The offset that this dummy component adds to the input values.",1.0);
  )
  
  SMILECOMPONENT_MAKEINFO(cExampleProcessor);
}


SMILECOMPONENT_CREATE(cExampleProcessor)

//-----

cExampleProcessor::cExampleProcessor(const char *_name) :
  cDataProcessor(_name)
{
}

void cExampleProcessor::fetchConfig()
{
  cDataProcessor::fetchConfig();

  // load all configuration parameters you will later require fast and easy access to here:

  offset = getDouble("offset");
  // note, that it is "polite" to output the loaded parameters at debug level 2:
  SMILE_DBG(2,"offset = %f",offset);
}

/*  This method is rarely used. It is only there to improve readability of component code.
    It is called from cDataProcessor::myFinaliseInstance just before the call to configureWriter.
    I.e. you can do everything that you would do here, also in configureWriter()
    However, if you implement the method, it must return 1 in order for the configure process to continue!
*/
/*
int cExampleProcessor::configureReader() 
{
  return 1;
}
*/

int cExampleProcessor::configureWriter(sDmLevelConfig &c) 
{
  // if you would like to change the write level parameters... do so HERE:

  c.T = 0.01; /* don't forget to set the write level sample/frame period */
  c.nT = 100; /* e.g. 100 frames buffersize for ringbuffer */

  return 1; /* success */
}

/* You shouldn't need to touch this....
int cExampleProcessor::myConfigureInstance()
{
  int ret = cDataProcessor::myConfigureInstance();
  return ret;
}
*/

/*
  Do what you like here... this is called after the input names and number of input elements have become available, 
  so you may use them here.
*/
/*
int cExampleProcessor::dataProcessorCustomFinalise()
{
  
  return 1;
}
*/


/* 
  Use setupNewNames() to freely set the data elements and their names in the output level
  The input names are available at this point, you can get them via reader->getFrameMeta()
  Please set "namesAreSet" to 1, when you do set names
*/
/*
int cExampleProcessor::setupNewNames(long nEl) 
{
  // namesAreSet = 1;
  return 1;
}
*/

/*
  If you don't use setupNewNames() you may set the names for each input field by overwriting the following method:
*/
/*
int cExampleProcessor::setupNamesForField( TODO )
{
  // DOC TODO...
}
*/

int cExampleProcessor::myFinaliseInstance()
{
  int ret = cDataProcessor::myFinaliseInstance();
  if (ret) {
    // do all custom init stuff here... 
    // e.g. allocating and initialising memory (which is not used before, e.g. in setupNames, etc.),
    // loading external data, 
    // etc.

    // ...

  }
  return ret;
}


int cExampleProcessor::myTick(long long t)
{
  /* actually process data... */

  SMILE_IDBG(4,"tick # %i, processing value vector",t);

  // get next frame from dataMemory
  cVector *vec = reader_->getNextFrame();

  // add offset
  int i;
  for (i=0; i<vec->N; i++) {
    vec->dataF[i] += (FLOAT_DMEM)offset;
  }

  // if you create a new vector here and pass it to setNextFrame(),
  // then be sure to assign a valid tmeta info for correct timing info:
  // e.g.:
  //   myVec->tmetaReplace(vec->tmeta);


  // save to dataMemory
  writer_->setNextFrame(vec);

  //   writer->setNextFrame(myVec);

  return 1;
}


cExampleProcessor::~cExampleProcessor()
{
  // cleanup...

}

