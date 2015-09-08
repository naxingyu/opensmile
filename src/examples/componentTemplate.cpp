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
