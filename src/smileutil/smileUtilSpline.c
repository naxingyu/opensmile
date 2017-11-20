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


/*  SmileUtilSpline
    =========

contains modular DSP functions for splines

*/

#include <smileutil/smileUtilSpline.h>

int smileMath_spline(const double *xval, const double *yval,
  long N, double y1p, double ynp, double *y2, 
  double **workarea)
{
  long i = 0;
  long j = 0;
  double p = 0.0;
  double qn = 0.0;
  double sigma = 0.0;
  double un = 0.0;
  double *u = NULL;

  if (workarea != NULL) {
    u = *workarea;
  }
  if (u == NULL) {
    u = (double*)calloc(1, sizeof(double) * (N - 1));
  }
  if (y1p > 0.99e30) {
    u[0] = 0.0;
    y2[0] = 0.0;
  } else {
    u[0] = (3.0 / (xval[1] - xval[0])) *
        ((yval[1] - yval[0]) / (xval[1] - xval[0]) - y1p);
    y2[0] = -0.5;
  }

  for (i = 1; i < N - 1; i++) {
    sigma = (xval[i] - xval[i - 1]) / (xval[i + 1] - xval[i - 1]);
    p = sigma * y2[i - 1] + 2.0;
    y2[i] = (sigma - 1.0) / p;
    u[i] = (yval[i + 1] - yval[i]) / (xval[i + 1] - xval[i])
        - (yval[i] - yval[i - 1]) / (xval[i] - xval[i - 1]);
    u[i] = 1.0 / p * (6.0 * u[i] / (xval[i + 1] - xval[i - 1])
        - sigma * u[i - 1]);
  }

  if (ynp > 0.99e30) {
    qn = 0.0; 
    un = 0.0;
  } else {
    un = (3.0 / (xval[N - 1] - xval[N - 2])) * (ynp - (yval[N - 1] - yval[N - 2]) 
           / (xval[N - 1] - xval[N - 2]));
    qn = 0.5;
  }

  y2[N - 1] = (un - qn * u[N - 2]) 
               / (qn * y2[N - 2] + 1.0);
  for (j = N - 2; j >= 0; j--) {
    y2[j] = y2[j] * y2[j + 1] + u[j];
  }

  if (workarea == NULL) {
    free(u);
  } else {
    *workarea = u;
  }
  return 1;
}

int smileMath_spline_FLOAT_DMEM(const FLOAT_DMEM *xval, const FLOAT_DMEM *yval,
  long N, FLOAT_DMEM y1p, FLOAT_DMEM ynp, FLOAT_DMEM *y2,
  FLOAT_DMEM **workarea)
{
  long i = 0;
  long j = 0;
  FLOAT_DMEM p = 0.0;
  FLOAT_DMEM qn = 0.0;
  FLOAT_DMEM sigma = 0.0;
  FLOAT_DMEM un = 0.0;
  FLOAT_DMEM *u = NULL;

  if (workarea != NULL) {
    u = *workarea;
  }
  if (u == NULL) {
    u = (FLOAT_DMEM*)calloc(1, sizeof(FLOAT_DMEM) * (N - 1));
  }
  if (y1p > (FLOAT_DMEM)0.99e30) {
    u[0] = (FLOAT_DMEM)0.0;
    y2[0] = (FLOAT_DMEM)0.0;
  } else {
    u[0] = ((FLOAT_DMEM)3.0 / (xval[1] - xval[0]))
        * ((yval[1] - yval[0]) / (xval[1] - xval[0]) - y1p);
    y2[0] = (FLOAT_DMEM)-0.5;
  }

  for (i = 1; i < N - 1; i++) {
    sigma = (xval[i] - xval[i - 1]) / (xval[i + 1] - xval[i - 1]);
    p = sigma * y2[i - 1] + (FLOAT_DMEM)2.0;
    y2[i] = (sigma - (FLOAT_DMEM)1.0) / p;
    u[i] = (yval[i + 1] - yval[i]) / (xval[i + 1] - xval[i])
        - (yval[i] - yval[i - 1]) / (xval[i] - xval[i - 1]);
    u[i] = (FLOAT_DMEM)1.0 / p * ((FLOAT_DMEM)6.0 * u[i] / (xval[i + 1] - xval[i - 1])
        - sigma * u[i - 1]);
  }

  if (ynp > (FLOAT_DMEM)0.99e30) {
    qn = (FLOAT_DMEM)0.0;
    un = (FLOAT_DMEM)0.0;
  } else {
    un = ((FLOAT_DMEM)3.0 / (xval[N - 1] - xval[N - 2])) * (ynp - (yval[N - 1] - yval[N - 2])
           / (xval[N - 1] - xval[N - 2]));
    qn = (FLOAT_DMEM)0.5;
  }

  y2[N - 1] = (un - qn * u[N - 2])
               / (qn * y2[N - 2] + (FLOAT_DMEM)1.0);
  for (j = N - 2; j >= 0; j--) {
    y2[j] = y2[j] * y2[j + 1] + u[j];
  }

  if (workarea == NULL) {
    free(u);
  } else {
    *workarea = u;
  }
  return 1;
}

