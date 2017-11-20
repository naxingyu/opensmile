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
 *            Martin Woellmer, Bjoern Schuller
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

example dataSink
writes data to data memory...

*/


#include <plug_exampleSink.hpp>

#define MODULE "cExampleSinkPlugin"


SMILECOMPONENT_STATICS(cExampleSinkPlugin)

SMILECOMPONENT_REGCOMP(cExampleSinkPlugin)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CEXAMPLESINK;
  sdescription = COMPONENT_DESCRIPTION_CEXAMPLESINK;

  // we inherit cDataSink configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSink")
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("filename","file to dump values to","dump.txt");
    ct->setField("lag","output data <lag> frames behind",0);
  )
  SMILECOMPONENT_MAKEINFO(cExampleSinkPlugin);
}

SMILECOMPONENT_CREATE(cExampleSinkPlugin)

//-----

cExampleSinkPlugin::cExampleSinkPlugin(const char *_name) :
  cDataSink(_name)
{
  // ...
printf("I'm here...\n");
}

void cExampleSinkPlugin::fetchConfig()
{
  cDataSink::fetchConfig();
printf("I'm here in fetchConfig...\n");
  
  filename = getStr("filename");
printf("filename=%s\n",filename);
printf("log=%i\n",(long)&smileLog);
smileLog.message(FMT("test"), 2, "cYEAH");

  SMILE_DBG(2,"filename = '%s'",filename);
  lag = getInt("lag");
  SMILE_DBG(2,"lag = %i",lag);
}

/*
int cExampleSink::myConfigureInstance()
{
  int ret=1;
  ret *= cDataSink::myConfigureInstance();
  // ....
  //return ret;
}
*/

/*
int cExampleSink::myFinaliseInstance()
{
  int ret=1;
  ret *= cDataSink::myFinaliseInstance();
  // ....
  //return ret;
}
*/

int cExampleSinkPlugin::myTick(long long t)
{
  SMILE_DBG(4,"tick # %i, reading value vector:",t);
  cVector *vec= reader->getFrameRel(lag);  //new cVector(nValues+1);
  if (vec == NULL) return 0;
  else reader->nextFrame();

  long vi = vec->tmeta->vIdx;
  double tm = vec->tmeta->time;
  
  // now print the vector:
  SMILE_IMSG(2,"FUCKING AWSOME PLUGIN OUTPUT:");
  int i;
  for (i=0; i<vec->N; i++) {
    printf("  (a=%i vi=%i, tm=%fs) %s.%s = %f\n",reader->getCurR(),vi,tm,reader->getLevelName(),vec->name(i),vec->dataF[i]);
  }

// SMILE_PRINT("%i",var1,)


  // tick success
  return 1;
}


cExampleSinkPlugin::~cExampleSinkPlugin()
{
  // ...
}

