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

