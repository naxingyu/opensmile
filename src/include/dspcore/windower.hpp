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

data windower.
takes frames from one level, applies window function, and saves to other level

*/


#ifndef __WINDOWER_HPP
#define __WINDOWER_HPP

#include <core/smileCommon.hpp>
#include <core/vectorProcessor.hpp>

#define COMPONENT_DESCRIPTION_CWINDOWER "This component applies applies window function to pcm frames."
#define COMPONENT_NAME_CWINDOWER "cWindower"


struct sWindowerConfigParsed
{
  int winFunc;
  int squareRoot;
  double offset, gain;
  double sigma, alpha, alpha0, alpha1, alpha2, alpha3;
  double * win;
  long frameSizeFrames;
};


// WINF_XXXXXX constants are defined in smileUtil.hpp !
#undef class
class DLLEXPORT cWindower : public cVectorProcessor {
  private:
    double offset, gain;
    double sigma, alpha, alpha0, alpha1, alpha2, alpha3;
    long  frameSizeFrames;
    int   winFunc;    // winFunc as numeric constant (see #defines above)
    int   squareRoot;
    double xscale;
    double xshift;
    double fade;
    const char *saveWindowToFile;
    int   dtype;     // data type (DMEM_FLOAT, DMEM_INT)
    double *win;
    
    void precomputeWinFunc();
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    virtual int myFinaliseInstance();

    virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cWindower(const char *_name);

    // return the parsed windower config. If this function is called after the component was finalised, the *win variable will point to the precomputed window.
    struct sWindowerConfigParsed * getWindowerConfigParsed();

    virtual ~cWindower();
};




#endif // __WINDOWER_HPP
