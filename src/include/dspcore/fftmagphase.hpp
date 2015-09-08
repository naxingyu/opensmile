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


*/


#ifndef __FFT_MAGPHASE_HPP
#define __FFT_MAGPHASE_HPP

#include <core/smileCommon.hpp>
#include <core/vectorProcessor.hpp>
//#include "fftXg.h"

#define COMPONENT_DESCRIPTION_CFFTMAGPHASE "This component computes magnitude and phase of each array in the input level (it thereby assumes that the arrays contain complex numbers with real and imaginary parts alternating, as computed by the cTransformFFT component)."
#define COMPONENT_NAME_CFFTMAGPHASE "cFFTmagphase"

#undef class
class DLLEXPORT cFFTmagphase : public cVectorProcessor {
  private:
    int inverse;
    int magnitude;
    int phase;
    int joinMagphase;
    int power;
    int normalise, dBpsd;
    FLOAT_DMEM dBpnorm;
    FLOAT_DMEM mindBp;
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();

    virtual int setupNamesForField(int i, const char*name, long nEl);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);


  public:
    SMILECOMPONENT_STATIC_DECL
    
    cFFTmagphase(const char *_name);

    virtual ~cFFTmagphase();
};




#endif // __FFT_MAGPHASE_HPP
