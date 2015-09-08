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

functionals: linear and quadratic regression coefficients

*/


#include <functionals/functionalRegression.hpp>

#define MODULE "cFunctionalRegression"


#define FUNCT_LINREGC1     0
#define FUNCT_LINREGC2     1
#define FUNCT_LINREGERRA   2
#define FUNCT_LINREGERRQ   3
#define FUNCT_QREGC1       4
#define FUNCT_QREGC2       5
#define FUNCT_QREGC3       6
#define FUNCT_QREGERRA     7
#define FUNCT_QREGERRQ     8
#define FUNCT_CENTROID     9
#define FUNCT_QREGLS       10  // left slope of parabola
#define FUNCT_QREGRS       11  // right slope of parabola
#define FUNCT_QREGX0       12  // vertex coordinates
#define FUNCT_QREGY0       13  // vertex coordinates
//#define FUNCT_QREGYL       14  // left value (t=0) of parabola  == qregc3 !!
#define FUNCT_QREGYR       14  // right value (t=Nin) of parabola

#define FUNCT_QREGY0NN    15  // vertex y coordinate, not normalised
#define FUNCT_QREGC3NN    16  // qregc3, not normalised
#define FUNCT_QREGYRNN    17  // not normalised

#define N_FUNCTS  18

#define NAMES  "linregc1","linregc2","linregerrA","linregerrQ","qregc1","qregc2","qregc3","qregerrA","qregerrQ","centroid","qregls","qregrs","qregx0","qregy0","qregyr","qregy0nn","qregc3nn","qregyrnn"

const char *regressionNames[] = {NAMES};  // change variable name to your clas...

SMILECOMPONENT_STATICS(cFunctionalRegression)

SMILECOMPONENT_REGCOMP(cFunctionalRegression)
{
  SMILECOMPONENT_REGCOMP_INIT
  
  scname = COMPONENT_NAME_CFUNCTIONALREGRESSION;
  sdescription = COMPONENT_DESCRIPTION_CFUNCTIONALREGRESSION;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("linregc1","1/0=enable/disable output of slope m (linear regression line)",1);
    ct->setField("linregc2","1/0=enable/disable output of offset t (linear regression line)",1);
    ct->setField("linregerrA","1/0=enable/disable output of linear error between contour and linear regression line",1);
    ct->setField("linregerrQ","1/0=enable/disable output of quadratic error between contour and linear regression line",1);
    ct->setField("qregc1","1/0=enable/disable output of quadratic regression coefficient 1 (a)",1);
    ct->setField("qregc2","1/0=enable/disable output of quadratic regression coefficient 2 (b)",1);
    ct->setField("qregc3","1/0=enable/disable output of quadratic regression coefficient 3 (c = offset)",1);
    ct->setField("qregerrA","1/0=enable/disable output of linear error between contour and quadratic regression line (parabola)",1);
    ct->setField("qregerrQ","1/0=enable/disable output of quadratic error between contour and quadratic regression line (parabola)",1);
    ct->setField("centroid","1/0=enable/disable output of centroid of contour (this is computed as a by-product of the regression coefficients).",1);
    ct->setField("centroidNorm","normalise time-scale of centroid to time in seconds (seconds), frame index (frame), or relative segment percentage (segment).", "segment");
    ct->setField("qregls","1/0=enable/disable output of left slope of parabola (slope of the line from first point on the parabola at t=0 to the vertex).",0);
    ct->setField("qregrs","1/0=enable/disable output of right slope of parabola (slope of the line from the vertex to the last point on the parabola at t=N).",0);
    ct->setField("qregx0","1/0=enable/disable output of x coordinate of the parabola vertex (since for very flat parabolas this can be very large/small, it is clipped to range -Nin - +Nin ).",0);
    ct->setField("qregy0","1/0=enable/disable output of y coordinate of the parabola vertex.",0);
    ct->setField("qregyr","1/0=enable/disable output of y coordinate of the last point on the parabola (t=N).",0);
    ct->setField("qregy0nn","1/0=enable/disable output of y coordinate of the parabola vertex. This value is unnormalised, regardless of value of normInput.",0);
    ct->setField("qregc3nn","1/0=enable/disable output of y coordinate of the first point on the parabola (t=0). This value is unnormalised, regardless of value of normInput.",0);
    ct->setField("qregyrnn","1/0=enable/disable output of y coordinate of the last point on the parabola (t=N). This value is unnormalised, regardless of value of normInput.",0);

    ct->setField("normRegCoeff","1/0=enable/disable normalisation of regression coefficients, slopes, and coordinates on the time scale. If enabled, the coefficients are scaled (multiplied by the contour length) so that a regression line or parabola approximating the contour can be plotted over an x-axis range from 0 to 1, i.e. this makes the coefficients independent of the contour length (a longer contour with a lower slope will then have the same 'm' (slope) linear regression coefficient as a shorter but steeper slope).",0);
    ct->setField("normInputs","1/0=enable/disable normalisation of regression coefficients, coordinates, and regression errors on the value scale. If enabled all input values will be normalised to the range 0..1. Use this in conjunction with normRegCoeff.",0);
    ct->setField("oldBuggyQerr","Set this to 1 (default) to output the (input lengthwise) unnormalised quadratic regression errors (if qregerr* == 1) for compatibility with older feature sets. In new setups you should always change from the default to 0 to enable the proper scaling of the quadratic error!",1);

  )

  SMILECOMPONENT_MAKEINFO_NODMEM(cFunctionalRegression);
}

