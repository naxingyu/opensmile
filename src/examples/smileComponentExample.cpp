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

example for a direct descendant of cSmileComponent

no dataMemory interface functionality is used (thus, the tick method may be ignored)

the component may only process messages, if that is desired

*/


#include <examples/smileComponentExample.hpp>

#define MODULE "cSmileComponentExample"

SMILECOMPONENT_STATICS(cSmileComponentExample)

SMILECOMPONENT_REGCOMP(cSmileComponentExample)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CSMILECOMPONENTEXAMPLE;
  sdescription = COMPONENT_DESCRIPTION_CSMILECOMPONENTEXAMPLE;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
  //ct->setField("reader", "dataMemory interface configuration (i.e. what slot to read from)", sconfman->getTypeObj("cDataReader"), NO_ARRAY, DONT_FREE);
  SMILECOMPONENT_IFNOTREGAGAIN( {} )

  SMILECOMPONENT_MAKEINFO(cSmileComponentExample);
}

SMILECOMPONENT_CREATE(cSmileComponentExample)



//-----

cSmileComponentExample::cSmileComponentExample(const char *_name) :
  cSmileComponent(_name)
{
  // initialisation code...

}

void cSmileComponentExample::fetchConfig() 
{

}

void cSmileComponentExample::mySetEnvironment()
{

}

int cSmileComponentExample::myRegisterInstance()
{

  return 1;
}

int cSmileComponentExample::myConfigureInstance()
{

  return 1;
}

int cSmileComponentExample::myFinaliseInstance()
{

  return 1;
}

int cSmileComponentExample::processComponentMessage( cComponentMessage *_msg ) 
{ 
  if (isMessageType(_msg,"myXYZmessage")) {  // see doc/messages.txt for documentation of currently available messages
   
    // return 1;  // if message was processed
  }
  return 0; // if message was not processed
}  

int cSmileComponentExample::myTick(long long t) 
{

  // return 1;  // tick did succeed!

  return 0; // tick did not succeed, i.e. nothing was processed or there was nothing to process
}

cSmileComponentExample::~cSmileComponentExample()
{
  // cleanup code...

}

