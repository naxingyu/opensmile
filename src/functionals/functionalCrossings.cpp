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

functionals: zero-crossings, mean-crossings, arithmetic mean

*/


#include <functionals/functionalCrossings.hpp>

#define MODULE "cFunctionalCrossings"


#define FUNCT_ZCR     0
#define FUNCT_MCR     1
#define FUNCT_AMEAN   2

#define N_FUNCTS  3

#define NAMES     "zcr","mcr","amean"

const char *crossingsNames[] = {NAMES};  // change variable name to your clas...

SMILECOMPONENT_STATICS(cFunctionalCrossings)

SMILECOMPONENT_REGCOMP(cFunctionalCrossings)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CFUNCTIONALCROSSINGS;
  sdescription = COMPONENT_DESCRIPTION_CFUNCTIONALCROSSINGS;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("zcr","1/0=enable/disable output of zero crossing rate",1);
    ct->setField("mcr","1/0=enable/disable output of mean crossing rate (the rate at which the signal crosses its arithmetic mean value (same as zcr for mean normalised signals)",1);
    ct->setField("amean","1/0=enable/disable output of arithmetic mean",0);
  )

  SMILECOMPONENT_MAKEINFO_NODMEM(cFunctionalCrossings);
}

SMILECOMPONENT_CREATE(cFunctionalCrossings)

//-----

cFunctionalCrossings::cFunctionalCrossings(const char *_name) :
  cFunctionalComponent(_name,N_FUNCTS,crossingsNames)
{
}

void cFunctionalCrossings::fetchConfig()
{
  if (getInt("zcr")) enab[FUNCT_ZCR] = 1;
  if (getInt("mcr")) enab[FUNCT_MCR] = 1;
  if (getInt("amean")) enab[FUNCT_AMEAN] = 1;

  cFunctionalComponent::fetchConfig();
}

long cFunctionalCrossings::process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM *out, long Nin, long Nout)
{
  int i;
  if ((Nin>0)&&(out!=NULL)) {
    double amean;
    long zcr=0, mcr=0;

    if (enab[FUNCT_MCR]||enab[FUNCT_AMEAN]) {
      amean = (double)*in;
      for (i=1; i<Nin; i++) {
        amean += in[i];
      }
      amean /= (double)Nin;
    }
    
    for (i=1; i<Nin-1; i++) {
      in++;
      if (  ( (*(in-1) * *(in+1) <= 0.0) && (*(in)==0.0) ) || (*(in-1) * *(in) < 0.0)  ) zcr++;
      if (enab[FUNCT_MCR])
        if (  ( ((*(in-1)-amean) * (*(in+1)-amean) <= 0.0) && ((*(in)-amean)==0.0) ) || ((*(in-1)-amean) * (*(in)-amean) < 0.0)  ) mcr++;
    }
    
    int n=0;
    if (enab[FUNCT_ZCR]) out[n++]=(FLOAT_DMEM) ( (double)zcr / (double)Nin );
    if (enab[FUNCT_MCR]) out[n++]=(FLOAT_DMEM) ( (double)mcr / (double)Nin );
    if (enab[FUNCT_AMEAN]) out[n++]=(FLOAT_DMEM)amean;
    return n;
  }
  return 0;
}

/*
long cFunctionalCrossings::process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout)
{

  return 0;
}
*/

cFunctionalCrossings::~cFunctionalCrossings()
{
}