SMILECOMPONENT_CREATE(cFunctionalRegression)

//-----

cFunctionalRegression::cFunctionalRegression(const char *_name) :
  cFunctionalComponent(_name,N_FUNCTS,regressionNames),
  enQreg(0)
{
}

void cFunctionalRegression::fetchConfig()
{
  if (getInt("linregc1")) enab[FUNCT_LINREGC1] = 1;
  if (getInt("linregc2")) enab[FUNCT_LINREGC2] = 1;
  if (getInt("linregerrA")) enab[FUNCT_LINREGERRA] = 1;
  if (getInt("linregerrQ")) enab[FUNCT_LINREGERRQ] = 1;
  if (getInt("qregc1")) { enab[FUNCT_QREGC1] = 1; enQreg=1; }
  if (getInt("qregc2")) { enab[FUNCT_QREGC2] = 1; enQreg=1; }
  if (getInt("qregc3")) { enab[FUNCT_QREGC3] = 1; enQreg=1; }
  if (getInt("qregerrA")) { enab[FUNCT_QREGERRA] = 1; enQreg=1; }
  if (getInt("qregerrQ")) { enab[FUNCT_QREGERRQ] = 1; enQreg=1; }
  if (getInt("centroid")) { enab[FUNCT_CENTROID] = 1; enQreg=1; }
  if (getInt("qregls")) { enab[FUNCT_QREGLS] = 1; enQreg=1; }
  if (getInt("qregrs")) { enab[FUNCT_QREGRS] = 1; enQreg=1; }
  if (getInt("qregx0")) { enab[FUNCT_QREGX0] = 1; enQreg=1; }
  if (getInt("qregy0")) { enab[FUNCT_QREGY0] = 1; enQreg=1; }
  if (getInt("qregyr")) { enab[FUNCT_QREGYR] = 1; enQreg=1; }
  if (getInt("qregy0nn")) { enab[FUNCT_QREGY0NN] = 1; enQreg=1; }
  if (getInt("qregc3nn")) { enab[FUNCT_QREGC3NN] = 1; enQreg=1; }
  if (getInt("qregyrnn")) { enab[FUNCT_QREGYRNN] = 1; enQreg=1; }
  const char *centrNorm = getStr("centroidNorm");
  if (!strncmp(centrNorm, "sec", 3)) {
    centroidNorm = TIMENORM_SECONDS;
  } else if (!strncmp(centrNorm, "fra", 3)) {
    centroidNorm = TIMENORM_FRAMES;
  } else if (!strncmp(centrNorm, "seg", 3)) {
    centroidNorm = TIMENORM_SEGMENT;
  } else {
    SMILE_IERR(1, "unknown value for option 'centroidNorm' (%s). Allowed values are: fra(mes), seg(ments), sec(onds)", centrNorm);
  }
  normRegCoeff = getInt("normRegCoeff");
  normInputs = getInt("normInputs");

  oldBuggyQerr = getInt("oldBuggyQerr");

  cFunctionalComponent::fetchConfig();
}

