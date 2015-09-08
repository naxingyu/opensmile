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


/*  openSMILE component: cNullSink
----------------------------------
NULL sink: reads data vectors from data memory and destroys them without writing them anywhere
----------------------------------

11/2009  Written by Florian Eyben
*/


#include <core/nullSink.hpp>

#define MODULE "cNullSink"


SMILECOMPONENT_STATICS(cNullSink)

SMILECOMPONENT_REGCOMP(cNullSink)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CNULLSINK;
  sdescription = COMPONENT_DESCRIPTION_CNULLSINK;

  // we inherit cDataSink configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSink")
  
  SMILECOMPONENT_IFNOTREGAGAIN( {} )

  SMILECOMPONENT_MAKEINFO(cNullSink);
}

SMILECOMPONENT_CREATE(cNullSink)

//-----

cNullSink::cNullSink(const char *_name) :
  cDataSink(_name)
{
}

int cNullSink::myTick(long long t)
{
  SMILE_IDBG(4,"tick # %i, reading value vector to NULL sink",t);
  cVector *vec= reader_->getNextFrame();
  if (vec == NULL) return 0;
  else 
    // tick success
    return 1;
}


cNullSink::~cNullSink()
{
}

