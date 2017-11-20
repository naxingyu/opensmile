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

Mel-frequency spectrum

*/


#ifndef __MELSPEC_HPP
#define __MELSPEC_HPP

#include <core/smileCommon.hpp>
#include <core/vectorProcessor.hpp>
#include <math.h>

#define COMPONENT_DESCRIPTION_CMELSPEC "This component computes an N-band Mel/Bark/Semitone-frequency spectrum (critical band spectrum) by applying overlapping triangular filters equidistant on the Mel/Bark/Semitone-frequency scale to an FFT magnitude or power spectrum."
#define COMPONENT_NAME_CMELSPEC "cMelspec"

/*
#define SPECSCALE_MEL      0
#define SPECSCALE_BARK     2
#define SPECSCALE_BARK_SCHROED    3
#define SPECSCALE_BARK_SPEEX     4
#define SPECSCALE_SEMITONE 10
*/

#define TWELFTH_ROOT_OF_2  1.1224620483093729814335330496787


#undef class
class DLLEXPORT cMelspec : public cVectorProcessor {
  private:
    int hfcc;
    int inverse;
    int nBands, htkcompatible, usePower;
    FLOAT_DMEM **filterCoeffs;
    FLOAT_DMEM **filterCfs;
    long **chanMap;
    long bs;
    FLOAT_DMEM lofreq, hifreq;
    long *nLoF, *nHiF;
    int specScale;
    double firstNote, logScaleBase, param;
    int customBandwidth;
    double halfBwTarg;

    // Hertz to Mel/Bark/..
/*    FLOAT_DMEM Mel(FLOAT_DMEM fhz) {
      switch(specScale) {
        case SPECSCALE_MEL : 
          return (FLOAT_DMEM)(1127.0*log(1.0+(double)fhz/700.0));
        case SPECSCALE_BARK:
          if (fhz>0) {
            return (26.81 / (1.0 + 1960.0/fhz)) - 0.53;
          } else return 0.0;
        case SPECSCALE_BARK_SCHROED : 
          if (fhz>0) {
            double f6 = fhz/600.0;
            return (6.0 * log(f6 + sqrt(f6*f6 + 1.0) ) );
          } else return 0.0;
        case SPECSCALE_BARK_SPEEX :  
          return (FLOAT_DMEM)(13.1*atan(.00074*(double)fhz)+2.24*atan(((double)fhz)*((double)fhz)*1.85e-8)+1e-4*((double)fhz));
        // EXPERIMENTAL:
        case SPECSCALE_SEMITONE : {
                          if (fhz < 27.5) fhz = 27.5;
                          return (FLOAT_DMEM)( log( (double)(fhz) / 27.5 ) / log (TWELFTH_ROOT_OF_2) ) ;
                       }
        default: // =Mel
          return (FLOAT_DMEM)(1127.0*log(1.0+(double)fhz/700.0));
      }
    }

    
    // Mel to Hertz
    FLOAT_DMEM Hertz(FLOAT_DMEM fmel) {
      switch(specScale) {
        case SPECSCALE_MEL : 
          return (FLOAT_DMEM)(700.0*(exp((double)fmel/1127.0)-1.0));        
        case SPECSCALE_BARK:        
          double z0 = (z+0.53)/26.81;
          if (z0 != 1.0) return (1960.0 * z0)/(1.0-z0);
          else return 0.0;
        case SPECSCALE_BARK_SCHROED: SMILE_IERR(1,"bark to hertz not yet implemented!"); break;
        case SPECSCALE_BARK_SPEEX:  SMILE_IERR(1,"bark to hertz not yet implemented!"); break;
        // EXPERIMENTAL:
        case SPECSCALE_SEMITONE : {
                          return (FLOAT_DMEM)( exp(fmel * log( TWELFTH_ROOT_OF_2 )) * 27.5 ) ;
                       }
        default: // =Mel
          return (FLOAT_DMEM)(700.0*(exp((double)fmel/1127.0)-1.0));
      }
      return 0.0;
    }
*/

    // convert frequency (hz) to FFT bin number
    long FtoN(FLOAT_DMEM fhz, FLOAT_DMEM baseF)
    {
      return (long)round((double)(fhz/baseF));
    }

    // convert FFT bin number to frequency (hz)
    FLOAT_DMEM NtoF(long bin, FLOAT_DMEM baseF)
    {
      return ((FLOAT_DMEM)bin * baseF);
    }

    // convert bin number to frequency in Mel/Bark/...
    FLOAT_DMEM NtoFmel(long N, FLOAT_DMEM baseF)
    {
      return (FLOAT_DMEM)smileDsp_specScaleTransfFwd( ((FLOAT_DMEM)N)*baseF , specScale, param );
    }

    int computeFilters( long blocksize, double frameSizeSec, int idxc );
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    //virtual int myTick(long long t);

    //virtual int configureWriter(const sDmLevelConfig *c);
    virtual int dataProcessorCustomFinalise();

    virtual void configureField(int idxi, long myN, long _nOut);
    virtual int setupNamesForField(int i, const char*name, long nEl);
    //virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cMelspec(const char *_name);

    virtual ~cMelspec();
};




#endif // __MELSPEC_HPP
