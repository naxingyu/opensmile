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

functionals: extreme values and ranges

*/


#include <functionals/functionalExtremes.hpp>

#define MODULE "cFunctionalExtremes"

#define NORM_TURN     0
#define NORM_SECOND   1
#define NORM_FRAME    2

#define FUNCT_MAX     0
#define FUNCT_MIN     1
#define FUNCT_RANGE   2
#define FUNCT_MAXPOS   3
#define FUNCT_MINPOS   4
#define FUNCT_AMEAN   5
#define FUNCT_MAXAMEANDIST   6
#define FUNCT_MINAMEANDIST   7

#define N_FUNCTS  8

#define NAMES     "max","min","range","maxPos","minPos","amean","maxameandist","minameandist"

const char *extremesNames[] = {NAMES};  // change variable name to your clas...

SMILECOMPONENT_STATICS(cFunctionalExtremes)

SMILECOMPONENT_REGCOMP(cFunctionalExtremes)
{
  SMILECOMPONENT_REGCOMP_INIT
  
  scname = COMPONENT_NAME_CFUNCTIONALEXTREMES;
  sdescription = COMPONENT_DESCRIPTION_CFUNCTIONALEXTREMES;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE

  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("max","1/0=enable/disable output of maximum value",1);
    ct->setField("min","1/0=enable/disable output of minimum value",1);
    ct->setField("range","1/0=enable/disable output of range (max-min)",1);
    ct->setField("maxpos","1/0=enable/disable output of position of maximum value (relative to the input segment length, in seconds, or in frames, see the 'norm' option or the 'masterTimeNorm' option of the cFunctionals parent component)",1);
    ct->setField("minpos","1/0=enable/disable output of position of minimum value (relative to the input segment length, in seconds, or in frames, see the 'norm' option or the 'masterTimeNorm' option of the cFunctionals parent component)",1);
    ct->setField("amean","1/0=enable/disable output of arithmetic mean",0);
    ct->setField("maxameandist","1/0=enable/disable output of (max-arithmetic_mean)",1);
    ct->setField("minameandist","1/0=enable/disable output of (arithmetic_mean-min)",1);
    // Please note: the default is "frame" here, due to compatibility with older versions. In new setting you should, however, always use "frame"
    ct->setField("norm","This option specifies how this component should normalise times (if it generates output values related to durations): \n   'segment' (or: 'turn') : normalise to the range 0..1, the result is the relative length wrt. to the segment length )\n   'second'  (absolute time in seconds) \n   'frame' (absolute time in number of frames of input level)","frames");
  )

  SMILECOMPONENT_MAKEINFO_NODMEM(cFunctionalExtremes);
}

SMILECOMPONENT_CREATE(cFunctionalExtremes)

//-----

cFunctionalExtremes::cFunctionalExtremes(const char *_name) :
  cFunctionalComponent(_name,N_FUNCTS,extremesNames)
{
}

void cFunctionalExtremes::fetchConfig()
{
  parseTimeNormOption();

  if (getInt("max")) enab[FUNCT_MAX] = 1;
  if (getInt("min")) enab[FUNCT_MIN] = 1;
  if (getInt("range")) enab[FUNCT_RANGE] = 1;
  if (getInt("maxpos")) enab[FUNCT_MAXPOS] = 1;
  if (getInt("minpos")) enab[FUNCT_MINPOS] = 1;
  if (getInt("amean")) enab[FUNCT_AMEAN] = 1;
  if (getInt("maxameandist")) enab[FUNCT_MAXAMEANDIST] = 1;
  if (getInt("minameandist")) enab[FUNCT_MINAMEANDIST] = 1;

  cFunctionalComponent::fetchConfig();
}

long cFunctionalExtremes::process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM min, FLOAT_DMEM max, FLOAT_DMEM mean, FLOAT_DMEM *out, long Nin, long Nout)
{
  int i;
  if ((Nin>0)&&(out!=NULL)) {
    long minpos=-1, maxpos=-1;
    
    for (i=0; i<Nin; i++) {
      if ((*in == max)&&(maxpos==-1)) { maxpos=i; }
      if ((*in == min)&&(minpos==-1)) { minpos=i; }
      in++;
    }

    FLOAT_DMEM maxposD = (FLOAT_DMEM)maxpos;
    FLOAT_DMEM minposD = (FLOAT_DMEM)minpos;

    // normalise max/min pos ...
    if (timeNorm==TIMENORM_SEGMENT) {
        maxposD /= (FLOAT_DMEM)(Nin);
        minposD /= (FLOAT_DMEM)(Nin);
    } else if (timeNorm==NORM_SECOND) {
      FLOAT_DMEM _T = (FLOAT_DMEM)getInputPeriod();
                           
      if (_T != 0.0) {
        maxposD *= _T;
        minposD *= _T;
      }
    } // default is TIMENORM_FRAME...

    int n=0;
    if (enab[FUNCT_MAX]) out[n++]=max;
    if (enab[FUNCT_MIN]) out[n++]=min;
    if (enab[FUNCT_RANGE]) out[n++]=max-min;
    if (enab[FUNCT_MAXPOS]) out[n++]=maxposD;
    if (enab[FUNCT_MINPOS]) out[n++]=minposD;
    if (enab[FUNCT_AMEAN]) out[n++]=mean;
    if (enab[FUNCT_MAXAMEANDIST]) out[n++]=max-mean;
    if (enab[FUNCT_MINAMEANDIST]) out[n++]=mean-min;
    return n;
  }
  return 0;
}

/*
long cFunctionalExtremes::process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout)
{

  return 0;
}
*/

cFunctionalExtremes::~cFunctionalExtremes()
{
}

