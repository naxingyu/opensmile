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

pre-emphasis per frame  (simplification, however, this is the way HTK does it... so for compatibility... here you go)
(use before window function is applied!)

*/


#ifndef __CVECTORPREEMPHASIS_HPP
#define __CVECTORPREEMPHASIS_HPP

#include <core/smileCommon.hpp>
#include <core/vectorProcessor.hpp>

#define COMPONENT_DESCRIPTION_CVECTORPREEMPHASIS "This component performs per frame pre-emphasis without an inter-frame state memory. (This is the way HTK does pre-emphasis). Pre-emphasis: y(t) = x(t) - k*x(t-1) ; de-emphasis : y(t) = x(t) + k*x(t-1)"
#define COMPONENT_NAME_CVECTORPREEMPHASIS "cVectorPreemphasis"

#undef class
class DLLEXPORT cVectorPreemphasis : public cVectorProcessor {
  private:
    FLOAT_DMEM k;
    double f;
    int de;
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    //virtual int myTick(long long t);

    //virtual int configureWriter(const sDmLevelConfig *c);

    virtual int dataProcessorCustomFinalise();
//    virtual int setupNamesForField(int i, const char*name, long nEl);
    virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cVectorPreemphasis(const char *_name);

    virtual ~cVectorPreemphasis();
};




#endif // __CVECTORPREEMPHASIS_HPP
