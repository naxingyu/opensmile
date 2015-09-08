/*F***************************************************************************
 * openSMILE - the open-Source Multimedia Interpretation by Large-scale
 * feature Extraction toolkit
 * 
 * (c) 2008-2011, Florian Eyben, Martin Woellmer, Bjoern Schuller: TUM-MMK
 * 
 * (c) 2012-2013, Florian Eyben, Felix Weninger, Bjoern Schuller: TUM-MMK
 * 
 * (c) 2013-2014 audEERING UG, haftungsbeschr√§nkt. All rights reserved.
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

compute simplified intensity according to :
 Andreas Kieﬂling - Extraktion und Klassifikation prosodischer Merkmale in der automatischen Sprachverarbeitung
 Pg. 156-157

*/


#ifndef __CINTENSITY_HPP
#define __CINTENSITY_HPP

#include <core/smileCommon.hpp>
#include <core/vectorProcessor.hpp>

#define COMPONENT_DESCRIPTION_CINTENSITY "This component computes simplified frame intensity (narrow band approximation). IMPORTANT: It expects UNwindowed raw PCM frames as input!! A Hamming window is internally applied and the resulting signal is squared bevore applying loudness compression, etc."
#define COMPONENT_NAME_CINTENSITY "cIntensity"

#undef class
class DLLEXPORT cIntensity : public cVectorProcessor {
  private:
    double I0;
    double *hamWin;
    long nWin;
    double winSum;

    int intensity, loudness;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    //virtual int myTick(long long t);

    //virtual int configureWriter(const sDmLevelConfig *c);

    virtual int setupNamesForField(int i, const char*name, long nEl);
    //virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cIntensity(const char *_name);

    virtual ~cIntensity();
};




#endif // __CINTENSITY_HPP
