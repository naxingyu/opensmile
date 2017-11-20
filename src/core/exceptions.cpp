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


/*

  SMILE exception classes
  ===========================
  
  Exception base class:
    cSMILException:
      + int getCode()
      + int getType()
        valid type constants:
          EX_GENERIC, EX_MEMORY, EX_COMPONENT, EX_IO, EX_CONFIG, EX_USER
      + getText()  : get human readable error message

  GenericException: (cSMILException) [Internal errors, usually not displayed or loggged, only informative purpose]
    > GenericException(int code)
    - int getCode()
    
  MemoryException: (cSMILException) [Memory allocation errors, etc.]
    > MemoryException(int code)
    - int getCode()

  ----------------------------------------------------------------------------
  ComponentException: (cSMILException) [Any error in a smile component]
    > ComponentException([const char* module], const char* text)
    > ComponentException([const char* module], FMT(const char* fmt, ...))
    + getText()  : get full verbose text of error message
    - int getCode()


  ConfigException: (ComponentException) [Any configuration error]
    > ConfigException(int origin, [const char *module], const char* text)
    > ConfigException(int origin, [const char *module], FMT(const char* fmt, ...))
    + int getOrigin()
   origin: (exception raised when...)
          a) errors occur during config file parsing (origin = CE_PARSER)
          b) a module detects a bogus configuration, invalid value, etc.  (origin = CE_INVALID)
          c) mandatory parameters are not specified, etc. (origin = CE_MANAGER)

  IOException: (ComponentException) [Any I/O error, drive, network, sound, ...]
    > IOException([const char *module], [int code], const char* text, )
    > IOException([const char *module], [int code], FMT(const char* fmt, ...))

  UserException: (ComponentException) [Any other error, user defined...]
    > UserException([const char *module], int code, const char* text, )
    > UserException([const char *module], int code, FMT(const char* fmt, ...))
  
 */
/*
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#define MODULE "exceptionHandler"
*/

