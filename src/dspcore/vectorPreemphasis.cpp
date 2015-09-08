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

pre-emphasis per frame  (simplification, however, this is the way HTK does it... so for compatibility... here you go)
(use before window function is applied!)

*/


#include <dspcore/vectorPreemphasis.hpp>

#define MODULE "cVectorPreemphasis"

SMILECOMPONENT_STATICS(cVectorPreemphasis)

SMILECOMPONENT_REGCOMP(cVectorPreemphasis)
{
  SMILECOMPONENT_REGCOMP_INIT
  
  scname = COMPONENT_NAME_CVECTORPREEMPHASIS;
  sdescription = COMPONENT_DESCRIPTION_CVECTORPREEMPHASIS;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("k","The pre-emphasis coefficient k in y[n] = x[n] - k*x[n-1]",0.97);
    ct->setField("f","The pre-emphasis frequency f in Hz : k = exp( -2*pi * f/samplingFreq. ) (this option will override k)",0,0,0);
    ct->setField("de","1 = perform de- instead of pre-emphasis",0);
  )
  SMILECOMPONENT_MAKEINFO(cVectorPreemphasis);
}

SMILECOMPONENT_CREATE(cVectorPreemphasis)

//-----

cVectorPreemphasis::cVectorPreemphasis(const char *_name) :
  cVectorProcessor(_name),
  k(0.0)
{

}

void cVectorPreemphasis::fetchConfig()
{
  cVectorProcessor::fetchConfig();
  
  k=(FLOAT_DMEM)getDouble("k");

  if (isSet("f")) {
    f = getDouble("f");
  } else {
    f = -1.0;
  }

  if (f < 0.0) { 
    SMILE_IDBG(2,"k = %f",k);
    if ((k<0.0)||(k>1.0)) {
      SMILE_IERR(1,"k must be in the range [0;1]! Setting k=0.0 !");
      k=0.0;
    }
  } else {
    SMILE_IDBG(2,"using preemphasis frequency f=%f Hz instead of k",f);
  }

  de=getInt("de");
  SMILE_DBG(2,"de=%f",de);
}

int cVectorPreemphasis::dataProcessorCustomFinalise()
{
  int ret = cVectorProcessor::dataProcessorCustomFinalise();
  if (f >= 0.0) {
    double _T = getBasePeriod();
    k = (FLOAT_DMEM)exp( -2.0*M_PI * f * _T );
    SMILE_IDBG(2,"computed k from f (%f Hz) : k = %f  (samplingRate = %f Hz)",f,k,1.0/_T);
  }
  return ret;
}


// a derived class should override this method, in order to implement the actual processing
int cVectorPreemphasis::processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  // do domething to data in *src, save result to *dst
  // NOTE: *src and *dst may be the same...
  
  return 0;
}

// a derived class should override this method, in order to implement the actual processing
int cVectorPreemphasis::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  // do domething to data in *src, save result to *dst
  // NOTE: *src and *dst may be the same...

  *(dst++) = (1-k) * *(src++); // - k * *(src-1);
  long n;
  if (de) {
    for (n=1; n<Ndst; n++) {
      *(dst++) = *(src) + k * *(src-1);
      src++;
    }
  } else {
    for (n=1; n<Ndst; n++) {
      *(dst++) = *(src) - k * *(src-1);
      src++;
    }
  }
  return 1;
}

cVectorPreemphasis::~cVectorPreemphasis()
{
}

