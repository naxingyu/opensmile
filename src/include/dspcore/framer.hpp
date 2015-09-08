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

dataFramer

*/


#ifndef __CFRAMER_HPP
#define __CFRAMER_HPP

#include <core/smileCommon.hpp>
#include <core/winToVecProcessor.hpp>

#define COMPONENT_DESCRIPTION_CFRAMER "This component creates frames from single dimensional input stream. It is possible to specify the frame step and frame size independently, thus allowing for overlapping frames or non continuous frames."
#define COMPONENT_NAME_CFRAMER "cFramer"

#undef class
class DLLEXPORT cFramer : public cWinToVecProcessor {
  private:

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    //virtual void fetchConfig();
    //virtual int myFinaliseInstance();
    //virtual int myTick(long long t);

    virtual int getMultiplier();
    //virtual int configureWriter(const sDmLevelConfig *c);
    //virtual int setupNamesForField(int idxi, const char*name, long nEl);
    virtual int doProcess(int i, cMatrix *row, FLOAT_DMEM*x);
    virtual int doProcess(int i, cMatrix *row, INT_DMEM*x);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cFramer(const char *_name);

    virtual ~cFramer();
};




#endif // __CFRAMER_HPP
