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


/*  openSMILE type definitions */


#ifndef __SMILE_TYPES_H
#define __SMILE_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* int types, platform independent (hopefully...) */
#ifdef HAVE_INTTYPES_H  // prefer inttypes.h if available
#include <inttypes.h>
#else
#ifdef _MSC_VER // Visual Studio specific macro, use MSVC built-in int types
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
typedef signed __int8 int8_t;
typedef signed __int16 int16_t;
typedef signed __int32 int32_t;
typedef signed __int64 int64_t;
#else
#ifdef __WINDOWS  // UINT8, etc., constants should be defined in Windows Platform SDK 2003
typedef UINT8 uint8_t ;
typedef UINT16 uint16_t ;
typedef UINT32 uint32_t ;
typedef UINT64 uint64_t ;
typedef INT8 int8_t ;
typedef INT16 int16_t ;
typedef INT32 int32_t ;
typedef INT64 int64_t ;
#else
#ifdef __ANDROID__
typedef unsigned char uint8_t ;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
#else  // last resort, use C standards.. warning, int is not very portable, also long long, etc...
typedef unsigned char uint8_t ;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long int uint64_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long int int64_t;
#endif
#endif
#endif

#endif

#include <float.h>
/* opensmile internal types */
#define FLOAT_DMEM_FLOAT  0
#define FLOAT_DMEM_DOUBLE  1


// this defines the float type used throughout the data memory, either 'float' or 'double'
typedef float FLOAT_DMEM;
#define FLOAT_DMEM_NUM  FLOAT_DMEM_FLOAT // this numeric constant MUST equal the float type set above ...
                                           // 0 = float, 1 = double:
#define FLOAT_DMEM_MAX FLT_MAX
#define FLOAT_DMEM_MIN FLT_MIN


// this defines the int type used throughout the data memory, either 'short', 'int' or 'long'
typedef int INT_DMEM;

#ifdef __cplusplus
}
#endif

#endif // __SMILE_TYPES_H

