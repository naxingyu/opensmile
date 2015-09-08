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

