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

data framer
 
*/


#include <dspcore/framer.hpp>
//#include <math.h>

#define MODULE "cFramer"


SMILECOMPONENT_STATICS(cFramer)

SMILECOMPONENT_REGCOMP(cFramer)
{
  SMILECOMPONENT_REGCOMP_INIT


  scname = COMPONENT_NAME_CFRAMER;
  sdescription = COMPONENT_DESCRIPTION_CFRAMER;

  // we inherit cWinToVecProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cWinToVecProcessor")
  
  SMILECOMPONENT_IFNOTREGAGAIN( {} 

  )
  
  SMILECOMPONENT_MAKEINFO(cFramer);
}

SMILECOMPONENT_CREATE(cFramer)

//-----

cFramer::cFramer(const char *_name) :
  cWinToVecProcessor(_name)
{

}


// this must return the multiplier, i.e. the vector size returned for each input element (e.g. number of functionals, etc.)
int cFramer::getMultiplier()
{
  return frameSizeFrames;
}

// idxi is index of input element
// row is the input row
// y is the output vector (part) for the input row
int cFramer::doProcess(int idxi, cMatrix *row, FLOAT_DMEM*y)
{
  // copy row to matrix... simple memcpy here
  memcpy(y,row->dataF,row->nT*sizeof(FLOAT_DMEM));
  // return the number of components in y!!
  return row->nT;
}
int cFramer::doProcess(int idxi, cMatrix *row, INT_DMEM*y)
{
  // copy row to matrix... simple memcpy here
  memcpy(y,row->dataI,row->nT*sizeof(INT_DMEM));
  // return the number of components in y!!
  return row->nT;
}


cFramer::~cFramer()
{
}

