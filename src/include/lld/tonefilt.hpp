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

on-line semi-tone filter bank

*/


#ifndef __CTONEFILT_HPP
#define __CTONEFILT_HPP

#include <core/smileCommon.hpp>
#include <core/dataProcessor.hpp>

#define BUILD_COMPONENT_Tonefilt
#define COMPONENT_DESCRIPTION_CTONEFILT "This component implements an on-line, sample by sample semi-tone filter bank which can be used as first step for the computation of CHROMA features as a replacement of cTonespec. The filter is based on correlating with a sine wave of the exact target frequency of a semi-tone for each note in the filter-bank."
#define COMPONENT_NAME_CTONEFILT "cTonefilt"

#undef class
class DLLEXPORT cTonefilt : public cDataProcessor {
  private:
    double outputPeriod; /* in seconds */
    long  /*outputBuffersize,*/ outputPeriodFrames; /* in frames */
    int nNotes;
    double firstNote;
    long N, Nf;
    double inputPeriod;
    double decayFN, decayF0;
    
    cVector *tmpVec;
    FLOAT_DMEM *tmpFrame;

    double **corrS, **corrC;
    double *decayF;
    double *freq;
    long * pos;

    void doFilter(int i, cMatrix *row, FLOAT_DMEM*x);
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    virtual int configureWriter(sDmLevelConfig &c);
    virtual int setupNewNames(long nEl);
      
  public:
    SMILECOMPONENT_STATIC_DECL
    
    cTonefilt(const char *_name);

    virtual ~cTonefilt();
};




#endif // __CTONEFILT_HPP
