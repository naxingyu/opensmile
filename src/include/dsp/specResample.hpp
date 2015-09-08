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

experimental resampling by ideal fourier interpolation and band limiting
this component takes a complex (!) dft spectrum (generated from real values) as input

*/


#ifndef __CSPECRESAMPLE_HPP
#define __CSPECRESAMPLE_HPP

#include <core/smileCommon.hpp>
#include <core/vectorProcessor.hpp>

#define BUILD_COMPONENT_SpecResample
#define COMPONENT_DESCRIPTION_CSPECRESAMPLE "This component implements a spectral domain resampling component. Input frames are complex valued spectral domain data, which will be shifted and scaled by this component, and a modified DFT is performed to synthesize samples at the new rate."
#define COMPONENT_NAME_CSPECRESAMPLE "cSpecResample"

#undef class
class DLLEXPORT cSpecResample : public cVectorProcessor {
  private:
    int antiAlias;
    long kMax;
    double sr;
    double fsSec;
    double targetFs;
    double resampleRatio;
    long _Nin, _Nout;
    FLOAT_DMEM *inData;
    const char * inputFieldPartial;

    sDftWork * dftWork;
    //FLOAT_DMEM *costable, *sintable;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
	  virtual int setupNewNames(long nEl);
    virtual int configureWriter(sDmLevelConfig &c);
    //virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cSpecResample(const char *_name);
    virtual ~cSpecResample();
};




#endif // __CSPECRESAMPLE_HPP
