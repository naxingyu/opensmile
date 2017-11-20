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

functionals: relative position of first onset and last offset 

*/


#include <functionals/functionalOnset.hpp>

#define MODULE "cFunctionalOnset"


#define FUNCT_ONSETPOS       0  // number of segments (relative to maxNumSeg)
#define FUNCT_OFFSETPOS      1  // mean length of segment (relative to input length)
#define FUNCT_NUMONSETS      2  // number of segments (relative to maxNumSeg)
#define FUNCT_NUMOFFSETS     3  // mean length of segment (relative to input length)
#define FUNCT_ONSETRATE      4  // rate of onsets

#define N_FUNCTS  5

#define NAMES     "onsetPos","offsetPos","numOnsets","numOffsets","onsetRate"

const char *onsetNames[] = {NAMES};  

SMILECOMPONENT_STATICS(cFunctionalOnset)

SMILECOMPONENT_REGCOMP(cFunctionalOnset)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CFUNCTIONALONSET;
  sdescription = COMPONENT_DESCRIPTION_CFUNCTIONALONSET;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("threshold","The absolute threshold used for onset/offset detection (i.e. the first onset will be where the input value is above the threshold for the first time)",0.0);
    ct->setField("thresholdOnset","A separate threshold only for onset detection. This will override the 'threshold' option, if set",0.0,0,0);
    ct->setField("thresholdOffset","A separate threshold only for offset detection. This will override the 'threshold' option, if set",0.0,0,0);
    ct->setField("useAbsVal","1/0=yes/no : apply thresholds to absolute input value instead of original input value",0);
    ct->setField("onsetPos","1/0=enable/disable output of relative position (relative to the input segment length, in seconds, or in frames, see the 'norm' option or the 'masterTimeNorm' option of the cFunctionals parent component) of first onset found [output name: onsetPos]",0);
    ct->setField("offsetPos","1/0=enable/disable output of position of last offset found (relative to the input segment length, in seconds, or in frames, see the 'norm' option or the 'masterTimeNorm' option of the cFunctionals parent component) [output name: offsetPos]",0);
    ct->setField("numOnsets","1/0=enable/disable output of the number of onsets found [output name: numOnsets]",1);
    ct->setField("numOffsets","1/0=enable/disable output of the number of offsets found (this is usually redundant and the same as numOnsets, use this only for special applications where it may make sense to use it) [output name: numOffsets]",0);
    ct->setField("onsetRate","1/0=enable/disable output of on-set rate, i.e. the number of onsets per second.",0);
    ct->setField("norm","This option specifies how this component should normalise times (if it generates output values related to durations): \n   'segment' (or: 'turn') : normalise to the range 0..1, the result is the relative length wrt. to the segment length )\n   'second'  (absolute time in seconds) \n   'frame' (absolute time in number of frames of input level)","segment",0,0);
  )

  SMILECOMPONENT_MAKEINFO_NODMEM(cFunctionalOnset);
}

SMILECOMPONENT_CREATE(cFunctionalOnset)

//-----

cFunctionalOnset::cFunctionalOnset(const char *_name) :
  cFunctionalComponent(_name,N_FUNCTS,onsetNames)
{
}

void cFunctionalOnset::fetchConfig()
{
  parseTimeNormOption();

  if (getInt("onsetPos")) enab[FUNCT_ONSETPOS] = 1;
  if (getInt("offsetPos")) enab[FUNCT_OFFSETPOS] = 1;
  if (getInt("numOnsets")) enab[FUNCT_NUMONSETS] = 1;
  if (getInt("numOffsets")) enab[FUNCT_NUMOFFSETS] = 1;
  if (getInt("onsetRate")) enab[FUNCT_ONSETRATE] = 1;

  thresholdOffset = thresholdOnset = (FLOAT_DMEM)getDouble("threshold");
  if (isSet("thresholdOnset")) 
    thresholdOnset = (FLOAT_DMEM)getDouble("thresholdOnset");
  if (isSet("thresholdOffset")) 
    thresholdOffset = (FLOAT_DMEM)getDouble("thresholdOffset");

  useAbsVal = getInt("useAbsVal");
  SMILE_IDBG(2,"useAbsVal = %i",useAbsVal);

  SMILE_IDBG(2,"thresholdOnset = %f",thresholdOnset);
  SMILE_IDBG(2,"thresholdOffset = %f",thresholdOffset);

  cFunctionalComponent::fetchConfig();
}

long cFunctionalOnset::process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM *out, long Nin, long Nout)
{
  long i;
  if ((Nin>0)&&(out!=NULL)) {
    long onsetPos = -1;
    long offsetPos = -1;
    long nOnsets = 0;
    long nOffsets = 0;
    int oo = 0; // sttus memory.. last value above threshold = 1
    if (in[0] > thresholdOnset) oo = 1;
    for (i=1; i<Nin; i++) {
      FLOAT_DMEM cur;
      if (useAbsVal) cur = fabs(in[i]);
      else cur = in[i];
      if (cur > thresholdOnset) {
        if (oo == 0) {
          nOnsets++;
          if (onsetPos == -1) onsetPos = i;
          oo = 1;
        }
      }
      if (cur <= thresholdOffset) {
        if (oo == 1) {
          nOffsets++;
          offsetPos = i;
          oo = 0;
        }
      }
    }
    if (offsetPos == -1) offsetPos = Nin-1;
    if (onsetPos == -1) onsetPos = 0;

    int n=0;

    if (timeNorm == TIMENORM_SEGMENT) {
      if (enab[FUNCT_ONSETPOS]) out[n++]=(FLOAT_DMEM)onsetPos/(FLOAT_DMEM)(Nin);
      if (enab[FUNCT_OFFSETPOS]) out[n++]=(FLOAT_DMEM)offsetPos/(FLOAT_DMEM)(Nin);
    } else if (timeNorm == TIMENORM_SECONDS) {
      FLOAT_DMEM _T = (FLOAT_DMEM)getInputPeriod();
      if (enab[FUNCT_ONSETPOS]) out[n++]=(FLOAT_DMEM)onsetPos*_T;
      if (enab[FUNCT_OFFSETPOS]) out[n++]=(FLOAT_DMEM)offsetPos*_T;
    } else if (timeNorm == TIMENORM_FRAMES) {
      if (enab[FUNCT_ONSETPOS]) out[n++]=(FLOAT_DMEM)onsetPos;
      if (enab[FUNCT_OFFSETPOS]) out[n++]=(FLOAT_DMEM)offsetPos;
    }

    if (enab[FUNCT_NUMONSETS]) 
      out[n++]=(FLOAT_DMEM)nOnsets;

    if (enab[FUNCT_NUMOFFSETS]) 
      out[n++]=(FLOAT_DMEM)nOffsets;

    if (enab[FUNCT_ONSETRATE]) { 
      FLOAT_DMEM _T = (FLOAT_DMEM)getInputPeriod();
      out[n++]=(FLOAT_DMEM)nOnsets/((FLOAT_DMEM)Nin * _T);
    }

    return n;
  }
  return 0;
}

/*
long cFunctionalOnset::process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout)
{

  return 0;
}
*/

cFunctionalOnset::~cFunctionalOnset()
{
}

