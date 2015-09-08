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

concatenates vectors from multiple levels and copy to another level

*/


#include <other/vectorConcat.hpp>

#define MODULE "cVectorConcat"

SMILECOMPONENT_STATICS(cVectorConcat)

SMILECOMPONENT_REGCOMP(cVectorConcat)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CVECTORCONCAT;
  sdescription = COMPONENT_DESCRIPTION_CVECTORCONCAT;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")
  
    SMILECOMPONENT_IFNOTREGAGAIN( {}
    //ct->setField("expandFields", "expand fields to single elements, i.e. each field in the output will correspond to exactly one element in the input [not yet implemented]", 0);
  )

  SMILECOMPONENT_MAKEINFO(cVectorConcat);
}

SMILECOMPONENT_CREATE(cVectorConcat)

//-----

cVectorConcat::cVectorConcat(const char *_name) :
  cVectorProcessor(_name)
{
}


int cVectorConcat::processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  if (dst!=src)
    memcpy( dst, src,  MIN(Ndst,Nsrc)*sizeof(INT_DMEM) );
  return 1;
}

int cVectorConcat::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  if (dst!=src)
    memcpy( dst, src,  MIN(Ndst,Nsrc)*sizeof(FLOAT_DMEM) );
  return 1;
}

cVectorConcat::~cVectorConcat()
{
}

