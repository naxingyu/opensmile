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

very basic and low-level cSmileComponent template ...

*/


#include <examples/componentTemplate.hpp>

#define MODULE "myComponentTemplate"

/*Library:
sComponentInfo * registerMe(cConfigManager *_confman) {
  cMyComponent::registerComponent(_confman);
}
*/

SMILECOMPONENT_STATICS(cMyComponent)
                 // vvv!!!!! change cMyComponent to ..... !
SMILECOMPONENT_REGCOMP(cMyComponent)
//sComponentInfo * cMyComponent::registerComponent(cConfigManager *_confman)
{
  SMILECOMPONENT_REGCOMP_INIT
  cname_ = COMPONENT_NAME_CMYCOMPONENT;
  description_ = COMPONENT_DESCRIPTION_CMYCOMPONENT;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
 // OR: inherit a type, e.g. cDataSink:   SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSink")

  SMILECOMPONENT_IFNOTREGAGAIN_BEGIN
  
  ct->setField("f1", "this is an example int", 0);
  if (ct->setField("subconf", "this is config of sub-component",
                  sconfman->getTypeObj("nameOfSubCompType"), NO_ARRAY, DONT_FREE) == -1) {
     rA=1; // if subtype not yet found, request , re-register in the next iteration
  }

  SMILECOMPONENT_IFNOTREGAGAIN_END

    // vvv!!!!! change cMyComponent to ..... !
  SMILECOMPONENT_MAKEINFO(cMyComponent);
}


SMILECOMPONENT_CREATE(cMyComponent)

// NOTE: use this for abstract classes, e.g. dataSource, etc., which do NOT implement fetchConfig()
//SMILECOMPONENT_CREATE_ABSTRACT(cMyComponent)


// ----

// constructor:
cMyComponent::cMyComponent(const char *_name) : cSmileComponent(_name);
{

}

/*
virtual int cMyComponent::configureInstance()
{
  if (isConfigured()) return 1;
  
  // ....
}

virtual int cMyComponent::finaliseInstance()
{
  if (isFinalised()) return 1;
  
  // ....
}

virtual int tick(long long t)
{
  if (!isFinalised()) return 0;
  
  // ....
}
*/
