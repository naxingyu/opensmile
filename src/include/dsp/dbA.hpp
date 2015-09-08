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


/*  openSMILE component: dbA

applies dbX weighting to fft magnitudes

*/


#ifndef __CDBA_HPP
#define __CDBA_HPP

#include <core/smileCommon.hpp>
#include <core/vectorProcessor.hpp>
#include <math.h>

#define BUILD_COMPONENT_DbA
#define COMPONENT_DESCRIPTION_CDBA "This component performs dbX (dbA,dbB,dbC,...) equal loudness weighting of FFT bin magnitudes. Currently only dbA weighting is implemented."
#define COMPONENT_NAME_CDBA "cDbA"

#define CURVE_DBA 0
#define CURVE_DBB 1
#define CURVE_DBC 2

extern void computeDBA(FLOAT_DMEM *x, long blocksize, FLOAT_DMEM F0);

#undef class
class DLLEXPORT cDbA : public cVectorProcessor {
  private:
    int curve;
    int usePower;
    FLOAT_DMEM **filterCoeffs;
    
    int computeFilters( long blocksize, double frameSizeSec, int idxc );

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    //virtual int myTick(long long t);

    //virtual int configureWriter(const sDmLevelConfig *c);

    virtual int dataProcessorCustomFinalise();
    virtual int setupNamesForField(int i, const char*name, long nEl);
    //virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cDbA(const char *_name);

    virtual ~cDbA();
};




#endif // __CDBA_HPP
