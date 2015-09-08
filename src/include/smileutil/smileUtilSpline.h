/*F******************************************************************************
 *
 * openSMILE - open Speech and Music Interpretation by Large-space Extraction
 *       the open-source Munich Audio Feature Extraction Toolkit
 * Copyright (C) 2008-2009  Florian Eyben, Martin Woellmer, Bjoern Schuller
 *
 *
 * Institute for Human-Machine Communication
 * Technische Universitaet Muenchen (TUM)
 * D-80333 Munich, Germany
 *
 *
 * If you use openSMILE or any code from openSMILE in your research work,
 * you are kindly asked to acknowledge the use of openSMILE in your publications.
 * See the file CITING.txt for details.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ******************************************************************************E*/


#ifndef __SMILE_UTIL_SPLINE_H
#define __SMILE_UTIL_SPLINE_H

/* you may remove this include if you are using smileUtil outside of openSMILE */
//#include <smileCommon.hpp>
/* --------------------------------------------------------------------------- */
#ifndef __SMILE_COMMON_H
#define __SMILE_COMMON_H

// this is a minimal set of defines if we are using smileUtil outside of openSMILE
// on linux you should consider compiling with -DHAVE_INTTYPES_H option (see smileTypes.hpp)

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

#include <core/smileTypes.hpp>


//#include <smileutil/smileUtil.h>

////#ifdef __cplusplus
//extern "C" {
//#endif

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
DLLEXPORT int smileMath_splint(const double xa[], const double ya[],
    const double y2a[], long n, double x, double *y);

DLLEXPORT int smileMath_splint_FLOAT_DMEM(const FLOAT_DMEM xa[], const FLOAT_DMEM ya[],
    const FLOAT_DMEM y2a[], long n, FLOAT_DMEM x, FLOAT_DMEM *y);

//#ifdef __cplusplus
//}
//#endif

#endif // __SMILE_UTIL_SPLINE_H

