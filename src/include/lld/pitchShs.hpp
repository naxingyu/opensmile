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
