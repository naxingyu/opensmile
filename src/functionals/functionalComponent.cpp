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

a single statistical functional or the like....

*/


#include <functionals/functionalComponent.hpp>

#define MODULE "cFunctionalComponent"


SMILECOMPONENT_STATICS(cFunctionalComponent)

SMILECOMPONENT_REGCOMP(cFunctionalComponent)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CFUNCTIONALCOMPONENT;
  sdescription = COMPONENT_DESCRIPTION_CFUNCTIONALCOMPONENT;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
  //  ConfigType *ct = new ConfigType(scname); if (ct == NULL) OUT_OF_MEMORY;
  SMILECOMPONENT_IFNOTREGAGAIN( {} )
  //   ct->setField(""...);
 
  SMILECOMPONENT_MAKEINFO_NODMEM_ABSTRACT(cFunctionalComponent);
}

SMILECOMPONENT_CREATE_ABSTRACT(cFunctionalComponent)

//-----

cFunctionalComponent::cFunctionalComponent(const char *_name, int _nTotal, const char*_names[]) :
  cSmileComponent(_name),
  nTotal(_nTotal),
  enab(NULL),
  functNames(_names),
  nEnab(0),
  timeNorm(TIMENORM_SEGMENT),
  timeNormIsSet(0),
  T(0.0)
{
  if (nTotal>0)
    enab = (int*)calloc(1,sizeof(int)*nTotal);
}

void cFunctionalComponent::parseTimeNormOption()
{
  if (isSet("norm")) { timeNormIsSet = 1; }

  const char*Norm = getStr("norm");
  if (Norm != NULL) {
    if (!strncmp(Norm,"tur",3)) timeNorm=TIMENORM_SEGMENT;
    else if (!strncmp(Norm,"seg",3)) timeNorm=TIMENORM_SEGMENT;
    else if (!strncmp(Norm,"sec",3)) timeNorm=TIMENORM_SECOND;
    else if (!strncmp(Norm,"fra",3)) timeNorm=TIMENORM_FRAME;
  }

  SMILE_IDBG(2,"timeNorm = %i (set= %i)\n",timeNorm,timeNormIsSet);
}

void cFunctionalComponent::fetchConfig()
{
  int i; 
  for (i=0; i<nTotal; i++) {
    if (enab[i]) nEnab++;
  }
}

const char* cFunctionalComponent::getValueName(long i)
{
  int j=0; int n=-1;
  if (functNames==NULL) return NULL;
  for (j=0; j<nTotal; j++) {
    if (enab[j]) n++;
    if (i==n) return functNames[j];
  }
  return NULL;
}

long cFunctionalComponent::process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM *out, long Nin, long Nout)
{
  SMILE_ERR(1,"dataType FLOAT_DMEM not yet supported in component '%s' of type '%s'",getTypeName(), getInstName());
  return 0;
}

long cFunctionalComponent::process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout)
{
  SMILE_ERR(1,"dataType INT_DMEM not yet supported in component '%s' of type '%s'",getTypeName(), getInstName());
  return 0;
}

cFunctionalComponent::~cFunctionalComponent()
{
  if (enab!=NULL) free(enab);
}

