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

computes (or rather estimates) semi-tone spectrum from fft spectrum

*/


#ifndef __CTONESPEC_HPP
#define __CTONESPEC_HPP

#include <core/smileCommon.hpp>
#include <core/vectorProcessor.hpp>
#include <math.h>

#define BUILD_COMPONENT_Tonespec
#define COMPONENT_DESCRIPTION_CTONESPEC "This component computes (or rather estimates) a semi-tone spectrum from an FFT magnitude spectrum."
#define COMPONENT_NAME_CTONESPEC "cTonespec"

#undef class
class DLLEXPORT cTonespec : public cVectorProcessor {
  private:
    int nOctaves, nNotes;
    int usePower, dbA;
  #ifdef DEBUG
    int printBinMap, printFilterMap;
  #endif
  
    FLOAT_DMEM firstNote;  // frequency of first note
    FLOAT_DMEM lastNote;   // frequency of last note (will be computed)

    FLOAT_DMEM **pitchClassFreq;
    FLOAT_DMEM **distance2key;
    FLOAT_DMEM **filterMap;
    FLOAT_DMEM **db;
    
    int **binKey;
    int **pitchClassNbins;
    int **flBin;
    
    int filterType;

    void computeFilters( long blocksize, double frameSizeSec, int idxc );
    void setPitchclassFreq( int idxc );
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    //virtual int myTick(long long t);

    //virtual int configureWriter(const sDmLevelConfig *c);
    virtual int dataProcessorCustomFinalise();
    virtual int setupNamesForField(int i, const char*name, long nEl);
    //virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cTonespec(const char *_name);

    virtual ~cTonespec();
};




#endif // __CTONESPEC_HPP
