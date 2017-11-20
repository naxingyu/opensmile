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

This component computes pitch via the Harmonic Product Spectrum method.
As input it requires FFT magnitude data. 
Note that the type of input data is not checked, thus be careful when writing your configuration files!

*/


#ifndef __CPITCHSHS_HPP
#define __CPITCHSHS_HPP

#include <core/smileCommon.hpp>
#include <lldcore/pitchBase.hpp>

#define BUILD_COMPONENT_PitchShs
#define COMPONENT_DESCRIPTION_CPITCHSHS "This component computes the fundamental frequency via the Sub-Harmonic-Sampling (SHS) method (this is related to the Harmonic Product Spectrum method)."
#define COMPONENT_NAME_CPITCHSHS "cPitchShs"


#undef class
class DLLEXPORT cPitchShs : public cPitchBase {
  private:
    int nHarmonics;
    int greedyPeakAlgo;
    FLOAT_DMEM Fmint, Fstept;
    FLOAT_DMEM nOctaves, nPointsPerOctave;
    FLOAT_DMEM *SS;
    FLOAT_DMEM *Fmap;
    FLOAT_DMEM compressionFactor;
    double base;
    double lfCut_;
    int shsSpectrumOutput;

    void addNameAppendFieldShs(const char*base, const char*append, int N, int arrNameOffset);
    int cloneInputFieldInfoShs(int sourceFidx, int targetFidx, int force);

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    cDataWriter * shsWriter_;
    cVector * shsVector_;

    virtual void fetchConfig();
    virtual int setupNewNames(long nEl);
    
    virtual void mySetEnvironment();
    virtual int myRegisterInstance(int *runMe);
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();

    // to be overwritten by child class:
    virtual int pitchDetect(FLOAT_DMEM * inData, long N_, double _fsSec, double baseT, FLOAT_DMEM *f0cand, FLOAT_DMEM *candVoice, FLOAT_DMEM *candScore, long nCandidates);
    virtual int addCustomOutputs(FLOAT_DMEM *dstCur, long NdstLeft);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cPitchShs(const char *_name);
    
    virtual ~cPitchShs();
};




#endif // __CPITCHSHS_HPP
