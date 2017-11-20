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

TODO:
Average Magnitude Difference Function (AMDF)

this implements a simple windowed AMDF


(further TODO: continuous AMDF of fixed delay and decay -> WindowProcessor)
*/


#include <dspcore/amdf.hpp>

#define MODULE "cAmdf"

/*Library:
sComponentInfo * registerMe(cConfigManager *_confman) {
cDataProcessor::registerComponent(_confman);
}
*/

SMILECOMPONENT_STATICS(cAmdf)

SMILECOMPONENT_REGCOMP(cAmdf)
//sComponentInfo * cAmdf::registerComponent(cConfigManager *_confman)
{
  SMILECOMPONENT_REGCOMP_INIT
    scname = COMPONENT_NAME_CAMDF;
  sdescription = COMPONENT_DESCRIPTION_CAMDF;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")

    SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("nameAppend",NULL,"amdf");
    ct->setField("nLag","If nLag > 0, compute AMDF up to maximum lag 'nLag' (or maximum possible lag limited by framesize). If nLag=0, then always the maximum framesize will be used (this is the default). If nLag < 0, then nLag=framesize/((-1)*nLag) will be used.",0);
    ct->setField("method","The AMDF computation method (i.e. the handling of border conditions):\n    'limit' :  compute AMDF only in regions where the shifted windows overlap\n    'warp' : compute cyclical AMDF by warping of input\n    'zeropad' : zero pad missing samples","limit");
    ct->setField("invert","1 = invert the AMDF output values (literally '1-amdf'), i.e. so that the behaviour of the AMDF output corresponds more to that of an autocorrelation function.",0);

  )
    SMILECOMPONENT_MAKEINFO(cAmdf);
}

SMILECOMPONENT_CREATE(cAmdf)

//-----

cAmdf::cAmdf(const char *_name) :
cVectorProcessor(_name),
nLag(1),
method(AMDF_LIMIT),
invert(0)
{

}

void cAmdf::fetchConfig()
{
  cVectorProcessor::fetchConfig();

  const char *_method = getStr("method");
  if (_method != NULL) {
    if (!strcmp(_method,"limit")) {
      method = AMDF_LIMIT;
    } else
      if (!strcmp(_method,"warp")) {
        method = AMDF_WARP;
      } else
        if (!strcmp(_method,"zeropad")) {
          method = AMDF_ZEROPAD;
        }
  }
  nLag = getInt("nLag");
  invert = getInt("invert");
}

/*
int cAmdf::myConfigureInstance()
{
int ret=1;
ret *= cVectorProcessor::myConfigureInstance();
if (ret == 0) return 0;

//...


return ret;
}
*/

/*
int cAmdf::configureWriter(const sDmLevelConfig *c)
{

// you must return 1, in order to indicate configure success (0 indicated failure)
return 1;
}

*/


int cAmdf::setupNamesForField(int i, const char*name, long nEl)
{
  if (nLag == 0) nLag = nEl-1;
  if (nLag < 0) nLag = (nEl)/((-1)*nLag) - 1;
  if (nLag < 1) nLag = 1;
  if (nLag > nEl-1) nLag = nEl-1;
  SMILE_DBG(2,"nLag = %i",nLag);
  return cVectorProcessor::setupNamesForField(i,name,nLag);
}


/*
int cAmdf::myFinaliseInstance()
{
int ret=1;
ret *= cVectorProcessor::myFinaliseInstance();
//.....
return ret;
}
*/

/*
// a derived class should override this method, in order to implement the actual processing
int cAmdf::processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
// do domething to data in *src, save result to *dst
// NOTE: *src and *dst may be the same...

return 0;
}
*/

// SMILElib function:
// compute AMDF
int computeAMDF(const FLOAT_DMEM *in, FLOAT_DMEM *out, long nIn, long nOut)
{ // assumption: nOut = nLag!
  long j; // lag j
  FLOAT_DMEM tmp;
  const FLOAT_DMEM *in1, *in2, *end;
  *(out++) = 0.0;
  for (j=1; j<nOut; j++) {
    tmp=0.0;
    in1 = in;
    end = in+nIn-j;
    in2 = in+j;
    while (in1<end) {
      tmp += fabs( *(in1++) - *(in2++) );
    }
    *(out++) = (tmp)/(FLOAT_DMEM)(nIn-j);
  }
  return 1;
}

// SMILElib function:
// compute warped AMDF from a vector
int computeAMDFwarped(const FLOAT_DMEM *in, FLOAT_DMEM *out, long nIn, long nOut, int invert=0)
{ // assumption: nOut = nLag!
  long j; // lag j
  FLOAT_DMEM tmp;
  const FLOAT_DMEM *in1, *in2, *end;
  *(out) = 0.0;

  for (j=1; j<nOut; j++) {
    //printf("j=%i\n",j);
    tmp=0.0;
    in1 = in;
    end = in+nIn;
    in2 = in+j;
    //long tt = 0;
    while (in2<end) {
      //while (tt<nIn-j) {
      tmp += fabs( *(in1++) - *(in2++) );
      //	tt++;
    }
    in2 = in;

    while (in1<end) {
      //while (tt<nIn) {
      tmp += fabs( *(in1++) - *(in2++) );
      //tt++;
    }
    out[j] = (tmp)/(FLOAT_DMEM)(nIn);
  }
  return 1;
}

// SMILElib function:
// compute zeropadded AMDF from a vector
int computeAMDFzeropad(const FLOAT_DMEM *in, FLOAT_DMEM *out, long nIn, long nOut)
{ // assumption: nOut = nLag!
  long j; // lag j
  FLOAT_DMEM tmp;
  const FLOAT_DMEM *in1, *in2, *end1, *end2;
  *(out++) = 0.0;
  for (j=1; j<nOut; j++) {
    tmp=0.0;
    in1 = in;
    end1 = in+nIn-j;
    end2 = in+nIn;
    in2 = in+j;
    while (in1<end1) {
      tmp += fabs( *(in1++) - *(in2++) );
    }
    while (in1<end2) { // TODO: avoid double computation effort here.... sum up only once and then look up cummulative value
      tmp += fabs( *(in1++) );
    }
    *(out++) = (tmp)/(FLOAT_DMEM)(nIn);
  }
  return 1;
}

// a derived class should override this method, in order to implement the actual processing
int cAmdf::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  // do domething to data in *src, save result to *dst
  // NOTE: *src and *dst may be the same...
  if (Nsrc == 0) return 0;

  switch (method) {
    case AMDF_LIMIT:  computeAMDF(src,dst,Nsrc,Ndst); break;
    case AMDF_WARP:  computeAMDFwarped(src,dst,Nsrc,Ndst, invert); break;
    case AMDF_ZEROPAD:  computeAMDFzeropad(src,dst,Nsrc,Ndst); break;
    default : {
      SMILE_ERR(1,"unknown AMDF computation method: %i",method);
      return 0;
              }
  }

  if (invert) {
    FLOAT_DMEM max = 0.0;
    long j;
    for (j=0; j<Ndst; j++)
      if (dst[j] > max ) max = dst[j];
    for (j=0; j<Ndst; j++)
      dst[j] = max - dst[j];
  }

  return 1;
}

cAmdf::~cAmdf()
{
}

