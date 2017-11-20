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

functionals container

passes unsorted row data array AND (if required) sorted row data array to functional processors

*/


#ifndef __CFUNCTIONALS_HPP
#define __CFUNCTIONALS_HPP

#include <core/smileCommon.hpp>
#include <core/winToVecProcessor.hpp>
#include <functionals/functionalComponent.hpp>

#define BUILD_COMPONENT_Functionals
#define COMPONENT_DESCRIPTION_CFUNCTIONALS "computes functionals from input frames, this component uses various cFunctionalXXXX sub-components, which implement the actual functionality"
#define COMPONENT_NAME_CFUNCTIONALS "cFunctionals"
#define COMPONENT_NAME_CFUNCTIONALS_LENGTH 12

#undef class
class DLLEXPORT cFunctionals : public cWinToVecProcessor {
  private:
    int nFunctTp, nFunctTpAlloc;  // number of cFunctionalXXXX types found
    char **functTp;  // type names (without cFunctional prefix)
    int *functTpI;   // index of cFunctionalXXXX type in componentManager
    int *functI;   // index of cFunctionalXXXX type in componentManager
    int *functN;   // number of output values of each functional object
    cFunctionalComponent **functObj;
    int requireSorted;
    int nonZeroFuncts;
    const char * functNameAppend;
    int timeNorm;

  protected:
    int nFunctionalsEnabled;
    int nFunctValues;  // size of output vector

    SMILECOMPONENT_STATIC_DECL_PR

//    virtual void fetchConfig();
    virtual int myConfigureInstance();

    //virtual int myFinaliseInstance();
    //virtual int myTick(long long t);

    virtual int getMultiplier();
    //virtual int configureWriter(const sDmLevelConfig *c);
    virtual int setupNamesForElement(int idxi, const char*name, long nEl);
    virtual int doProcess(int i, cMatrix *row, FLOAT_DMEM*x);
    virtual int doProcessMatrix(int i, cMatrix *in, FLOAT_DMEM *out, long nOut);

//    virtual int doProcess(int i, cMatrix *row, INT_DMEM*x);

  public:
    SMILECOMPONENT_STATIC_DECL
    static int rAcounter;
    cFunctionals(const char *_name);

    virtual ~cFunctionals();
};




#endif // __CFUNCTIONALS_HPP
