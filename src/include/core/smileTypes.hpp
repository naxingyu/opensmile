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


/*  openSMILE type definitions */


#ifndef __SMILE_TYPES_HPP
#define __SMILE_TYPES_HPP

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

/* opensmile internal types */
#define FLOAT_DMEM_FLOAT  0
#define FLOAT_DMEM_DOUBLE  1

// this defines the float type used throughout the data memory, either 'float' or 'double'
typedef float FLOAT_DMEM;
#define FLOAT_DMEM_NUM  FLOAT_DMEM_FLOAT // this numeric constant MUST equal the float type set above ...
                                           // 0 = float, 1 = double:
// this defines the int type used throughout the data memory, either 'short', 'int' or 'long'
typedef int INT_DMEM;








#endif // __SMILE_TYPES_HPP

