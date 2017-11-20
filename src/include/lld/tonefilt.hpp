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

on-line semi-tone filter bank

*/


#ifndef __CTONEFILT_HPP
#define __CTONEFILT_HPP

#include <core/smileCommon.hpp>
#include <core/dataProcessor.hpp>

#define BUILD_COMPONENT_Tonefilt
#define COMPONENT_DESCRIPTION_CTONEFILT "This component implements an on-line, sample by sample semi-tone filter bank which can be used as first step for the computation of CHROMA features as a replacement of cTonespec. The filter is based on correlating with a sine wave of the exact target frequency of a semi-tone for each note in the filter-bank."
#define COMPONENT_NAME_CTONEFILT "cTonefilt"

#undef class
class DLLEXPORT cTonefilt : public cDataProcessor {
  private:
    double outputPeriod; /* in seconds */
    long  /*outputBuffersize,*/ outputPeriodFrames; /* in frames */
    int nNotes;
    double firstNote;
    long N, Nf;
    double inputPeriod;
    double decayFN, decayF0;
    
    cVector *tmpVec;
    FLOAT_DMEM *tmpFrame;

    double **corrS, **corrC;
    double *decayF;
    double *freq;
    long * pos;

    void doFilter(int i, cMatrix *row, FLOAT_DMEM*x);
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    virtual int configureWriter(sDmLevelConfig &c);
    virtual int setupNewNames(long nEl);
      
  public:
    SMILECOMPONENT_STATIC_DECL
    
    cTonefilt(const char *_name);

    virtual ~cTonefilt();
};




#endif // __CTONEFILT_HPP
