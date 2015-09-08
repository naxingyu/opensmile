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

an example of a windowProcessor component

*/



#include <examples/exampleWindowProcessor.hpp>
//#include <math.h>

#define MODULE "cExampleWindowProcessor"


SMILECOMPONENT_STATICS(cExampleWindowProcessor)

SMILECOMPONENT_REGCOMP(cExampleWindowProcessor)
{
  if (_confman == NULL) return NULL;
  int rA = 0;

  sconfman = _confman;
  scname = COMPONENT_NAME_CEXAMPLEWINDOWPROCESSOR;
  sdescription = COMPONENT_DESCRIPTION_CEXAMPLEWINDOWPROCESSOR;

  // we inherit cWindowProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cWindowProcessor")

  // if the inherited config type was found, we register our configuration variables
  SMILECOMPONENT_IFNOTREGAGAIN( {} // <- this is only to avoid compiler warnings...
    // name append has a special role: it is defined in cDataProcessor, and can be overwritten here:
	// if you set description to NULL, the existing description will be used, thus the following call can
	// be used to update the default value:
    //ct->setField("nameAppend",NULL,"processed");
    // this is an example for adding an integer option:
	//ct->setField("inverse","1 = perform inverse FFT",0);
  )

  // The configType gets automatically registered with the config manger by the SMILECOMPONENT_IFNOTREGAGAIN macro
  
  // we now create out sComponentInfo, including name, description, success status, etc. and return that
  SMILECOMPONENT_MAKEINFO(cExampleWindowProcessor);
}

SMILECOMPONENT_CREATE(cExampleWindowProcessor)

//-----

cExampleWindowProcessor::cExampleWindowProcessor(const char *_name) :
  cWindowProcessor(_name,1,0)
{
}


void cExampleWindowProcessor::fetchConfig()
{
  cWindowProcessor::fetchConfig();
  
  k = (FLOAT_DMEM)getDouble("k");
  SMILE_DBG(2,"k = %f",k);
  if ((k<0.0)||(k>1.0)) {
    SMILE_ERR(1,"k must be in the range [0;1]! Setting k=0.0 !");
    k=0.0;
  }
}

// order is the amount of memory (overlap) that will be present in _in
// buf will have nT timesteps, however also order negative indicies (i.e. you may access a negative array index up to -order!)
int cExampleWindowProcessor::processBuffer(cMatrix *_in, cMatrix *_out, int _pre, int _post )
{
  long n;

  if (_in->type!=DMEM_FLOAT) COMP_ERR("dataType (%i) != DMEM_FLOAT not yet supported!",_in->type);
  FLOAT_DMEM *x=_in->dataF;
  FLOAT_DMEM *y=_out->dataF;
  for (n=0; n<_out->nT; n++) {
    *(y++) = *(x) - k * *(x-1);
    x++;
  }
  return 1;
}


cExampleWindowProcessor::~cExampleWindowProcessor()
{
}

