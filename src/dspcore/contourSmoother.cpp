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


/*  openSMILE component: contour smoother

smooth data contours by moving average filter

*/


#include <dspcore/contourSmoother.hpp>
#include <math.h>

#define MODULE "cContourSmoother"


SMILECOMPONENT_STATICS(cContourSmoother)

SMILECOMPONENT_REGCOMP(cContourSmoother)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CCONTOURSMOOTHER;
  sdescription = COMPONENT_DESCRIPTION_CCONTOURSMOOTHER;

  // we inherit cWindowProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cWindowProcessor")
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("nameAppend", NULL, "sma");
    ct->setField("smaWin","The size of the moving average window. A larger window means more smoothing.",3);
    ct->setField("noZeroSma"," 1 = exclude frames where the element value is 0 from smoothing (i.e. 0 input will be 0 output, and zeros around non-zero values will not be included in the computation of the running average).",0);
    ct->setField("blocksize", NULL ,1);
  )
  SMILECOMPONENT_MAKEINFO(cContourSmoother);
}

SMILECOMPONENT_CREATE(cContourSmoother)

//-----

cContourSmoother::cContourSmoother(const char *_name) :
  cWindowProcessor(_name),
  smaWin(0)
{
}


void cContourSmoother::fetchConfig()
{
  cWindowProcessor::fetchConfig();
  
  noZeroSma = getInt("noZeroSma");
  SMILE_IDBG(2,"noZeroSma = %i",noZeroSma);

  smaWin = getInt("smaWin");
  
  if (smaWin < 1) {
    SMILE_IWRN(1,"smaWin must be >= 1 ! (setting to 1)");
    smaWin = 1;
  }
  if (smaWin % 2 == 0) {
    smaWin++;
    SMILE_IWRN(1,"smaWin must be an uneven number >= 1 ! (increasing smaWin by 1 -> smaWin=%i)",smaWin);
  }
  SMILE_IDBG(2,"smaWin = %i",smaWin);

  setWindow(smaWin/2,smaWin/2);
}

/*
int cContourSmoother::setupNamesForField(int i, const char*name, long nEl)
{
  char *tmp = myvprint("%s_de",name);
  writer->addField( name, nEl );
  return nEl;
}
*/

// order is the amount of memory (overlap) that will be present in _in
// buf will have nT timesteps, however also order negative indicies (i.e. you may access a negative array index up to -order!)
int cContourSmoother::processBuffer(cMatrix *_in, cMatrix *_out, int _pre, int _post )
{
  long n,w;
  if (_in->type!=DMEM_FLOAT) COMP_ERR("dataType (%i) != DMEM_FLOAT not yet supported!",_in->type);
  FLOAT_DMEM *x=_in->dataF;
  FLOAT_DMEM *y=_out->dataF;

  if (noZeroSma) {
    for (n=0; n<_out->nT; n++) {
      if (x[n] != 0.0) {
        long _N=1;
        y[n] = x[n];
        for (w=1; w<=smaWin/2; w++) {
          if (x[n-w] != 0.0) { y[n] += x[n-w]; _N++; }
          if (x[n+w] != 0.0) { y[n] += x[n+w]; _N++; }
        }
        y[n] /= (FLOAT_DMEM)_N;
      } else {
        y[n] = 0.0;
      }
    }
  } else {
    for (n=0; n<_out->nT; n++) {
      y[n] = x[n];
      for (w=1; w<=smaWin/2; w++) {
        y[n] += x[n-w];
        y[n] += x[n+w];
      }
      y[n] /= (FLOAT_DMEM)smaWin;
    }
  }

  return 1;
}


cContourSmoother::~cContourSmoother()
{
}

