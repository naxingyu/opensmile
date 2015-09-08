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

This component extends the base class cVectorTransform and implements mean/variance normalisation

*/


#ifndef __CVECTORMVN_HPP
#define __CVECTORMVN_HPP

#include <core/smileCommon.hpp>
#include <core/vectorTransform.hpp>
#include <math.h>

#define COMPONENT_DESCRIPTION_CVECTORMVN "This component extends the base class cVectorTransform and implements mean/variance normalisation. You can use this component to perform on-line cepstral mean normalisation. See cFullinputMean for off-line cepstral mean normalisation."
#define COMPONENT_NAME_CVECTORMVN "cVectorMVN"

/* we define some transform type IDs, other will be defined in child classes */
//#define TRFTYPE_CMN     10    /* mean normalisation, mean vector only */
//#define TRFTYPE_MVN     20    /* mean variance normalisation, mean vector + stddev vector */
//#define TRFTYPE_UNDEF    0    /* undefined, or custom type */

#define STDDEV_FLOOR  0.0000001


#undef class
class DLLEXPORT cVectorMVN : public cVectorTransform {
  private:
    FLOAT_DMEM specFloor;
    int spectralFlooring, subtractMeans;
    int meanEnable;
    int stdEnable, normEnable;
    int minMaxNormEnable;
    int htkLogEnorm;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
      
    /**** refactoring hooks ****/
    /* allocate the memory for vectors and userData, initialise header
       the only field that has been set is head.vecSize ! 
       This function is only called in modes analysis and incremental if no data was loaded */
    virtual void allocTransformData(struct sTfData *tf, int Ndst, int idxi);
    /* For mode == ANALYSIS or TRANSFORMATION, this functions allows for a final processing step
       at the end of input and just before the transformation data is saved to file */
    //virtual void computeFinalTransformData(struct sTfData *tf, int idxi) {}

    /* this will be called BEFORE the transform will be reset to initial values (at turn beginning/end) 
       you may modify the initial values here or the new values, 
       if you return 1 then no further changes to tf will be done,
       if you return 0 then tf0 will be copied to tf after running this function */
    //virtual int transformResetNotify(struct sTfData *tf, struct sTfData *tf0) { return 0; }

    /* Do the actual transformation (do NOT use this to calculate parameters!) 
       This function will only be called if not in ANALYSIS mode 
       Please return the number of output samples (0, if you haven't produced output) */
    virtual int transformDataFloat(const struct sTfData * tf, const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

    //virtual int transformDataInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);

    /* Update transform parameters incrementally
       This function will only be called if not in TRANSFORMATIONs mode 
       *buf is a pointer to a buffer if updateMethod is fixedBuffer */
    // return value: 0: no update was performed , 1: successfull update
    virtual int updateTransformFloatExp(struct sTfData * tf, const FLOAT_DMEM *src, int idxi);
    virtual int updateTransformFloatBuf(struct sTfData * tf, const FLOAT_DMEM *src, FLOAT_DMEM *buf, long Nbuf, long wrPtr, int idxi);
    virtual int updateTransformFloatAvg(struct sTfData * tf, const FLOAT_DMEM *src, int idxi);
    virtual int updateTransformFloatAvgI(struct sTfData * tf, const FLOAT_DMEM *src, FLOAT_DMEM *buf, long * bufferNframes, long Nbuf, long wrPtr, int idxi);

    /* generic method, default version will select one of the above methods,
       overwrite to implement your own update strategy ('usr' option) */
    //virtual int updateTransformFloat(struct sTfData * tf, const FLOAT_DMEM *src, FLOAT_DMEM *buf, long * bufferNframes, long Nbuf, int idxi);

/////////////////////////////////////////////
    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    //virtual int myTick(long long t);

    //virtual int configureWriter(sDmLevelConfig &c);

    //virtual int processComponentMessage( cComponentMessage *_msg );
    //virtual int setupNamesForField(int i, const char*name, long nEl);
    //virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    //virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    //virtual int flushVectorFloat(FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cVectorMVN(const char *_name);

    virtual ~cVectorMVN();
};




#endif // __CVECTORMVN_HPP
