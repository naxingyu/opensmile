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


/*  openSMILE component: dbA

applies dbX weighting to fft magnitudes

*/


#include <dsp/dbA.hpp>

#define MODULE "cDbA"

/*Library:
sComponentInfo * registerMe(cConfigManager *_confman) {
  cDataProcessor::registerComponent(_confman);
}
*/

SMILECOMPONENT_STATICS(cDbA)

SMILECOMPONENT_REGCOMP(cDbA)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CDBA;
  sdescription = COMPONENT_DESCRIPTION_CDBA;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")
  SMILECOMPONENT_IFNOTREGAGAIN(
//    ct->setField("nameAppend", NULL, (const);
    ct->setField("curve","1 character, which specifies the type of the curve to use (supported: A ; soon supported: B,C) [NOT YET FULLY IMPLEMENTED, ONLY A is supported]",'A');
    ct->setField("usePower","1 = square the input magnitudes before multiplying with the dX weighting function (the output will then be a dBX weighted power spectrum)",1);
  )
  SMILECOMPONENT_MAKEINFO(cDbA);
}

SMILECOMPONENT_CREATE(cDbA)

//-----

cDbA::cDbA(const char *_name) :
  cVectorProcessor(_name),
  curve(0),
  filterCoeffs(NULL)
{

}

void cDbA::fetchConfig()
{
  cVectorProcessor::fetchConfig();
  
  char c = getChar("curve");
  SMILE_DBG(2,"curve = %c",c);
  if ((c=='A')||(c=='a')) curve = CURVE_DBA;
  if ((c=='B')||(c=='b')) curve = CURVE_DBB;
  if ((c=='C')||(c=='c')) curve = CURVE_DBC;

  usePower = getInt("usePower");
  if (usePower) { SMILE_DBG(2,"using power spectrum"); }
}

/*
int cDbA::myConfigureInstance()
{
  int ret=1;
  ret *= cVectorProcessor::myConfigureInstance();
  if (ret == 0) return 0;

  return ret;
}
*/

/*
int cDbA::configureWriter(const sDmLevelConfig *c)
{
  if (c==NULL) return 0;
  
  // you must return 1, in order to indicate configure success (0 indicated failure)
  return 1;
}
*/

int cDbA::dataProcessorCustomFinalise()
{
  // allocate for multiple configurations..
  if (filterCoeffs == NULL) filterCoeffs = (FLOAT_DMEM**)multiConfAlloc();
  
  return cVectorProcessor::dataProcessorCustomFinalise();
}

int cDbA::setupNamesForField(int i, const char*name, long nEl)
{
  if ((strstr(name,"phase") == NULL)&&(strstr(name,"Phase") == NULL)) {
    const sDmLevelConfig *c = reader_->getLevelConfig();
    computeFilters(nEl, c->frameSizeSec, getFconf(i));

    return cVectorProcessor::setupNamesForField(i,name,nEl);
  } else { return 0; }
}


void computeDBA(FLOAT_DMEM *x, long blocksize, FLOAT_DMEM F0)
{
  FLOAT_DMEM curF = 0.0;
  FLOAT_DMEM tmp,cf2;
  int i;
  
  for (i=0; i < blocksize; i++)
  {
    cf2 = (FLOAT_DMEM)pow(curF,(FLOAT_DMEM)2.0);
    tmp = (FLOAT_DMEM)( pow(12200.0,2.0) * pow(cf2,(FLOAT_DMEM)2.0) ) /  ( (cf2+(FLOAT_DMEM)pow(20.6,2.0)) * (cf2+(FLOAT_DMEM)pow(12200.0,2.0))  );
    tmp /=  (FLOAT_DMEM)( sqrt(cf2+pow(107.7,2.0)) * sqrt(cf2+pow(737.9,2.0)) );
      //obj->db[i] = 10.0*log(ddummy) + 2.0;
    *(x++) = (FLOAT_DMEM) pow (10.0,( (10.0*log(tmp) + 2.0) / 10.0));
      //obj->db[i] = fdummy;
    curF += F0;
  }
}

// blocksize is size of fft block, _T is period of fft frames
// sampling period is reconstructed by: _T/((blocksize-1)*2)
int cDbA::computeFilters( long blocksize, double frameSizeSec, int idxc )
{
  FLOAT_DMEM *_filterCoeffs = filterCoeffs[idxc];
  FLOAT_DMEM F0 = (FLOAT_DMEM)(1.0/frameSizeSec);

  if (_filterCoeffs != NULL) { free(_filterCoeffs);   }
  _filterCoeffs = (FLOAT_DMEM *)malloc(sizeof(FLOAT_DMEM) * blocksize);

  // dbA::
  computeDBA(_filterCoeffs, blocksize, F0);
  // TODO::: support dbB, dbC, etc.

  filterCoeffs[idxc] = _filterCoeffs;
  return 1;
}


/*
int cDbA::myFinaliseInstance()
{
  int ret;
  ret = cVectorProcessor::myFinaliseInstance();
  return ret;
}
*/

// a derived class should override this method, in order to implement the actual processing
/*
int cDbA::processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  // do domething to data in *src, save result to *dst
  // NOTE: *src and *dst may be the same...
  
  return 1;
}
*/

// a derived class should override this method, in order to implement the actual processing
int cDbA::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  int n;
  FLOAT_DMEM *db = filterCoeffs[getFconf(idxi)];

  // do the dbX filtering by multiplying with the filters and summing up
  bzero(dst, Ndst*sizeof(FLOAT_DMEM));

  // copy & square the fft magnitude
  FLOAT_DMEM *_src;
  if (usePower) {
    _src = (FLOAT_DMEM*)malloc(sizeof(FLOAT_DMEM)*Nsrc);
    if (src==NULL) OUT_OF_MEMORY;
    for (n=0; n<Nsrc; n++) {
      _src[n] = src[n]*src[n];
    }
    src = _src;
  }

  for (n=0; n<Ndst; n++) {
    *(dst++) = *(src++) * *(db++);
  }

  if ((usePower)&&(_src!=NULL)) free((void *)_src);

  return 1;
}

cDbA::~cDbA()
{
  multiConfFree(filterCoeffs);
}

