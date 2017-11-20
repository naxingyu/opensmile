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

number of segments based on delta thresholding

*/


#ifndef __CFUNCTIONALMODULATION_HPP
#define __CFUNCTIONALMODULATION_HPP

#include <core/smileCommon.hpp>
#include <core/dataMemory.hpp>
#include <functionals/functionalComponent.hpp>

#define COMPONENT_DESCRIPTION_CFUNCTIONALMODULATION "  Modulation Spectrum"
#define COMPONENT_NAME_CFUNCTIONALMODULATION "cFunctionalModulation"

class cSmileUtilWindowedMagnitudeSpectrum {
private:
  FLOAT_TYPE_FFT * fftWork_;  // fft work area (size Nfft)
  int * ip_;
  FLOAT_TYPE_FFT * w_;
  FLOAT_DMEM * winFunc_;
  int winFuncId_;
  long Nfft_;  // number of samples to perform FFT on (after zero-padding)
  long Nin_;  // number of inputs (for window function)

protected:
  void allocateWinFunc(long Nin);
  void allocateFFTworkspace(long Nin);
  void freeFFTworkspace();
  void freeWinFunc();
  void copyInputAndZeropad(const FLOAT_DMEM *in, long Nin, bool allowWinSmaller);
  void doFFT();
  void computeMagnitudes();

public:
  cSmileUtilWindowedMagnitudeSpectrum():
    fftWork_(NULL),
    ip_(NULL), w_(NULL),
    winFunc_(NULL), winFuncId_(0),
    Nin_(0), Nfft_(0) {}

  long getNin() {
    return Nin_;
  }
  cSmileUtilWindowedMagnitudeSpectrum(long Nin, int winFuncId);
  ~cSmileUtilWindowedMagnitudeSpectrum();

  // T is the period of one input sample
  // Returns the frequency width of each FFT magnitude bin (deltaF)
  double getBinDeltaF(double T) {
    if (T == 0.0) {
      return 0.0;
    } else {
      return 1.0 / (T * (double)Nfft_);
    }
  }
  const FLOAT_DMEM * getMagnitudes(const FLOAT_DMEM *in,
      long Nin, bool allowWinSmaller);
  long getNmagnitudes() {
    return Nfft_ / 2 + 1;
  }
};

class cSmileUtilMappedMagnitudeSpectrum {
private:
  long Nmag_;
  long Nout_;  // number of output bins
  cSmileUtilWindowedMagnitudeSpectrum *fft_;
  FLOAT_DMEM * modSpec_;  // the actual modulation spectrum
  double minFreq_;
  double maxFreq_;
  FLOAT_DMEM * splineWork_;
  FLOAT_DMEM * splineDerivs_;
  FLOAT_DMEM * magFreq_;
  double deltaF_;
  double T_;

protected:
  void mapMagnitudesToModSpecBins(const FLOAT_DMEM *mag, long N);

public:
  cSmileUtilMappedMagnitudeSpectrum():
    modSpec_(NULL), fft_(NULL), Nmag_(0),
    splineWork_(NULL), splineDerivs_(NULL), magFreq_(NULL) {}

  long getNin() {
    if (fft_ != NULL)
      return fft_->getNin();
    return 0;
  }

  cSmileUtilMappedMagnitudeSpectrum(
      long Nin,       // number of input samples (before zero padding)
      long Nout,      // number of modulation spectrum bins
      int winFuncId,  // window function numeric ID (see smileutil)
      double minFreq, double maxFreq, // min/max frequency of modulation spectrum
      double T        // T is the sample/frame period  (to be able to map FFT bins to frequencies)
      );
  ~cSmileUtilMappedMagnitudeSpectrum();

  void compute(const FLOAT_DMEM *in, long Nin, bool allowWinSmaller);
  const FLOAT_DMEM * getModSpec();
  long getModSpecN() {
    return Nout_;
  }
};


////////////////////////

#undef class
class DLLEXPORT cFunctionalModulation : public cFunctionalComponent {
private:
  double stftWinSizeSec_;
  double stftWinStepSec_;
  long stftWinSizeFrames_;
  long stftWinStepFrames_;
  double modSpecMinFreq_;
  double modSpecMaxFreq_;
  int modSpecNumBins_;
  double modSpecResolution_;
  int winFuncId_;
  int removeNonZeroMean_;
  int ignoreLastFrameIfTooShort_;

  FLOAT_DMEM *inNorm_;
  long inNormN_;
  FLOAT_DMEM *avgModSpec_;
  cSmileUtilMappedMagnitudeSpectrum *mappedSpec_;
  char * tmpstr_;

protected:
  SMILECOMPONENT_STATIC_DECL_PR
  virtual void fetchConfig();
  void computeModSpecSTFT(const FLOAT_DMEM *in, long Nin);
  int computeModSpecSTFTavg(const FLOAT_DMEM *in, long Nin, FLOAT_DMEM *ms);

public:
  SMILECOMPONENT_STATIC_DECL

  cFunctionalModulation(const char *_name);
  // inputs: sorted and unsorted array of values, out: pointer to space in output array, You may not return MORE than Nout elements, please return as return value the number of actually computed elements (usually Nout)
  virtual long process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM *out, long Nin, long Nout);
  //virtual long process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout);
  virtual const char * getValueName(long i);
  virtual void setFieldMetaData(cDataWriter *writer,
          const FrameMetaInfo *fmeta, int idxi, long nEl);
  virtual long getNumberOfElements(long j);
  virtual long getNoutputValues() { return nEnab; }
  virtual int getRequireSorted() { return 0; }

  virtual ~cFunctionalModulation();
};




#endif // __CFUNCTIONALMODULATION_HPP
