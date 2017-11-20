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

functionals: samples values

*/


#include <functionals/functionalSamples.hpp>

#define MODULE "cFunctionalSamples"


#define FUNCT_SAMPLES     0

#define N_FUNCTS  1

#define NAMES     "samples"

#define DEFAULT_NR_SAMPLES 5

const char *samplesNames[] = {NAMES};  // change variable name to your clas...

SMILECOMPONENT_STATICS(cFunctionalSamples)

SMILECOMPONENT_REGCOMP(cFunctionalSamples)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CFUNCTIONALSAMPLES;
  sdescription = COMPONENT_DESCRIPTION_CFUNCTIONALSAMPLES;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("samplepos","Array of positions of samples to copy to the output. The size of this array determines the number of sample frames that will be passed to the output. The given positions must be in the range from 0 to 1, indicating the relative position whithin the input segment, where 0 is the beginning and 1 the end of the segment.",0.0,ARRAY_TYPE);
  )
  
  SMILECOMPONENT_MAKEINFO_NODMEM(cFunctionalSamples);
}

SMILECOMPONENT_CREATE(cFunctionalSamples)

//-----

cFunctionalSamples::cFunctionalSamples(const char *_name) :
  cFunctionalComponent(_name,N_FUNCTS,samplesNames),
  samplepos(0)
{
}

void cFunctionalSamples::fetchConfig()
{
  nSamples = getArraySize("samplepos");
  SMILE_IDBG(2,"nSamples from config = %d", nSamples);
  if (nSamples > 0) {
    samplepos = (double*) malloc(sizeof(double) * nSamples);
    for (int i = 0; i < nSamples; ++i) {
      samplepos[i] = getDouble_f(myvprint("samplepos[%i]", i));
      if (samplepos[i] < 0.0) {
        SMILE_WRN(2,"(inst '%s') samplepos[%i] is out of range [0..1] : %f (clipping to 0.0)",getInstName(),i,samplepos[i]);
        samplepos[i] = 0.0;
      }
      if (samplepos[i] > 1.0) {
        SMILE_WRN(2,"(inst '%s') samplepos[%i] is out of range [0..1] : %f (clipping to 1.0)",getInstName(),i,samplepos[i]);
        samplepos[i] = 1.0;
      }
    }
  }
  else {
    nSamples = DEFAULT_NR_SAMPLES;
    samplepos = (double*) malloc(sizeof(double) * nSamples);
    for (int i = 0; i < nSamples; ++i) {
      samplepos[i] = (double)i / (DEFAULT_NR_SAMPLES-1.0);
      SMILE_IDBG(2,"samplepos[%d] = %.2f", i, samplepos[i]);
    }
  }
  enab[0] = 1;
  cFunctionalComponent::fetchConfig();
  nEnab = nSamples;
}

const char* cFunctionalSamples::getValueName(long i)
{
  const char *n = cFunctionalComponent::getValueName(0);
    // append [i]
  tmpstr = myvprint("%s%.3f",n,samplepos[i]);
  return tmpstr;
}

long cFunctionalSamples::process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted,  FLOAT_DMEM min, FLOAT_DMEM max, FLOAT_DMEM mean, FLOAT_DMEM *out, long Nin, long Nout)
{
  if ((Nin>0)&&(out!=NULL)) {
    FLOAT_DMEM Nind = (FLOAT_DMEM)Nin;
    for (int spi = 0; spi < nSamples; ++spi) {
      SMILE_IDBG(5,"Nind = %.2f, samplepos[%d] = %.2f",Nind,spi,samplepos[spi]);
      int si = (int)((Nind - 1.0) * samplepos[spi]);
      SMILE_IDBG(5,"si(%d) = %d",spi,si);
      out[spi] = in[si];
    }
    return nSamples;
  }
  else {
    return 0;
  }
}

/*
long cFunctionalSamples::process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout)
{

  return 0;
}
*/

cFunctionalSamples::~cFunctionalSamples()
{
  free(samplepos);
  /*if (samplepos != 0) {
    delete[] samplepos;
    samplepos = 0;
  }*/
}

