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
