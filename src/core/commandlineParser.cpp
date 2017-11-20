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
 openSMILE commandline parser


*/


#include <core/commandlineParser.hpp>

#define MODULE "commandlineParser"

cCommandlineParser::cCommandlineParser(int _argc, const char ** _argv) :
  active(0),
  N(0),
  opt(NULL),
  argc(0),
  argv(NULL),
  Nalloc(0)
{
  if (_argc > 0) {
    argc = _argc;
    argv = _argv;
    active = 1;
    Nalloc = N_ALLOC_BLOCK;
    opt = (struct sCmdlineOpt *)calloc(1,sizeof(struct sCmdlineOpt)*Nalloc);
  }
}

cCommandlineParser::~cCommandlineParser()
{
  int i;
  if (opt != NULL) {
    for (i=0; i<N; i++) {
      if (opt[i].description != NULL) free(opt[i].description);
      if (opt[i].dfltStr != NULL) free(opt[i].dfltStr);
    }
    free(opt);
  }
}

int cCommandlineParser::getWrIdx()
{
  if (N>=Nalloc) {
    struct sCmdlineOpt *tmp = (struct sCmdlineOpt *)calloc(1,sizeof(struct sCmdlineOpt)*(N+N_ALLOC_BLOCK));
    if (tmp==NULL) OUT_OF_MEMORY;
    memcpy( tmp, opt, sizeof(struct sCmdlineOpt)*Nalloc );
    free(opt); opt = tmp;
    Nalloc = N+N_ALLOC_BLOCK;
  }
  return N++;
}

int cCommandlineParser::addOpt( const char *name, char abbr, const char *description, int argMandatory, int isMandatory)
{
  if ((abbr == 'h')||((name != NULL)&&(name[0]=='h')&&(name[1]==0))) {
    COMP_ERR("option -h is reserved for show usage internally! please chose another name in your code! sorry..");
  }
  int n = getWrIdx();
  if (name == NULL) COMP_ERR("addOpt: cannot add commandlineParser option with name==NULL!");
  size_t l = strlen(name)+1;
  if (l > STRLEN_NAME) COMP_ERR("addOpt: name is longer then %i characters (legnth(name) = %i charachters)! increase STRLEN_NAME in commandlineParser.cpp!",STRLEN_NAME,l);
  memcpy(opt[n].name,name,l);
  opt[n].abbr = abbr;
  opt[n].description = strdup(description);
  opt[n].argMandatory = argMandatory;
  opt[n].isMandatory = isMandatory;
  opt[n].isSet = 0;
  return n;
}

int cCommandlineParser::addBoolean( const char *name, char abbr, const char *description , int dflt, int argMandatory, int isMandatory )
{
  int n = addOpt(name,abbr,description, argMandatory, isMandatory);
  opt[n].dfltInt = dflt;
  opt[n].type = CMDOPT_BOOLEAN;
  return n;
}

int cCommandlineParser::addInt( const char *name, char abbr, const char *description , int dflt, int argMandatory, int isMandatory )
{
  int n = addOpt(name,abbr,description, argMandatory, isMandatory);
  opt[n].dfltInt = dflt;
  opt[n].dfltDouble = (double)dflt;
  opt[n].type = CMDOPT_INT;
  return n;
}

int cCommandlineParser::addDouble( const char *name, char abbr, const char *description, double dflt, int argMandatory, int isMandatory )
{
  int n = addOpt(name,abbr,description, argMandatory, isMandatory);
  opt[n].dfltDouble = dflt;
  opt[n].dfltInt = (int)dflt;
  opt[n].type = CMDOPT_DOUBLE;
  return n;
}

int cCommandlineParser::addStr( const char *name, char abbr, const char *description, const char *dflt, int argMandatory, int isMandatory )
{
  int n = addOpt(name,abbr,description, argMandatory, isMandatory);
  if (dflt != NULL)
    opt[n].dfltStr = strdup(dflt);
  else
    opt[n].dfltStr = NULL;
  opt[n].type = CMDOPT_STR;
  return n;
}


