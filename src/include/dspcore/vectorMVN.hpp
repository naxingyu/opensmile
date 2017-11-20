/*F***************************************************************************
 * 
 * openSMILE - the Munich open source Multimedia Interpretation by 
 * Large-scale Extraction toolkit
 * 
 * This file is part of openSMILE.
 * 
 * openSMILE is copyright (c) by audEERING GmbH. All rights reserved.
 * 
 * See file "COPYING" for details on usage rights and licensing terms.
 * By using, copying, editing, compiling, modifying, reading, etc. this
 * file, you agree to the licensing terms in the file COPYING.
 * If you do not agree to the licensing terms,
 * you must immediately destroy all copies of this file.
 * 
 * THIS SOFTWARE COMES "AS IS", WITH NO WARRANTIES. THIS MEANS NO EXPRESS,
 * IMPLIED OR STATUTORY WARRANTY, INCLUDING WITHOUT LIMITATION, WARRANTIES OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ANY WARRANTY AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE SOFTWARE OR ANY WARRANTY OF TITLE
 * OR NON-INFRINGEMENT. THERE IS NO WARRANTY THAT THIS SOFTWARE WILL FULFILL
 * ANY OF YOUR PARTICULAR PURPOSES OR NEEDS. ALSO, YOU MUST PASS THIS
 * DISCLAIMER ON WHENEVER YOU DISTRIBUTE THE SOFTWARE OR DERIVATIVE WORKS.
 * NEITHER TUM NOR ANY CONTRIBUTOR TO THE SOFTWARE WILL BE LIABLE FOR ANY
 * DAMAGES RELATED TO THE SOFTWARE OR THIS LICENSE AGREEMENT, INCLUDING
 * DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL OR INCIDENTAL DAMAGES, TO THE
 * MAXIMUM EXTENT THE LAW PERMITS, NO MATTER WHAT LEGAL THEORY IT IS BASED ON.
 * ALSO, YOU MUST PASS THIS LIMITATION OF LIABILITY ON WHENEVER YOU DISTRIBUTE
 * THE SOFTWARE OR DERIVATIVE WORKS.
 * 
 * Main authors: Florian Eyben, Felix Weninger, 
 * 	      Martin Woellmer, Bjoern Schuller
 * 
 * Copyright (c) 2008-2013, 
 *   Institute for Human-Machine Communication,
 *   Technische Universitaet Muenchen, Germany
 * 
 * Copyright (c) 2013-2015, 
 *   audEERING UG (haftungsbeschraenkt),
 *   Gilching, Germany
 * 
 * Copyright (c) 2016,	 
 *   audEERING GmbH,
 *   Gilching Germany
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
