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

simple mixer, which adds multiple channels (elements) to a single channel (element)

*/


#ifndef __CMONOMIXDOWN_HPP
#define __CMONOMIXDOWN_HPP

#include <core/smileCommon.hpp>
#include <core/dataProcessor.hpp>

#define COMPONENT_DESCRIPTION_CMONOMIXDOWN "This is a simple mixer, which adds multiple channels (elements) to a single channel (element)."
#define COMPONENT_NAME_CMONOMIXDOWN "cMonoMixdown"

#undef class
class DLLEXPORT cMonoMixdown : public cDataProcessor {
  private:
    int normalise;
    long bufsize;
    cMatrix *matout;
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    // virtual int dataProcessorCustomFinalise();
    // virtual int setupNewNames(long nEl);
    virtual int setupNamesForField(int i, const char*name, long nEl);
    virtual int configureWriter(sDmLevelConfig &c);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cMonoMixdown(const char *_name);

    virtual ~cMonoMixdown();
};




#endif // __CMONOMIXDOWN_HPP
