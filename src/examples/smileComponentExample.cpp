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

