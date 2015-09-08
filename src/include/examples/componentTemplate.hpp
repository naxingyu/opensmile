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


*/


#ifndef __MY_COMPONENT_HPP
#define __MY_COMPONENT_HPP

#include <core/smileCommon.hpp>
#include <core/smileComponent.hpp>

#define COMPONENT_DESCRIPTION_CMYCOMPONENT "a good template..."
#define COMPONENT_NAME_CMYCOMPONENT "cMyComponent"

#undef class
class DLLEXPORT cMyComponent : public cSmileComponent {
  private:

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

	virtual void fetchConfig();

  public:
    SMILECOMPONENT_STATIC_DECL

    //cMyComponent(cConfigManager *cm) : cSmileComponent("myComponentTemplate",cm) {}
    cMyComponent(const char *_name);

    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);
    virtual int manualConfig(); // custom function with arbirtrary parameters, use instead of fetchConfig when passing NULL for cConfigManager to constructor

    virtual ~cMyComponent() {}
};




#endif // __MY_COMPONENT_HPP
