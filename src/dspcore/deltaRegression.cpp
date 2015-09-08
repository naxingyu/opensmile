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


/*  openSMILE component: delta coefficients

compute delta regression using regression formula

*/



#include <dspcore/deltaRegression.hpp>
//#include <math.h>

#define MODULE "cDeltaRegression"


SMILECOMPONENT_STATICS(cDeltaRegression)

SMILECOMPONENT_REGCOMP(cDeltaRegression)
{
  SMILECOMPONENT_REGCOMP_INIT
    scname = COMPONENT_NAME_CDELTAREGRESSION;
  sdescription = COMPONENT_DESCRIPTION_CDELTAREGRESSION;

  // we inherit cWindowProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cWindowProcessor")
    SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("nameAppend", NULL, "de");
  ct->setField("deltawin","Specifies the size of half of the delta regression window (If set to 0, a simple difference x[n]-x[n-1] will be computed)",2);
  ct->setField("absOutput","1/0 = on/off : Output absolute value of delta regression or difference function (i.e. do full wave rectification on output signal).",0);
  ct->setField("halfWaveRect","1/0 = on/off : Do half-wave rectification on output values (i.e. keep only positive values and set negative values to 0). Please note that 'halfWaveRect' overrides the 'absOutput' option.",0);
  ct->setField("onlyInSegments","1/0 = on/off : Don't compute deltas at segment boundaries. Segments are bounded by one or more NaN values, or zeros, if zeroSegBound=1 (default)",0);
  ct->setField("zeroSegBound","1/0 = on/off : Consider zeros as segment boundaries (in conjunction with onlyInSegments option).",1);
  ct->setField("blocksize", NULL , 1);
  )
    SMILECOMPONENT_MAKEINFO(cDeltaRegression);
}

SMILECOMPONENT_CREATE(cDeltaRegression)

//-----

cDeltaRegression::cDeltaRegression(const char *_name) :
cWindowProcessor(_name),
deltawin(0), halfWaveRect(0), absOutput(0),
norm(1.0)
{
}


void cDeltaRegression::fetchConfig()
{
  cWindowProcessor::fetchConfig();

  halfWaveRect = getInt("halfWaveRect");
  SMILE_IDBG(2,"halfWaveRect = %i",halfWaveRect);
  if (!halfWaveRect) {
    absOutput = getInt("absOutput");
    SMILE_IDBG(2,"absOutput = %i",absOutput);
  }

  deltawin = getInt("deltawin");
  SMILE_IDBG(2,"deltawin = %i",deltawin);
  if (deltawin < 0) {
    SMILE_IERR(1,"deltawin must be >= 0 ! (setting to 0)");
    deltawin = 0;
  }

  int i;
  norm = 0.0;
  for (i=1; i<=deltawin; i++) norm += (FLOAT_DMEM)i*(FLOAT_DMEM)i;
  norm *= 2.0;

  if (deltawin > 0) {
    setWindow(deltawin,deltawin);
  } else {
    setWindow(1,0);
  }

  onlyInSegments=getInt("onlyInSegments");
  zeroSegBound=getInt("zeroSegBound");
}

/*
int cDeltaRegression::setupNamesForField(int i, const char*name, long nEl)
{
char *tmp = myvprint("%s_de",name);
writer->addField( name, nEl );
return nEl;
}
*/

// order is the amount of memory (overlap) that will be present in _in
// buf will have nT timesteps, however also order negative indicies (i.e. you may access a negative array index up to -order!)
int cDeltaRegression::processBuffer(cMatrix *_in, cMatrix *_out, int _pre, int _post )
{
  long n;
  int i;
  FLOAT_DMEM num;
  if (_in->type!=DMEM_FLOAT) COMP_ERR("dataType (%i) != DMEM_FLOAT not yet supported!",_in->type);
  FLOAT_DMEM *x=_in->dataF;
  FLOAT_DMEM *y=_out->dataF;

  if (deltawin > 0) {
    if (onlyInSegments) {
      for (n=0; n<_out->nT; n++) {
        num = 0.0;
        FLOAT_DMEM _norm = 0.0;
        for (i=1; i<=deltawin; i++) {
          if (!(isNoValue(x[n+i]) || isNoValue(x[n-i]))) {
            num += (FLOAT_DMEM)i * (x[n+i] - x[n-i]);
            norm += (FLOAT_DMEM)i*(FLOAT_DMEM)i;
          }
        }
        if (norm != 0.0) {
          y[n] = num / norm;
        } else {
          y[n] = 0.0;
        }
      }
    } else {
      for (n=0; n<_out->nT; n++) {
        num = 0.0;
        for (i=1; i<=deltawin; i++) num += (FLOAT_DMEM)i * (x[n+i] - x[n-i]);
        y[n] = num / norm;
      }
    }
  } else { // simple difference
    if (onlyInSegments) {
      for (n=0; n<_out->nT; n++) {
        if (isNoValue(x[n]) || isNoValue(x[n-1])) {
          y[n] = 0.0; 
        } else {
          y[n] = x[n]-x[n-1];
        }
      }
    } else {
      for (n=0; n<_out->nT; n++) {
        y[n] = x[n]-x[n-1];
      }
    }
  }

  if (halfWaveRect) {
    for (n=0; n<_out->nT; n++) {
      if (y[n] < 0.0) y[n] = 0.0;
    }
  } else if (absOutput) {
    for (n=0; n<_out->nT; n++) {
      if (y[n] < 0.0) y[n] = -y[n];
    }
  }

  return 1;
}


cDeltaRegression::~cDeltaRegression()
{
}

