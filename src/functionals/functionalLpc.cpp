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

functional: lpc of input data

*/


#include <functionals/functionalLpc.hpp>

#define MODULE "cFunctionalLpc"

#define N_FUNCTS  2

#define NAMES     "lpgain","lpc"
#define IDX_VAR_FUNCTS 1  // start of lpc array of variable length

const char *lpcNames[] = {NAMES};  

SMILECOMPONENT_STATICS(cFunctionalLpc)

SMILECOMPONENT_REGCOMP(cFunctionalLpc)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CFUNCTIONALLPC;
  sdescription = COMPONENT_DESCRIPTION_CFUNCTIONALLPC;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("lpGain","Output of LP gain (1=yes / 0=no).",0);
    ct->setField("lpc","Output of LP coefficients (1=yes / 0=no).",1);
    ct->setField("firstCoeff","The first LP coefficient to compute. The first coefficient is coeff 0 (default)",0);
    ct->setField("order","The order of the linear prediction (i.e. the max number of coefficients, starting at coefficient number 0).",5);
  )

  SMILECOMPONENT_MAKEINFO_NODMEM(cFunctionalLpc);
}

SMILECOMPONENT_CREATE(cFunctionalLpc)

//-----

cFunctionalLpc::cFunctionalLpc(const char *_name) :
  cFunctionalComponent(_name,N_FUNCTS,lpcNames),
  firstCoeff(1),
  order(0), lpc(NULL), acf(NULL), tmpstr(NULL)
{
}

void cFunctionalLpc::fetchConfig()
{
  firstCoeff = getInt("firstCoeff");
  SMILE_IDBG(2,"firstCoeff = %i",firstCoeff);
  if (firstCoeff < 0) {
    SMILE_IWRN(2,"firstCoeff < 0 in config. forcing firstCoeff=0 !");
	  firstCoeff = 0;
  }
  order = getInt("order");
  if (order <= firstCoeff) {
    SMILE_IERR(2,"error in config: order (%i) cannot be smaller or equal than firstCoeff (%i) (firstCoeff=0 is the first coefficient)!",order,firstCoeff);
    order = firstCoeff;
  }

  if (getInt("lpGain")) {  enab[0] = 1;  }
  if (getInt("lpc")) {  enab[1] = 1;  }

  cFunctionalComponent::fetchConfig();
  nEnab += order - firstCoeff - 1;

  if (acf == NULL) acf = (FLOAT_DMEM *)malloc(sizeof(FLOAT_DMEM)*(order+1));
  if (lpc == NULL) lpc = (FLOAT_DMEM *)malloc(sizeof(FLOAT_DMEM)*(order+1));
}

const char* cFunctionalLpc::getValueName(long i)
{
  if (i >= IDX_VAR_FUNCTS) {
    const char *n = cFunctionalComponent::getValueName(1);
    // append coefficient number
    if (tmpstr != NULL) free(tmpstr);
    tmpstr = myvprint("%s%i",n,(i-1)+firstCoeff);
    return tmpstr;
  } else {
    return cFunctionalComponent::getValueName(i);
  }
}

long cFunctionalLpc::process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM *out, long Nin, long Nout)
{
  long i;
  if ((Nin>0)&&(out!=NULL)) {
    //Durbin on input values...

    FLOAT_DMEM gain = 0.0;
    FLOAT_DMEM *refl = NULL; /* TODO: add support for reflection coefficients */

    smileDsp_autoCorr(in, Nin, acf, order+1);
    smileDsp_calcLpcAcf(acf, lpc, order, &gain, refl);

    if (enab[0]) *(out++) = gain/(FLOAT_DMEM)Nin;

    if (enab[1]) {
      for (i=firstCoeff; i<order; i++) {
        if (i-firstCoeff >= Nout) break;
        *(out++) = lpc[i];
      }
    }

    return enab[1]*(order-firstCoeff)+enab[0];
  }
  return 0;
}

/*
long cFunctionalLpc::process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout)
{

  return 0;
}
*/

cFunctionalLpc::~cFunctionalLpc()
{
  if (tmpstr != NULL) free(tmpstr);
  if (acf != NULL) free(acf);
  if (lpc != NULL) free(lpc);
}