/* smileMath_splint: 
    Does spline interpolation of y value for a given x value of function ya=f(xa).

    xorig[1..n] and yorig[1..n] contain the function f to be interpolated,
     --> yorig[i] = f(xorig[i]), where xorig[1] < xorig[2] < ... < xorig[n]
    y2[1..n] is the output of smileMath_spline(...) above, which contains the second order derivatives.
    N contains the length of both xorig and yorig arrays.
    'x' holds the x position at which to interpolate *y = f(x)
*/
int smileMath_splint(const double *xorig, const double *yorig, const double *y2, long N,
    double x, double *y)
{
  long klower = 1;
  long kupper = N;
  long kcurrent = 0;
  double range = 0.0;
  double b = 0.0;
  double a = 0.0;

  while (kupper - klower > 1) {
    kcurrent = (kupper + klower) >> 1;
    if (xorig[kcurrent - 1] > x) {
      kupper = kcurrent;
    } else {
      klower = kcurrent;
    }
  }
  kupper--; 
  klower--;
  range = xorig[kupper] - xorig[klower];
  if (range == 0.0) {
    printf("smileMath_splint(): bad input (range == 0)!\n");
    return 0;
  }
  a = (xorig[kupper] - x) / range;
  b = (x - xorig[klower]) / range;
  *y = a * yorig[klower] + b * yorig[kupper] + ((a * a * a - a) * y2[klower] +
    (b * b * b - b) * y2[kupper]) * (range * range) / 6.0;
  return 1;
}

int smileMath_splint_FLOAT_DMEM(const FLOAT_DMEM *xorig, const FLOAT_DMEM *yorig,
    const FLOAT_DMEM *y2, long N, FLOAT_DMEM x, FLOAT_DMEM *y)
{
  long klower = 1;
  long kupper = N;
  long kcurrent = 0;
  FLOAT_DMEM range = (FLOAT_DMEM)0.0;
  FLOAT_DMEM b = (FLOAT_DMEM)0.0;
  FLOAT_DMEM a = (FLOAT_DMEM)0.0;

  while (kupper - klower > 1) {
    kcurrent = (kupper + klower) >> 1;
    if (xorig[kcurrent - 1] > x) {
      kupper = kcurrent;
    } else {
      klower = kcurrent;
    }
  }
  kupper--;
  klower--;
  range = xorig[kupper] - xorig[klower];
  if (range == 0.0) {
    printf("smileMath_splint(): bad input (range == 0)!\n");
    return 0;
  }
  a = (xorig[kupper] - x) / range;
  b = (x - xorig[klower]) / range;
  *y = a * yorig[klower] + b * yorig[kupper] + ((a * a * a - a) * y2[klower] +
    (b * b * b - b) * y2[kupper]) * (range * range) / (FLOAT_DMEM)6.0;
  return 1;
}




