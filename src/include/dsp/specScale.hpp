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
