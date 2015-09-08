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

functionals: statistical moments

*/



#include <functionals/functionalMoments.hpp>

#define MODULE "cFunctionalMoments"


#define FUNCT_VAR        0
#define FUNCT_STDDEV     1
#define FUNCT_SKEWNESS   2
#define FUNCT_KURTOSIS   3
#define FUNCT_AMEAN      4
#define FUNCT_STDDEV_NORM   5

#define N_FUNCTS  6

#define NAMES     "variance","stddev","skewness","kurtosis","amean","stddevNorm"

const char *momentsNames[] = {NAMES};  // change variable name to your class...

SMILECOMPONENT_STATICS(cFunctionalMoments)

SMILECOMPONENT_REGCOMP(cFunctionalMoments)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CFUNCTIONALMOMENTS;
  sdescription = COMPONENT_DESCRIPTION_CFUNCTIONALMOMENTS;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE

  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("variance","1/0=enable/disable output of variance",1);
    ct->setField("stddev","1/0=enable/disable output of standard deviation",1);
    ct->setField("skewness","1/0=enable/disable output of skewness",1);
    ct->setField("kurtosis","1/0=enable/disable output of kurtosis",1);
    ct->setField("amean","1/0=enable/disable output of arithmetic mean",0);
    ct->setField("stddevNorm","1/0=enable/disable output of standard deviation normalised to the mean (stddev/mean, also referred to as 'coefficient of variance').", 0);
  )

  SMILECOMPONENT_MAKEINFO_NODMEM(cFunctionalMoments);
}

SMILECOMPONENT_CREATE(cFunctionalMoments)

//-----

cFunctionalMoments::cFunctionalMoments(const char *_name) :
  cFunctionalComponent(_name,N_FUNCTS,momentsNames)
{
}

void cFunctionalMoments::fetchConfig()
{
  if (getInt("variance")) enab[FUNCT_VAR] = 1;
  if (getInt("stddev")) enab[FUNCT_STDDEV] = 1;
  if (getInt("skewness")) enab[FUNCT_SKEWNESS] = 1;
  if (getInt("kurtosis")) enab[FUNCT_KURTOSIS] = 1;
  if (getInt("amean")) enab[FUNCT_AMEAN] = 1;
  if (getInt("stddevNorm")) enab[FUNCT_STDDEV_NORM] = 1;
  cFunctionalComponent::fetchConfig();
}

long cFunctionalMoments::process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM min, FLOAT_DMEM max, FLOAT_DMEM mean, FLOAT_DMEM *out, long Nin, long Nout)
{
  int i;
  if ((Nin>0)&&(out!=NULL)) {
    double m2 = 0.0;
    double m3 = 0.0;
    double m4 = 0.0;
    double Nind = (double)Nin;
    double tmp, tmp2;
    FLOAT_DMEM *in0=in;
    double meanD = (double)mean;
    double mean1 = (double)mean;
    if (mean1 == 0.0) {
      mean1 = 1.0;
    }
    for (i=0; i<Nin; i++) {
      tmp = ((double)*(in++) - meanD); // ?? * p(x) ??
      tmp2 = tmp * tmp;
      m2 += tmp2;
      tmp2 *= tmp;
      m3 += tmp2;
      m4 += tmp2*tmp;
    }
    m2 /= Nind;  // variance

    int n=0;
    if (enab[FUNCT_VAR]) out[n++]=(FLOAT_DMEM)m2;
    double sqm2=sqrt(m2);
    if (enab[FUNCT_STDDEV]) {
      if (m2 > 0.0) out[n++]=(FLOAT_DMEM)sqm2;
      else out[n++] = 0.0;
    }
    if (enab[FUNCT_SKEWNESS]) {
      if (m2 > 0.0) out[n++]=(FLOAT_DMEM)( m3/(Nind*m2*sqm2) );
      else out[n++] = 0.0;
    }
    if (enab[FUNCT_KURTOSIS]) {
      if (m2 > 0.0) out[n++]=(FLOAT_DMEM)( m4/(Nind*m2*m2) );
      else out[n++] = 0.0;
    }
    if (enab[FUNCT_AMEAN]) out[n++]=(FLOAT_DMEM)mean;
    if (enab[FUNCT_STDDEV_NORM]) {
      double sqm2N = sqm2 / mean1;
      if (m2 > 0.0) out[n++]=(FLOAT_DMEM)sqm2N;
      else out[n++] = 0.0;
    }
    return n;
  }
  return 0;
}

/*
long cFunctionalMoments::process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout)
{

  return 0;
}
*/

cFunctionalMoments::~cFunctionalMoments()
{
}

