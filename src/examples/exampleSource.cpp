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

example dataSource
writes data to data memory...

*/


#include <examples/exampleSource.hpp>
#define MODULE "cExampleSource"

SMILECOMPONENT_STATICS(cExampleSource)

SMILECOMPONENT_REGCOMP(cExampleSource)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CEXAMPLESOURCE;
  sdescription = COMPONENT_DESCRIPTION_CEXAMPLESOURCE;

  // we inherit cDataSource configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSource")
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("nValues","The number of random values to generate",1);
    ct->setField("randSeed","The random seed",1.0);
  )

  SMILECOMPONENT_MAKEINFO(cExampleSource);
}

SMILECOMPONENT_CREATE(cExampleSource)

//-----

cExampleSource::cExampleSource(const char *_name) :
  cDataSource(_name)
{
  // ...
}

void cExampleSource::fetchConfig()
{
  cDataSource::fetchConfig();
  
  nValues = getInt("nValues");
  SMILE_DBG(2,"nValues = %i",nValues);
  randSeed = getDouble("randSeed");
  SMILE_DBG(2,"nValues = %f",randSeed);
}

/*
int cExampleSource::myConfigureInstance()
{
  int ret = cDataSource::myConfigureInstance();
  // ...

  return ret;
}
*/

int cExampleSource::configureWriter(sDmLevelConfig &c)
{
  // configure your writer by setting values in &c

  return 1;
}

// NOTE: nEl is always 0 for dataSources....
int cExampleSource::setupNewNames(long nEl)
{
  // configure dateMemory level, names, etc.
  writer_->addField("randVal",nValues);
  writer_->addField("const");
  // ...

  allocVec(nValues+1);
  return 1;
}

/*
int cExampleSource::myFinaliseInstance()
{
  return cDataSource::myFinaliseInstance();
}
*/

int cExampleSource::myTick(long long t)
{
  SMILE_DBG(4,"tick # %i, writing value vector",t);

  // todo: fill with random values...
  vec_->dataF[0] = (FLOAT_DMEM)t+(FLOAT_DMEM)3.3;
  writer_->setNextFrame(vec_);
  
  return 1;
}


cExampleSource::~cExampleSource()
{
  // ..
}
