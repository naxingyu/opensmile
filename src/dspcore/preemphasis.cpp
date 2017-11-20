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


/*  openSMILE component: preemphasis

simple preemphasis : y(t) = x(t) - k*x(t-1)

k = exp( -2*pi * preemphasisFreq./samplingFreq. )

*/


#include <dspcore/preemphasis.hpp>
//#include <math.h>

#define MODULE "cPreemphasis"


SMILECOMPONENT_STATICS(cPreemphasis)

SMILECOMPONENT_REGCOMP(cPreemphasis)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CPREEMPHASIS;
  sdescription = COMPONENT_DESCRIPTION_CPREEMPHASIS;

  // we inherit cWindowProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cWindowProcessor")

  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("k","The pre-emphasis coefficient k in y[n] = x[n] - k*x[n-1]",0.97);
    ct->setField("f","The pre-emphasis frequency f in Hz : k = exp( -2*pi * f/samplingFreq. ) (if set, f will override k!)",0,0,0);
    ct->setField("de","1 = perform de-emphasis instead of pre-emphasis (i.e. y[n] = x[n] + k*x[n-1])",0);
  )

  SMILECOMPONENT_MAKEINFO(cPreemphasis);
}

SMILECOMPONENT_CREATE(cPreemphasis)

//-----

cPreemphasis::cPreemphasis(const char *_name) :
  cWindowProcessor(_name,1,0)
{
}


void cPreemphasis::fetchConfig()
{
  cWindowProcessor::fetchConfig();
  
  k = (FLOAT_DMEM)getDouble("k");

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
  if (de) {
    SMILE_IDBG(2,"performing de-emphasis instead of pre-emphasis");
  }
}

int cPreemphasis::dataProcessorCustomFinalise()
{
  int ret = cWindowProcessor::dataProcessorCustomFinalise();
  if (f >= 0.0) {
    double _T = reader_->getLevelT();
    k = (FLOAT_DMEM)exp( -2.0*M_PI * f * _T );
    SMILE_IDBG(2,"computed k from f (%f Hz) : k = %f  (samplingRate = %f Hz)",f,k,1.0/_T);
  }
  return ret;
}

// order is the amount of memory (overlap) that will be present in _in
// buf will have nT timesteps, however also order negative indicies (i.e. you may access a negative array index up to -order!)
int cPreemphasis::processBuffer(cMatrix *_in, cMatrix *_out, int _pre, int _post )
{
  long n;

  if (_in->type!=DMEM_FLOAT) COMP_ERR("dataType (%i) != DMEM_FLOAT not yet supported!",_in->type);
  FLOAT_DMEM *x=_in->dataF;
  FLOAT_DMEM *y=_out->dataF;
  if (de) {
    for (n=0; n<_out->nT; n++) {
      *(y++) = *(x) + k * *(x-1);
      x++;
    }
  } else {
    for (n=0; n<_out->nT; n++) {
      *(y++) = *(x) - k * *(x-1);
      x++;
    }
  }
  return 1;
}


cPreemphasis::~cPreemphasis()
{
}

