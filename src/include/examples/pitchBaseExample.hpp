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


#ifndef __CPITCHBASEEXAMPLE_HPP
#define __CPITCHBASEEXAMPLE_HPP

#include <core/smileCommon.hpp>
#include <lldcore/pitchBase.hpp>

#define COMPONENT_DESCRIPTION_CPITCHBASEEXAMPLE "Base class for all pitch classes, no functionality on its own!"
#define COMPONENT_NAME_CPITCHBASEEXAMPLE "cPitchBaseExample"


#undef class
class DLLEXPORT cPitchBaseExample : public cPitchBase {
  private:

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
/*	  virtual int setupNewNames(long nEl); */
    
    // to be overwritten by child class:
    virtual int pitchDetect(FLOAT_DMEM * _inData, long _N, double _fsSec, double _baseT, FLOAT_DMEM *_f0cand, FLOAT_DMEM *_candVoice, FLOAT_DMEM *_candScore, long _nCandidates);
    virtual int addCustomOutputs(FLOAT_DMEM *dstCur, long NdstLeft);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cPitchBaseExample(const char *_name);
    
    virtual ~cPitchBaseExample();
};




#endif // __CPITCHBASEEXAMPLE_HPP