/* find index of option "name", return index upon success, else return -1 */
int cCommandlineParser::findOpt( const char *name ) const
{
  int i;
  if (name == NULL) return -1;
  for (i=0; i<N; i++) {
    if (!strcmp(opt[i].name, name)) return i;
  }
  // experimental: support for abbr...
  if (strlen(name) == 1) {
    char c = name[0];
    for (i=0; i<N; i++) {
      if (opt[i].abbr == c) return i;
    }
  }
  return -1;
}

int cCommandlineParser::getBoolean( const char *name ) const
{
  int n = findOpt( name );
  if (n >= 0) {
    if ((opt[n].type != CMDOPT_BOOLEAN)&&(opt[n].type != CMDOPT_INT))
      { COMP_ERR("requested commandline argument '%s' is not of type boolean or int!"); }
    return opt[n].dfltInt;
  } else {
    COMP_ERR("boolean commandline argument '%s' not found in commandline parser object!",name);
  }
  return 0;
}

int cCommandlineParser::getInt( const char *name ) const
{
  int n = findOpt( name );
  if (n >= 0) {
    if (opt[n].type != CMDOPT_INT)
      { COMP_ERR("requested commandline argument '%s' is not of type int!"); }
    return opt[n].dfltInt;
  } else {
    COMP_ERR("int commandline argument '%s' not found in commandline parser object!",name);
  }
  return 0;
}

double cCommandlineParser::getDouble( const char *name ) const
{
  int n = findOpt( name );
  if (n >= 0) {
    if (opt[n].type != CMDOPT_DOUBLE)
      { COMP_ERR("requested commandline argument '%s' is not of type double!"); }
    return opt[n].dfltDouble;
  } else {
    COMP_ERR("floating point commandline argument '%s' not found in commandline parser object!",name);
  }
  return 0;
}

const char * cCommandlineParser::getStr( const char *name ) const
{
  int n = findOpt( name );
  if (n >= 0) {
    if (opt[n].type != CMDOPT_STR)
      { COMP_ERR("requested commandline argument '%s' is not of type string!"); }
    return opt[n].dfltStr;
  } else {
    COMP_ERR("string commandline argument '%s' not found in commandline parser object!",name);
  }
  return 0;
}

int cCommandlineParser::isSet( const char *name ) const
{
  int n = findOpt( name );
  if (n >= 0) {
    return opt[n].isSet;
  }
  return 0;
}

