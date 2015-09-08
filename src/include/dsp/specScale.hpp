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


#ifndef __CSPECSCALE_HPP
#define __CSPECSCALE_HPP

#include <core/smileCommon.hpp>
#include <core/vectorProcessor.hpp>

#define BUILD_COMPONENT_SpecScale
#define COMPONENT_DESCRIPTION_CSPECSCALE "This component performs linear/non-linear axis scaling of FFT magnitude spectra with spline interpolation."
#define COMPONENT_NAME_CSPECSCALE "cSpecScale"

/*
#define SPECTSCALE_LINEAR   0
#define SPECTSCALE_LOG      1
#define SPECTSCALE_BARK     2
#define SPECTSCALE_MEL      3
#define SPECTSCALE_SEMITONE 4
#define SPECTSCALE_BARK_SCHROED     5
#define SPECTSCALE_BARK_SPEEX       6
*/

#define INTERP_NONE       0
#define INTERP_LINEAR     1
#define INTERP_SPLINE     2

#undef class
class DLLEXPORT cSpecScale : public cVectorProcessor {
  private:
    int scale; // target scale
    int sourceScale;
    int interpMethod;
    int specSmooth, specEnhance;
    int auditoryWeighting;
    double logScaleBase, logSourceScaleBase;
    double minF, maxF, fmin_t, fmax_t;
    long nPointsTarget;
    double firstNote, param;

    long nMag, magStart;
    double fsSec;
    double deltaF, deltaF_t;

    double *f_t;
    double *spline_work;
    double *y, *y2;
    double *audw;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
	  virtual int setupNewNames(long nEl);
    virtual int dataProcessorCustomFinalise();
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cSpecScale(const char *_name);
    
    virtual ~cSpecScale();
};

#endif // __CSPECSCALE_HPP
