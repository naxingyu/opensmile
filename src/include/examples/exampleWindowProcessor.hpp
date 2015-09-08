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

example of a windowProcessor component

*/


#ifndef __CEXAMPLEWINDOWPROCESSOR_HPP
#define __CEXAMPLEWINDOWPROCESSOR_HPP

#include <core/smileCommon.hpp>
#include <core/windowProcessor.hpp>

#define COMPONENT_DESCRIPTION_CEXAMPLEWINDOWPROCESSOR "This is an example of a cWindowProcessor descendant. It has no meaningful function, this component is intended as a template for developers."
#define COMPONENT_NAME_CEXAMPLEWINDOWPROCESSOR "cExampleWindowProcessor"

#undef class
class DLLEXPORT cExampleWindowProcessor : public cWindowProcessor {
  private:
    FLOAT_DMEM k;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR


    virtual void fetchConfig();
/*
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);
*/

    //virtual int configureWriter(const sDmLevelConfig *c);

   // buffer must include all (# order) past samples
    virtual int processBuffer(cMatrix *_in, cMatrix *_out, int _pre, int _post );
    
/*
    virtual int setupNamesForField(int i, const char*name, long nEl);
*/
    
  public:
    SMILECOMPONENT_STATIC_DECL
    
    cExampleWindowProcessor(const char *_name);

    virtual ~cExampleWindowProcessor();
};




#endif // __CEXAMPLEWINDOWPROCESSOR_HPP