int cCommandlineParser::doParse(int ignDupl, int ignUnknown)
{
  int expectOpt = 1;
  int showusage = 0;

  // foreach argv element with(--) or (-), find the thing in opt
  int i,n;
  SMILE_DBG(3,"about to parse %i commandline options ",argc-1);
  for (i=1; i<argc; i++) {
    SMILE_DBG(5,"parsing option %i : '%s'",i,argv[i]);
    if ((unsigned char)argv[i][0] > 127) {
      SMILE_ERR(0, "Please don't be lazy and copy & paste the commandlines from the SMILE book PDF. Your commandline contains invalid ASCII characters (probably incorrect '-'es) as a result of this and might not be parsed correctly! Type the commandline from scratch to avoid errors.");
      SMILE_ERR(0, "The offending argument is: '%s'", argv[i]);
      COMP_ERR("Parse error.");
    }
    if (argv[i][0] == '-') { // option...?
      if ((argv[i][1] == 'h')&&(argv[i][2] == 0)) { // usage (-h)
        showusage = 1;
      } else {
        const char *o = argv[i]+1;
        if (o[0] == '-') o++;
        n = findOpt(o);
        if (n >= 0) { // found!
          if (opt[n].isSet == 1) { // option specified twice...!
            if (!ignDupl) {  // if ignDupl == 1, we supress this error message
              SMILE_ERR(0,"duplicate option '%s' on commandline (ignoring duplicate!) ",o);
            }
            if (opt[n].nArgSet > 0) {
              i += opt[n].nArgSet;
            }
          } else {
          // now look for value....
          if (i+1 < argc) {
            int optCand = 0;
            if (argv[i+1][0] == '-') optCand = 1;  // argument might be an option... or a negative numeric
            int v;
            double vd;
            const char *o2;
            char *ep = NULL;
            
            // parse value according to type...
            switch (opt[n].type) {
              case CMDOPT_BOOLEAN:
                if (!optCand) {  // explicit value given
                  i++; // next parameter of commandline
                  if ((argv[i][0] == '0')||(argv[i][0] == 'N')||(argv[i][0] == 'n')||(argv[i][0] == 'F')||(argv[i][0] == 'f')) opt[n].dfltInt = 0;
                  else if ((argv[i][0] == '1')||(argv[i][0] == 'Y')||(argv[i][0] == 'y')||(argv[i][0] == 'T')||(argv[i][0] == 't')) opt[n].dfltInt = 1;
                } else {
                  if (opt[n].dfltInt == 0) opt[n].dfltInt = 1;  // invert boolean default, if option is given
                  else {
                    SMILE_WRN(4, "setting boolean option '%s' inverts default (true) to false.", argv[i]);
                    opt[n].dfltInt = 0;
                  }
                }
                opt[n].isSet = 1;
                opt[n].nArgSet = 0;
                break;
              case CMDOPT_INT:
                //  parse value:
                errno=0;
                v = strtol(argv[i+1],NULL,10); // values base 10... TODO: support hex
                if ( (optCand) && ((v==0)&&(errno==EINVAL)) ) { // option... no value given
                    if (opt[n].argMandatory) COMP_ERR("option '%s' requires an argument!",o);
                    else {
                      opt[n].isSet = 1;
                      opt[n].nArgSet = 0;
                    }
                } else {
                  if ((v==0)&&(errno==EINVAL)) COMP_ERR("invalid value specified for commandline option '%s'",o);
                  if ((v==0)&&(errno==ERANGE)) COMP_ERR("value specified for commandline option '%s' is out of range",o);
                  opt[n].dfltInt = v;
                  opt[n].dfltDouble = (double)v;
                  opt[n].isSet = 1;
                  opt[n].nArgSet = 1;
                  i++; // next parameter of commandline
                }
                break;
              case CMDOPT_DOUBLE:
                //  parse value:
                vd = strtod(argv[i+1],&ep);
                if ( (optCand) && ((vd==0.0)&&(ep==argv[i+1])) ) { // option... no value given
                    if (opt[n].argMandatory) COMP_ERR("option '%s' requires an argument!",o);
                    else {
                      opt[n].isSet = 1;
                      opt[n].nArgSet = 0;
                    }
                } else {
                  if ((vd==0)&&(ep==argv[i+1])) COMP_ERR("invalid value specified for commandline option '%s'",o);
                  //if ((v==0)&&(errno==ERANGE)) COMP_ERR("value specified for commandline option '%s' is out of range",o);
                  opt[n].dfltDouble = vd;
                  opt[n].dfltInt = (int)vd;
                  opt[n].isSet = 1;
                  opt[n].nArgSet = 1;
                  i++; // next parameter of commandline
                }
                break;
              case CMDOPT_STR:
                if (optCand) {  // bad situation... look for option
                  o2 = argv[i+1]+1;
                  if (o2[0] == '-') o2++;
                  int tmp = findOpt(o2);
                  if (tmp >= 0) { // next option
                    if (opt[n].argMandatory) COMP_ERR("option '%s' requires an argument!",o);
                    else {
                      opt[n].isSet = 1;
                      opt[n].nArgSet = 0;
                    }
                  } else { // string value
                    i++; // next parameter of commandline
                    if (opt[n].dfltStr != NULL) free(opt[n].dfltStr);
                    opt[n].dfltStr = strdup(argv[i]);
                    opt[n].isSet = 1;
                    opt[n].nArgSet = 1;
                  }
                } else { // string value
                  i++; // next parameter of commandline
                  if (opt[n].dfltStr != NULL) free(opt[n].dfltStr);
                  opt[n].dfltStr = strdup(argv[i]);
                  opt[n].isSet = 1;
                  opt[n].nArgSet = 1;
                }
                break;
              default:
                COMP_ERR("unknown option type (%i) encountered... this actually cannot be!",opt[n].type);
            }
          } else {  // no value... check if one is required... 
            if (opt[n].argMandatory) COMP_ERR("option '%s' requires an argument!",o);
            else {
              // process boolean option, if boolean, else ignore...
              if (opt[n].type == CMDOPT_BOOLEAN) {
                if (opt[n].dfltInt == 0) opt[n].dfltInt = 1;  // invert boolean default, if option is given
                else {
                  SMILE_WRN(4, "setting boolean option '%s' inverts default (true) to false.", argv[i]);
                  opt[n].dfltInt = 0;
                }
              }
              opt[n].isSet = 1;
              opt[n].nArgSet = 0;
            }
          }
          }
        } else {
          if (!ignUnknown) {
            SMILE_ERR(0,"doParse: unknown option '%s' on commandline!",argv[i]);
          }
        }
      } // end: if option == -h
    }
  }

  // TODO: from argv[0] get correct 'binname' to pass to showUsage
  if (showusage) {
    showUsage();
    return -1;
  }
  
  // now check if all mandatory parameters have been specified...
  int err = 0;
  for (i=0; i<N; i++) {
    if ( (opt[i].isSet == 0)&&(opt[i].isMandatory == 1) ) { SMILE_ERR(0,"mandatory commandline option '%s' was not specified!",opt[i].name); err = 1; }
  }
  if (err) {
    showUsage();
    SMILE_ERR(0,"missing options on the commandline!");
    return -1;
  }
  
  return 0;
}

