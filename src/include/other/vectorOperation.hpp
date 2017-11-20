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

do elementary operations on vectors 
(i.e. basically everything that does not require history or context,
 everything that can be performed on single vectors w/o external data (except for constant parameters, etc.))

*/


#ifndef __CVECTOROPERATION_HPP
#define __CVECTOROPERATION_HPP

#include <core/smileCommon.hpp>
#include <core/vectorProcessor.hpp>

#define COMPONENT_DESCRIPTION_CVECTOROPERATION "This component performs elementary operations on vectors (i.e. basically everything that does not require history or context, everything that can be performed on single vectors w/o external data (except for constant parameters, etc.))"
#define COMPONENT_NAME_CVECTOROPERATION "cVectorOperation"

// n->n mapping operations
#define VOP_NORMALISE 0
#define VOP_ADD       1
#define VOP_MUL       2
#define VOP_LOG       3
#define VOP_NORMALISE_L1 4
#define VOP_SQRT      5
#define VOP_LOGA      6
#define VOP_POW       7
#define VOP_EXP       8
#define VOP_E         9
#define VOP_ABS      10
#define VOP_AGN      11
#define VOP_FLATTEN  12
#define VOP_NORM_RANGE0  13
#define VOP_NORM_RANGE1  14
#define VOP_MIN      15
#define VOP_MAX      16
#define VOP_DB_POW   17
#define VOP_DB_MAG   18
#define VOP_FCONV    20

// begin of n->1 mapping operations
#define VOP_X          1000 
// n->1 mapping operations
#define VOP_X_SUM      1001
#define VOP_X_SUMSQ    1002
#define VOP_X_L1       1003
#define VOP_X_L2       1004


#undef class
class DLLEXPORT cVectorOperation : public cVectorProcessor {
  private:
    int powOnlyPos;
    double gnGenerator();
    char * operationString_;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    int operation;
    int fscaleA, fscaleB;
    const char * nameBase;
    const char * targFreqScaleStr;

    FLOAT_DMEM param1,param2,logfloor;

    virtual void fetchConfig();
  
    virtual int setupNamesForField(int i, const char*name, long nEl);
    //virtual int setupNewNames(long ni);
    virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cVectorOperation(const char *_name);

    virtual ~cVectorOperation();
};




#endif // __CVECTOROPERATION_HPP
