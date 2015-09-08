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

example for a cSmileComponent descendant

*/


#ifndef __CSMILECOMPONENTEXAMPLE_HPP
#define __CSMILECOMPONENTEXAMPLE_HPP

#include <core/smileCommon.hpp>
#include <core/smileComponent.hpp>

#define COMPONENT_DESCRIPTION_CSMILECOMPONENTEXAMPLE "example for a cSmileComponent descendant"
#define COMPONENT_NAME_CSMILECOMPONENTEXAMPLE "cSmileComponentExample"


#undef class
class DLLEXPORT cSmileComponentExample : public cSmileComponent {
  private:

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    virtual void mySetEnvironment();
    virtual int myRegisterInstance();
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);
    virtual int processComponentMessage( cComponentMessage *_msg );

  public:
    SMILECOMPONENT_STATIC_DECL

    cSmileComponentExample(const char *_name);
    virtual ~cSmileComponentExample();
};




#endif // __CSMILECOMPONENTEXAMPLE_HPP