void cCommandlineParser::showUsage(const char *binname)
{
  smilePrintHeader();
  if (binname == NULL) {
    SMILE_PRINT("Usage: SMILExtract [-option (value)] ...");
  } else {
    SMILE_PRINT("Usage: %s [-option (value)] ...",binname);
  }
  SMILE_PRINT(" ");
  SMILE_PRINT(" -h    Show this usage information");
  SMILE_PRINT(" ");
  int i;
  for (i=0; i<N; i++) {
    const char *ty="";
    switch (opt[i].type) {
      case CMDOPT_BOOLEAN:
        if (opt[i].argMandatory) ty="<boolean 0/1>";
        else ty="[boolean 0/1]";
        break;
      case CMDOPT_INT:
        if (opt[i].argMandatory) ty="<integer value>";
       else ty="[integer value]";
        break;
      case CMDOPT_DOUBLE:
        if (opt[i].argMandatory) ty="<float value>";
        else ty="[float value]";
        break;
      case CMDOPT_STR:
        if (opt[i].argMandatory) ty="<string>";
        else ty="[string]";
        break;
      default:
        COMP_ERR("showUsage: unknown option type encountered! (ty=%i for opt # %i)",opt[i].type,i);
    }
    if (opt[i].abbr != 0) {
      SMILE_PRINT(" -%c, -%s  \t %s",opt[i].abbr,opt[i].name,ty);
    } else {
      SMILE_PRINT(" -%s      \t %s",opt[i].name,ty);
    }
    if (opt[i].description) {
      SMILE_PRINT("     %s",opt[i].description);
    }
    switch (opt[i].type) {
      case CMDOPT_BOOLEAN:
      case CMDOPT_INT:
        SMILE_PRINT("     {{ default = %i }}",opt[i].dfltInt);
        break;
      case CMDOPT_DOUBLE:
        SMILE_PRINT("     {{ default = %f }}",opt[i].dfltDouble);
        break;
      case CMDOPT_STR:
        SMILE_PRINT("     {{ default = '%s' }}",opt[i].dfltStr);
        break;
      default:
        COMP_ERR("showUsage: unknown option type encountered! (ty=%i for opt # %i)",opt[i].type,i);
    }
    SMILE_PRINT(" ");
  }
}

