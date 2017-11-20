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


#ifndef __SMILE_UTIL_SPLINE_H
#define __SMILE_UTIL_SPLINE_H

/* you may remove this include if you are using smileUtil outside of openSMILE */
//#include <smileCommon.hpp>
/* --------------------------------------------------------------------------- */
#ifndef __SMILE_COMMON_H
#define __SMILE_COMMON_H

// this is a minimal set of defines if we are using smileUtil outside of openSMILE
// on linux you should consider compiling with -DHAVE_INTTYPES_H option (see smileTypes.h)

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef _MSC_VER // Visual Studio specific macro
  #ifdef BUILDING_DLL
    #define DLLEXPORT __declspec(dllexport)
//    #define class class __declspec(dllexport)
  #else
    #define DLLEXPORT __declspec(dllimport)
//    #define class class __declspec(dllimport)
  #endif
  #define DLLLOCAL
#else
    #define DLLEXPORT
    #define DLLLOCAL
#endif

#include <stdlib.h>
#include <stdio.h>

#endif  // __SMILE_COMMON_H


#ifndef __SMILE_LOGGER_HPP
#define __SMILE_LOGGER_HPP
#define LOGLEVEL 2
#define SMILE_PRINT(...) fprintf(stderr,__VA_ARGS__)
#define SMILE_PRINTL(level, ...) { if (level <= LOGLEVEL) fprintf(stderr,__VA_ARGS__); }

#define SMILE_MSG(level, ...) { if (level <= LOGLEVEL) { fprintf(stderr,"MSG(%i): ",level) ; fprintf(stderr,__VA_ARGS__); } }
#define SMILE_IMSG(level, ...) { if (level <= LOGLEVEL) { fprintf(stderr,"MSG(%i): ",level) ; fprintf(stderr,__VA_ARGS__); } }

#define SMILE_ERR(level, ...) { if (level <= LOGLEVEL) { fprintf(stderr,"ERR(%i): ",level) ; fprintf(stderr,__VA_ARGS__); } }
#define SMILE_IERR(level, ...) { if (level <= LOGLEVEL) { fprintf(stderr,"ERR(%i): ",level) ; fprintf(stderr,__VA_ARGS__); } }

#define SMILE_WRN(level, ...) { if (level <= LOGLEVEL) { fprintf(stderr,"WRN(%i): ",level) ; fprintf(stderr,__VA_ARGS__); } }
#define SMILE_IWRN(level, ...) { if (level <= LOGLEVEL) { fprintf(stderr,"WRN(%i): ",level) ; fprintf(stderr,__VA_ARGS__); } }


#ifdef DEBUG
#define SMILE_DBG(level, ...) { if (level <= LOGLEVEL) { fprintf(stderr,"DBG(%i): ",level) ; fprintf(stderr,__VA_ARGS__); } }
#define SMILE_IDBG(level, ...) { if (level <= LOGLEVEL) { fprintf(stderr,"DBG(%i): ",level) ; fprintf(stderr,__VA_ARGS__); } }
#endif
#endif  // __SMILE_LOGGER_HPP

#include <core/smileTypes.h>


/****** spline functions ******/

/*
        smileMath_spline: evalutes a spline function:

        Given two arrays x[1..n] and y[1..n] containing a tabulated function, that is
        y[i] = f(x[i]), with x[1] < x[2] < ... < x[n], and the values yp1 and
        ypn for the first derivative of the interpolating function at points
        1 and n, respectively, this function returns an array y2[1..n] which
        contains the second derivative of the interpolating function at the
        point x.
        If yp1 and / or ypn are greater or equal than 10^30,
        the corresponding boundary condition for a natuarl spline is set, with
        a zero second derivative on that boundary.
        u is an optional pointer to a workspace area (smileMath_spline will
        allocate a vector there if the pointer pointed to is NULL).
        The calling code is responsible of freeing this memory with free() at any
        later time which seems convenient (i.e. at the end of all calculations).
*/
DLLEXPORT int smileMath_spline(const double *x, const double *y,
    long n, double yp1, double ypn, double *y2, double **workspace);

DLLEXPORT int smileMath_spline_FLOAT_DMEM(const FLOAT_DMEM *x, const FLOAT_DMEM *y,
    long n, FLOAT_DMEM yp1, FLOAT_DMEM ypn, FLOAT_DMEM *y2, FLOAT_DMEM **workspace);

/* smileMath_splint:
    Does spline interpolation of y for a given x value of function ya=f(xa).

    Xa[1..n] and ya[1..n] contain the function to be interpolated,
    i.e., ya[i] = f(xa[i]), with xa[1] < xa[2] < ... < xa[n].
    y2a[1..n] is the output of smileMath_spline(...) above,
    containing the second derivatives.
    n contains the length of xa and ya
    x holds the position at which to interpolate *y = f(x).
*/
DLLEXPORT int smileMath_splint(const double *xa, const double *ya,
    const double *y2a, long n, double x, double *y);

DLLEXPORT int smileMath_splint_FLOAT_DMEM(const FLOAT_DMEM *xa, const FLOAT_DMEM *ya,
    const FLOAT_DMEM *y2a, long n, FLOAT_DMEM x, FLOAT_DMEM *y);

#endif // __SMILE_UTIL_SPLINE_H

