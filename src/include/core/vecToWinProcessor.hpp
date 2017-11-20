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

reads in frames , outputs windows
*/


#ifndef __CVECTOWINPROCESSOR_HPP
#define __CVECTOWINPROCESSOR_HPP

#include <core/smileCommon.hpp>
#include <core/dataProcessor.hpp>
#include <dspcore/windower.hpp>

#define COMPONENT_DESCRIPTION_CVECTOWINPROCESSOR "Base class: reads in frames , outputs windows"
#define COMPONENT_NAME_CVECTOWINPROCESSOR "cVecToWinProcessor"

struct sVecToWinProcessorOla
{
  double * norm;
  FLOAT_DMEM * buffer;
  long bufferPtr;
  long bufferReadPtr;
  long buffersize;
  double overlap;
  long framelen;
};


#undef class
class DLLEXPORT cVecToWinProcessor : public cDataProcessor {
  private:
    
    int   dtype;     // data type (DMEM_FLOAT, DMEM_INT)
    int   noPostEOIprocessing;
    int   processArrayFields;
    int   normaliseAdd;
    int   useWinAasWinB;
    FLOAT_DMEM gain;

    long Nfi;
    long No;

    cWindower * ptrWinA;
    cWindower * ptrWinB;

    long inputPeriodS;
    double inputPeriod;
    double samplePeriod;
    
    int hasOverlap;
    struct sVecToWinProcessorOla * ola;

    cMatrix *mat;

  protected:

    SMILECOMPONENT_STATIC_DECL_PR
    double getInputPeriod(){ return inputPeriod; }
    double getSamplePeriod(){ return samplePeriod; }

    long getNfi() { return Nfi; } // number of fields
    

    virtual void fetchConfig();
    //virtual int myFinaliseInstance();
    //virtual int myConfigureInstance();
    virtual int dataProcessorCustomFinalise();
    virtual int myTick(long long t);

    virtual int configureWriter(sDmLevelConfig &c);
    //virtual int setupNamesForElement(int idxi, const char*name, long nEl);
    //virtual int setupNamesForField(int idxi, const char*name, long nEl);

    void initOla(long n, double _samplePeriod, double _inputPeriod, int idx);
    void computeOlaNorm(long n, int idx);
    double * getWindowfunction(cWindower *_ptrWin, long n, int *didAlloc = NULL);
    //void setOlaBuffer(long i, long j, FLOAT_DMEM val);
    void setOlaBufferNext(long idx, FLOAT_DMEM val);
    int flushOlaBuffer(cMatrix *mat);
    
    virtual int doProcess(int i, cMatrix *row, FLOAT_DMEM*x);
    virtual int doProcess(int i, cMatrix *row, INT_DMEM*x);
    virtual int doFlush(int i, FLOAT_DMEM*x);
    virtual int doFlush(int i, INT_DMEM*x);
    

    //virtual int processComponentMessage( cComponentMessage *_msg );

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cVecToWinProcessor(const char *_name);

    virtual ~cVecToWinProcessor();
};




#endif // __CVECTOWINPROCESSOR_HPP
