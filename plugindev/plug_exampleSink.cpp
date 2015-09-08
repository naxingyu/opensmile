/*F***************************************************************************
 * openSMILE - the Munich open source Multimedia Interpretation by Large-scale
 * Extraction toolkit
 * 
 * (c) 2008-2011, Florian Eyben, Martin Woellmer, Bjoern Schuller @ TUM-MMK
 * (c) 2012-2013, Florian Eyben, Felix Weninger, Bjoern Schuller @ TUM-MMK (c)
 * 2013-2014 audEERING UG, haftungsbeschränkt. All rights reserved.
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
  cVector *vec= reader_->getFrameRel(lag);  //new cVector(nValues+1);
  if (vec == NULL) return 0;
  else reader_->nextFrame();

  long vi = vec->tmeta->vIdx;
  double tm = vec->tmeta->time;
  
  // now print the vector:
  SMILE_IMSG(2,"FUCKING AWSOME PLUGIN OUTPUT:");
  int i;
  for (i=0; i<vec->N; i++) {
    printf("  (a=%i vi=%i, tm=%fs) %s.%s = %f\n",reader_->getCurR(),vi,tm,reader_->getLevelName(),vec->name(i),vec->dataF[i]);
  }

// SMILE_PRINT("%i",var1,)


  // tick success
  return 1;
}


cExampleSinkPlugin::~cExampleSinkPlugin()
{
  // ...
}

