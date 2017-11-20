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

// is called by cFunctionals::setupNamesForElement after each new field has been added
void cFunctionalComponent::setFieldMetaData(cDataWriter *writer,
    const FrameMetaInfo *fmeta, int idxi, long nEl) {
  double * buf = (double *)malloc(fmeta->field[idxi].infoSize);
  memcpy(buf, fmeta->field[idxi].info, fmeta->field[idxi].infoSize);
  writer->setFieldInfo(-1 /* last field added */,
      fmeta->field[idxi].dataType, buf,
      fmeta->field[idxi].infoSize);
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