long cFunctionalRegression::process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM min, FLOAT_DMEM max, FLOAT_DMEM mean, FLOAT_DMEM *out, long Nin, long Nout)
{
  if ((Nin>0)&&(out!=NULL)) {
    //compute centroid
    FLOAT_DMEM *iE = in + Nin;
    FLOAT_DMEM *i0 = in;
    double Nind = (double)Nin;
    double range = max-min;
    if (range <= 0.0) range = 1.0;
    
    double num = 0.0;
    double num2 = 0.0;
    double tmp = 0.0;
    double ii = 0.0;
    double asum = mean*Nind;
    while (in<iE) {
      tmp = (double)(*(in++)) * ii;
      num += tmp;
      tmp *= ii;
      ii += 1.0;
      num2 += tmp;
    }

    double centroid;
    if (asum != 0.0) {
      centroid = num / asum;
    } else {
      centroid = 0.0;
    }

    if (centroidNorm == TIMENORM_SECONDS) {
      centroid *= getInputPeriod();
    } else if (centroidNorm == TIMENORM_SEGMENT) {
      centroid /= Nind;
    }
    in=i0;

    
    double m=0.0,t=0.0,leq=0.0,lea=0.0;
    double a=0.0,b=0.0,c=0.0,qeq=0.0,qea=0.0;
    double S1,S2,S3,S4;
    if (Nin > 1) {
      // LINEAR REGRESSION:
/*
      S1 = (Nind-1.0)*Nind/2.0;  // sum of all i=0..N-1
      S2 = (Nind-1.0)*Nind*(2.0*Nind-1.0)/6.0; // sum of all i^2 for i=0..N-1
                                              // num: if sum of y_i*i for all i=0..N-1
      t = ( asum - num*S1/S2) / ( Nind - S1*S1/S2 );
      m = ( num - t * S1 ) / S2;
*/ // optimised computation:
      double NNm1 = (Nind)*(Nind-(double)1.0);

      S1 = NNm1/(double)2.0;  // sum of all i=0..N-1
      S2 = NNm1*((double)2.0*Nind-(double)1.0)/(double)6.0; // sum of all i^2 for i=0..N-1

      // check!
      double S1dS2 = S1/S2;
      double tmp = ( Nind - S1*S1dS2 );
      if (tmp == 0.0) t = 0.0;
      else t = ( asum - num*S1dS2) / tmp;
      m = ( num - t * S1 ) / S2;

      S3 = S1*S1;
      double Nind1 = Nind-(double)1.0;
      S4 = S2 * ((double)3.0*(Nind1*Nind1 + Nind1)-(double)1.0) / (double)5.0;

      // QUADRATIC REGRESSION:
      if (enQreg) {

        double det;
        double S3S3 = S3*S3;
        double S2S2 = S2*S2;
        double S1S2 = S1*S2;
        double S1S1 = S3;
        det = S4*S2*Nind + (double)2.0*S3*S1S2 - S2S2*S2 - S3S3*Nind - S1S1*S4;

        if (det != 0.0) {
          a = ( (S2*Nind - S1S1)*num2 + (S1S2 - S3*Nind)*num + (S3*S1 - S2S2)*asum ) / det;
          b = ( (S1S2 - S3*Nind)*num2 + (S4*Nind - S2S2)*num + (S3*S2 - S4*S1)*asum ) / det;
          c = ( (S3*S1 - S2S2)*num2 + (S3*S2-S4*S1)*num + (S4*S2 - S3S3)*asum ) / det;
        } else {
          a=0.0; b=0.0; c=0.0;
        }

      }
//    printf("nind:%f  S1=%f,  S2=%f  S3=%f  S4=%f  num2=%f  num=%f  asum=%f t=%f\n",Nind,S1,S2,S3,S4,num2,num,asum,t);
    } else {
      m = 0; t=c=*in;
      a = 0.0; b=0.0;
    }
    
    // linear regression error:
    ii=0.0; double e;
    while (in<iE) {
      e = (double)(*(in++)) - (m*ii + t);
      if (normInputs) e /= range;
      lea += fabs(e);
      ii += 1.0;
      leq += e*e;
    }
    in=i0;

    double rs=0.0, ls=0.0;
    double x0=0.0, y0=0.0;
    double yr=0.0;
    double yrnn=0.0, c3nn=0.0;
    double x0nn=0.0, y0nn=0.0;

    // quadratic regresssion error:
    if (enQreg) {
      ii=0.0; double e;
      while (in<iE) {
        e = (double)(*(in++)) - (a*ii*ii + b*ii + c);
        if (normInputs) e /= range;
        qea += fabs(e);
        ii += 1.0;
        qeq += e*e;
      }
      in=i0;

      // parabola vertex (x coordinate clipped to range -Nind + Nind!)  // TODO: why -Nind?
      x0 = b/(-2.0*a);
      if (x0 < -1.0*Nind) x0 = -Nind;
      if (x0 > Nind) x0 = Nind;
      if (!finite(x0)) x0 = Nind;
      x0nn = x0;
      y0 = c - b*b/(4.0*a);  // TODO: also limit to range 0..Nind
      if (!finite(y0)) y0 = 0.0;
      y0nn = y0;

      // parabola left / right points
      yrnn = yr = a * (Nind-1.0)*(Nind-1.0) + b*(Nind-1.0) + c;
      if (!finite(yr)) { yr = 0.0; yrnn = 0.0; }
      c3nn = c;
    }

    int n=0;
    


    if (normRegCoeff) {
      // TODO: below is segment normalisation... provide norm. to units of seconds!
      m *= Nind-1.0;
      a *= (Nind-1.0)*(Nind-1.0);
      b *= Nind-1.0;
      if (Nind != 1.0)
        x0 /= Nind-1.0;
    }
    if (normInputs) {
      m /= range;
      t = (t-min)/range;
      a /= range;
      b /= range;
      c = (c-min)/range;
      y0 = (y0 - min)/range;
      yr = (yr - min)/range;
    }

    if (enQreg) {
      // parabola partial slopes
      if (x0 > 0) ls = (y0-c)/x0;
      if (normRegCoeff) {
        if (x0 < 1.0) rs = (yr-y0)/(1.0-x0);
      } else {
        if (x0 < Nind-1.0) rs = (yr-y0)/(Nind-1.0-x0);
      }
    }

    // security checks:
    if (!finite(m)) m = 0.0;
    if (!finite(t)) t = 0.0;
    if (!finite(lea/Nind)) lea = 0.0;
    if (!finite(leq/Nind)) leq = 0.0;
    if (!finite(a)) a = 0.0;
    if (!finite(b)) b = 0.0;
    if (!finite(c)) { c = 0.0; c3nn = 0.0; }
    if (!finite(ls)) ls = 0.0;
    if (!finite(rs)) rs = 0.0;
    if (!finite(qea/Nind)) qea = 0.0;
    if (!finite(qeq/Nind)) qeq = 0.0;
    if (!finite(centroid)) centroid = 0.0;

    // save values:
    if (enab[FUNCT_LINREGC1]) out[n++]=(FLOAT_DMEM)m;
    if (enab[FUNCT_LINREGC2]) out[n++]=(FLOAT_DMEM)t;
    if (enab[FUNCT_LINREGERRA]) out[n++]=(FLOAT_DMEM)(lea/Nind);
    if (enab[FUNCT_LINREGERRQ]) out[n++]=(FLOAT_DMEM)(leq/Nind);

    if (enab[FUNCT_QREGC1]) out[n++]=(FLOAT_DMEM)a;
    if (enab[FUNCT_QREGC2]) out[n++]=(FLOAT_DMEM)b;
    if (enab[FUNCT_QREGC3]) out[n++]=(FLOAT_DMEM)c;
    if (!oldBuggyQerr) {
      if (enab[FUNCT_QREGERRA]) out[n++]=(FLOAT_DMEM)(qea/Nind);
      if (enab[FUNCT_QREGERRQ]) out[n++]=(FLOAT_DMEM)(qeq/Nind);
    } else {
      if (enab[FUNCT_QREGERRA]) out[n++]=(FLOAT_DMEM)(qea);
      if (enab[FUNCT_QREGERRQ]) out[n++]=(FLOAT_DMEM)(qeq);
    }

    if (enab[FUNCT_CENTROID]) out[n++]=(FLOAT_DMEM)centroid;

    if (enab[FUNCT_QREGLS]) out[n++]=(FLOAT_DMEM)ls;
    if (enab[FUNCT_QREGRS]) out[n++]=(FLOAT_DMEM)rs;
    if (enab[FUNCT_QREGX0]) out[n++]=(FLOAT_DMEM)x0;
    if (enab[FUNCT_QREGY0]) out[n++]=(FLOAT_DMEM)y0;
    if (enab[FUNCT_QREGYR]) out[n++]=(FLOAT_DMEM)yr;
    if (enab[FUNCT_QREGY0NN]) out[n++]=(FLOAT_DMEM)y0nn;
    if (enab[FUNCT_QREGC3NN]) out[n++]=(FLOAT_DMEM)c3nn;
    if (enab[FUNCT_QREGYRNN]) out[n++]=(FLOAT_DMEM)yrnn;

    return n;
  }
  return 0;
}

/*
long cFunctionalRegression::process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout)
{

  return 0;
}
*/

cFunctionalRegression::~cFunctionalRegression()
{
}

