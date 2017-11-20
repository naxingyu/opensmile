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

computes spectral features such as flux, roll-off, centroid, etc.

*/


#ifndef __CSPECTRAL_HPP
#define __CSPECTRAL_HPP

#include <core/smileCommon.hpp>
#include <core/vectorProcessor.hpp>

#define COMPONENT_DESCRIPTION_CSPECTRAL "This component computes spectral features such as flux, roll-off, centroid, and user defined band energies (rectangular summation of FFT magnitudes), etc."
#define COMPONENT_NAME_CSPECTRAL "cSpectral"

#undef class
class DLLEXPORT cSpectral : public cVectorProcessor {
  private:
    int frqScale;
    int normBandEnergies;
    double frqScaleParam;

    int squareInput, centroid, nBands, nSlopes, nRollOff, entropy;
    int specDiff, specPosDiff;
    int flux, fluxCentroid, fluxAtFluxCentroid;
    int standardDeviation, variance, skewness, kurtosis, slope;
    int sharpness, tonality, harmonicity, flatness;
    int alphaRatio, hammarbergIndex;

    long specRangeLower, specRangeUpper, specRangeLowerBin, specRangeUpperBin;
    long nScale;
    int maxPos, minPos;
    long *bandsL, *bandsH; // frequencies
    long *slopesL, *slopesH; // frequencies    
    double *bandsLi, *bandsHi; // indicies (fft bins), real values
    double *wghtLi, *wghtHi; // indicies (fft bins), real values
    double *slopeBandsLi, *slopeBandsHi; // indicies (fft bins), real values
    double *slopeWghtLi, *slopeWghtHi; // indicies (fft bins), real values
    double *rollOff;
    double fsSec;
    const double *frq;
    int buggyRollOff, buggySlopeScale;
    int useLogSpectrum;
    int logFlatness;
    FLOAT_DMEM specFloor, logSpecFloor;
    bool requireMagSpec, requirePowerSpec, requireLogSpec;

    FLOAT_DMEM **prevSpec;
    long nFieldsPrevSpec;
    long *nSrcPrevSpec;


    void setRequireLorPspec();
    
    int isBandValid(long start, long end)
    {
      if ((start>=0)&&(end>0)) return 1;
      else return 0;
    }
    
    void parseRange(const char *val, long *lowerHz, long *upperHz);
    int parseBandsConfig(const char * field, long ** bLow, long ** bHigh);
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();

    virtual int setupNamesForField(int i, const char*name, long nEl);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);


  public:
    SMILECOMPONENT_STATIC_DECL
    
    cSpectral(const char *_name);

    virtual ~cSpectral();
};




#endif // __CSPECTRAL_HPP
