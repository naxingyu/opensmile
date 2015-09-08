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


#ifndef __EXCEPTIONS_HPP
#define __EXCEPTIONS_HPP

#include <core/smileLogger.hpp>

#define EX_GENERIC    0   // fast , integer based, exception
#define EX_COMPONENT  1
#define EX_IO         2
#define EX_CONFIG     3
#define EX_MEMORY     4
#define EX_USER       9

#undef class
/* The base class for all exception classes in openSMILE */
class DLLEXPORT cSMILException {
  protected:
    int code;
    int type;

  public:
    cSMILException(int _type, int _code=0) { type=_type; code=_code; }
    virtual int getCode() { return code; }
    int getType() { return type; }

    // get Error message as human readable text
    virtual char * getText() {
        const char *etype;
        switch(type) {
          case EX_GENERIC: etype=""; break;
          case EX_MEMORY: etype="Memory"; break;
          default: etype="Unknown";
        }
        return myvprint("%s ERROR : code = %i",etype,code);
    }

    virtual ~cSMILException() {}
};

/* generic exception, only contains an error code */
class DLLEXPORT cGenericException : public cSMILException {
  public:
    cGenericException(int _code) : cSMILException(EX_GENERIC, _code) { }
    virtual ~cGenericException() {}
};

/* memory exception, only contains an error code */
class DLLEXPORT cMemoryException : public cSMILException {
  public:
    cMemoryException(int _code) : cSMILException(EX_MEMORY, _code) { }
    virtual ~cMemoryException() {}
};

#define OUT_OF_MEMORY throw(cMemoryException(0))


/* Component Exception, base class for all exceptions with a text message and module name */
class DLLEXPORT cComponentException : public cSMILException {
  private:
    char *tmp;

  protected:
    char *text;
    const char *module;

  public:
    cComponentException(char *t, const char *m=NULL):
      cSMILException(EX_COMPONENT),
      tmp(NULL),
      text(NULL)
      { module=m; text=t; logException(); }
    cComponentException(int _type, char *t, const char *m=NULL):
      cSMILException(_type),
      tmp(NULL),
      text(NULL)
      { module=m; text=t; logException(); }

    virtual char * getText() {
      if (text!=NULL) {
        const char *etype;
        switch(type) {
          case EX_COMPONENT: etype="Component"; break;
          case EX_IO: etype="I/O"; break;
          case EX_CONFIG: etype="Config"; break;
          case EX_USER: etype=""; break;
          default: etype="Unknown"; break;
        }
        if (module != NULL) {
          tmp = myvprint("%s Exception in %s : %s [code = %i]",etype,module,text,code);
        } else {
          tmp = myvprint("%s Exception : %s [code = %i]",etype,text,code);
        }
      }
      return tmp;
    }

    // automatically log the exception to smileLog component
    virtual void logException() {
      #ifdef AUTO_EXCEPTION_LOGGING
      #ifdef SMILE_LOG_GLOBAL
      SMILE_LOG_GLOBAL.error(text,1,module);
      text = NULL;
      #endif
      #endif
    }

    virtual ~cComponentException() {
      if (tmp!=NULL) free(tmp);
      if (text != NULL) free(text);
    }
};

// throw a component exception
#define COMP_ERR(...)  throw(cComponentException(myvprint(__VA_ARGS__), MODULE))

#define CE_PARSER  1
#define CE_INVALID 2
#define CE_MANAGER 3

/*
 ConfigException:
   raised when:
          a) errors occur during config file parsing (origin = CE_PARSER)
          b) a module detects a bogus configuration, invalid value, etc.  (origin = CE_INVALID)
          c) mandatory parameters are not specified, etc. (origin = CE_MANAGER)
*/
class DLLEXPORT cConfigException : public cComponentException {
  private:
    int origin;

  public:
    cConfigException(int _origin, char *t, const char *m=NULL) :
      cComponentException(EX_CONFIG,t,m),
      origin(_origin)
      { }
    virtual int getOrigin() { return origin; }
    virtual ~cConfigException() {}
};

// throw config exceptions:
#ifdef _MSC_VER
#define CONF_INVALID_ERR(...) { SMILE_ERR(1, myvprint(__VA_ARGS__)); throw(cConfigException(CE_INVALID, myvprint(__VA_ARGS__), MODULE)); }
#define CONF_PARSER_ERR(...) { SMILE_ERR(1, myvprint(__VA_ARGS__));  throw(cConfigException(CE_PARSER, myvprint(__VA_ARGS__), MODULE)); }
#define CONF_MANAGER_ERR(...) { SMILE_ERR(1, myvprint(__VA_ARGS__));  throw(cConfigException(CE_MANAGER, myvprint(__VA_ARGS__), MODULE)); }
#else
#define CONF_INVALID_ERR(...)  throw(cConfigException(CE_INVALID, myvprint(__VA_ARGS__), MODULE))
#define CONF_PARSER_ERR(...)  throw(cConfigException(CE_PARSER, myvprint(__VA_ARGS__), MODULE))
#define CONF_MANAGER_ERR(...)  throw(cConfigException(CE_MANAGER, myvprint(__VA_ARGS__), MODULE))
#endif

class DLLEXPORT cIOException : public cComponentException {
  public:
    cIOException(char *t, int _code=0, const char *m=NULL) :
      cComponentException(EX_IO,t,m)
      { code = _code; }
    virtual ~cIOException() {}
};

#define IO_ERR(...)  throw(cIOException(FMT(__VA_ARGS__),0,MODULE));

class DLLEXPORT cUserException : public cComponentException {
  public:
    cUserException(char *t, int _code, const char *m=NULL) :
      cComponentException(EX_USER,t,m)
     { code = _code; }
    virtual ~cUserException() {}
};

#define USER_ERR(CODE, ...)  throw(cIOException(FMT(__VA_ARGS__),CODE,MODULE));


/**** typedefs *****************************/

typedef cGenericException GenericException;
typedef cMemoryException MemoryException;

typedef cComponentException ComponentException;
typedef cConfigException ConfigException;
typedef cIOException IOException;
typedef cUserException UserException;


//#define MIN_LOG_STRLEN   255


#endif  // __EXCEPTIONS_HPP

