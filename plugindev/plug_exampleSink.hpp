/*F***************************************************************************
 * openSMILE - the Munich open source Multimedia Interpretation by Large-scale
 * Extraction toolkit
 * 
 * (c) 2008-2011, Florian Eyben, Martin Woellmer, Bjoern Schuller @ TUM-MMK
 * (c) 2012-2013, Florian Eyben, Felix Weninger, Bjoern Schuller @ TUM-MMK (c)
 * 2013-2014 audEERING UG, haftungsbeschränkt. All rights reserved.
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

example plugin: data sink example

*/


#ifndef __EXAMPLE_SINK_HPP
#define __EXAMPLE_SINK_HPP

#include <smileCommon.hpp>
#include <smileComponent.hpp>
#include <dataSink.hpp>

#define COMPONENT_DESCRIPTION_CEXAMPLESINK "dataSink example, read data and prints it to screen"
#define COMPONENT_NAME_CEXAMPLESINK "cExampleSinkPlugin"

// add this for plugins to compile properly in MSVC++
#ifdef SMILEPLUGIN
#ifdef class
#undef class
#endif
#endif

class cExampleSinkPlugin : public cDataSink {
  private:
/*    static cConfigManager *sconfman;
    static const char *cname;  // name of component object type
    static const char *description;*/

    const char *filename;
    int lag;
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    virtual int myTick(long long t);


  public:
    //static sComponentInfo * registerComponent(cConfigManager *_confman);
    //static cSmileComponent * create(const char *_instname);
    SMILECOMPONENT_STATIC_DECL
    
    cExampleSinkPlugin(const char *_name);

    virtual ~cExampleSinkPlugin();
};




#endif // __EXAMPLE_SINK_HPP
