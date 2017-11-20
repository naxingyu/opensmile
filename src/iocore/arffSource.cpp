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


/*  openSMILE component:

example dataSource
writes data to data memory...

*/


#include <iocore/arffSource.hpp>



#define MODULE "cArffSource"

/*Library:
sComponentInfo * registerMe(cConfigManager *_confman) {
  cDataSource::registerComponent(_confman);
}
*/
#define N_ALLOC_BLOCK 50

SMILECOMPONENT_STATICS(cArffSource)

//sComponentInfo * cArffSource::registerComponent(cConfigManager *_confman)
SMILECOMPONENT_REGCOMP(cArffSource)
{
  SMILECOMPONENT_REGCOMP_INIT
/*  if (_confman == NULL) return NULL;
  int rA = 0;
  sconfman = _confman;
*/
  scname = COMPONENT_NAME_CARFFSOURCE;
  sdescription = COMPONENT_DESCRIPTION_CARFFSOURCE;

  // we inherit cDataSource configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSource")
/*
  ConfigType *ct = new ConfigType( *(sconfman->getTypeObj("cDataSource")) , scname );
  if (ct == NULL) {
    SMILE_WRN(4,"cDataSource config Type not found!");
    rA=1;
  }*/
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("filename","The filename of the ARFF file to read","input.arff");
    // TODO: support reading a frame time field from the arff file...
    ct->setField("skipFirst","the number of numeric(!) attributes to skip at the beginning",0);
    ct->setField("skipClasses","The number of numeric(!) (or real) attributes (values) at end of each instance to skip (Note: nominal and string attributes are ignored anyway, this option only applies to the last numeric attributes, even if they are followd by string or nominal aatributes). To have more fine grained control over selecting attributes, please use the component cDataSelector!",0);
    ct->setField("saveInstanceIdAsMetadata","1/0 = on/off : save the first string attribute of the arff file as instance ID string the vector meta data (e.g. for use by the winToVecProcessor component in the frameMode=meta mode).",0);
    ct->setField("saveTargetsAsMetadata","1/0 = on/off : save everything after the last numeric attribute as meta-data string. This can be read by arffSink and appended to the instances instead of reading individual targets from the config.",0);
    // TODO: add classes as custom meta data...?

    //ct->setField("selection","indicies of attributes to read (separate indicies by , and specify ranges with a-b e.g. 1-5,7,8). Default ('all') is to read all numeric/real attributes.","all");
  )

/*
    ConfigInstance *Tdflt = new ConfigInstance( scname, ct, 1 );
    sconfman->registerType(Tdflt);
  } else {
    if (ct!=NULL) delete ct;
  }
*/
  SMILECOMPONENT_MAKEINFO(cArffSource);
}

SMILECOMPONENT_CREATE(cArffSource)

//-----

cArffSource::cArffSource(const char *_name) :
  cDataSource(_name),
  field(NULL),
  fieldNalloc(0),
  lineNr(0),
  eof(0),
  lastNumeric(-1),
  strField(-1),
  origline(NULL), lineLen(0)
{
}

void cArffSource::fetchConfig()
{
  cDataSource::fetchConfig();
  
  filename = getStr("filename");
  SMILE_IDBG(2,"filename = '%s'",filename);
  skipClasses = getInt("skipClasses");
  SMILE_IDBG(2,"skipClasses = %i",skipClasses);
  skipFirst = getInt("skipFirst");
  SMILE_IDBG(2,"skipFirst = %i",skipFirst);

  useInstanceID = getInt("saveInstanceIdAsMetadata");
  SMILE_IDBG(2,"saveInstanceIdAsMetadata = %i",useInstanceID);
  saveClassesAsMetadata = getInt("saveTargetsAsMetadata");
  SMILE_IDBG(2,"saveTargetsAsMetadata = %i",saveClassesAsMetadata);
}

/*
int cArffSource::myConfigureInstance()
{
    // call writer->setConfig here to set the dataMemory level configuration and override config file and defaults...
//  double T = 1.0 / (double)(pcmParam.sampleRate);
//  writer->setConfig(1,2*buffersize,T, 0.0, 0.0 , 0, DMEM_FLOAT);  // lenSec = (double)(2*buffersize)*T

  int ret = 1;
  ret *= cDataSource::myConfigureInstance();
  // ...
  return ret;
}
*/

int cArffSource::setupNewNames(long nEl)
{
  // read header lines...
  int ret=1;
  long read;
  char *line;
  int head=1;
  int fnr = 0;
  int nnr = 0;
  // count attributes
  int nAttributes = 0;
  if (filehandle != NULL) {
    fclose(filehandle);
  }
  filehandle = fopen(filename, "r");
  if (filehandle == NULL) {
    COMP_ERR("Error opening file '%s' for reading (component instance '%s', type '%s')",filename, getInstName(), getTypeName());
  }
  do {
    read = smile_getline(&origline, &lineLen, filehandle);
    line = origline;
    if ((read > 0)&&(line!=NULL)) {
      lineNr++;
      if ( (!strncasecmp(line, "@attribute ", 11)) ) {
        char *name = line+11;
        while (*name == ' ') name++;
        char *type = strchr(name,' ');
        if (type != NULL) {
          *(type++) = 0;
          while (*type == ' ') type++; // skip spaces here too
          if ((!strncasecmp(type,"real",4))||(!strncasecmp(type,"numeric",7))) { // add numeric attribute:
            nAttributes++;
          }
        }
      } else if ( (!strncasecmp(line, "@data", 5)) ) {
        head = 0;
      }
    } else {
      // ERROR: EOF in header!!!
      head = 0;
      eof = 1;
      SMILE_ERR(1,"incomplete arff file '%s', could not find '@data' line!",filename);
      ret = 0;
    }
  } while (head);
  if (ret == 0) {
    return 0;
  }
  SMILE_IMSG(3, "Arff file '%s' has %i numeric attributes.", filename, nAttributes);
  head = 1;
  fclose(filehandle);
  filehandle = fopen(filename, "r");
  if (filehandle == NULL) {
    COMP_ERR("Error opening file '%s' for reading (component instance '%s', type '%s')",filename, getInstName(), getTypeName());
  }
  do {
    // TODO: specify index based selection of attributes!
    read = smile_getline(&origline, &lineLen, filehandle);
    line = origline;
    if ((read > 0)&&(line!=NULL)) {
      lineNr++;
      if ( (!strncasecmp(line,"@attribute ",11)) ) {
        char *name = line+11;
        /* bugfix thanks to Ina: skip spaces between @attribute and name! */
        while (*name == ' ') name++;

        char *type = strchr(name,' ');
        if (type != NULL) {
          *(type++) = 0;
          while (*type == ' ') type++; // skip spaces here too
          if ((!strncasecmp(type,"real",4))||(!strncasecmp(type,"numeric",7))) { // add numeric attribute:
            // TODO: correct skip classes option! it currently specifies the number of numeric attributes (from first one) to include!
            if (nnr < nAttributes - skipClasses) {
              if (nnr >= skipFirst) {
              writer_->addField(name,1);
              // TODO: check for [] at end of name and accumulate names to add as array field??
              if (fnr >= fieldNalloc) {
                field = (int*)crealloc( field, sizeof(int)*(fieldNalloc+N_ALLOC_BLOCK), sizeof(int)*(fieldNalloc) );
                fieldNalloc += N_ALLOC_BLOCK;
              }
              field[fnr] = 1;
              lastNumeric = fnr;
              }
              nnr++;
            }
            // TODO: detect array fields [X]
          } else if (!strncasecmp(type,"string",6)) {
            if ((strField == -1)&&(useInstanceID)) strField = fnr; // set strField to point to the FIRST string attribute in the arff file
          }
          fnr++;

        } else { // ERROR:...
          ret=0;
        }
      } else if ( (!strncasecmp(line,"@data",5)) ) {
        head = 0;
      }
    } else {
      head = 0; eof=1;
      SMILE_ERR(1,"incomplete arff file '%s', could not find '@data' line!",filename);
      ret=0;
    } // ERROR: EOF in header!!!
  } while (head);
  //if (line!=NULL) free(line);

  // skip 'skipClasses' numeric classes from end
  /*
  if (skipClasses) {
    int i;
    int s=skipClasses;
    for (i=fnr-1; i>=0; i++) {
      if (field[i]) { field[i]=0; s--; nnr--; }
      if (s<=0) break;
    }
  }*/
 
  nFields = fnr;
  nNumericFields = nnr-skipFirst;

  allocVec(nnr-skipFirst);

  namesAreSet_=1;
  return 1;
}

int cArffSource::myFinaliseInstance()
{
/*  filehandle = fopen(filename, "r");
  if (filehandle == NULL) {
    COMP_ERR("Error opening file '%s' for reading (component instance '%s', type '%s')",filename, getInstName(), getTypeName());
  }*/

  int ret = cDataSource::myFinaliseInstance();

  if (ret == 0) {
    fclose(filehandle); filehandle = NULL;
  }
  return ret;
  
}


int cArffSource::myTick(long long t)
{
  if (isEOI()) return 0;
  
  SMILE_DBG(4,"tick # %i, reading value vector from arff file",t);
  if (eof) {
    SMILE_DBG(4,"(inst '%s') EOF, no more data to read",getInstName());
    return 0;
  }

  if (!(writer_->checkWrite(1))) return 0;
  
  size_t n=0,read;
  char *line=NULL;
  int l=1;
  int len=0;
  int i=0, ncnt=0;
  // get next non-empty line
  do {
    
    read = smile_getline(&origline, &lineLen, filehandle);
    //origline = line;
    line = origline;

    if ((read != -1)&&(line!=NULL)) {
      lineNr++;
      len=(int)strlen(line);
      if (len>0) { if (line[len-1] == '\n') { line[len-1] = 0; len--; } }
      if (len>0) { if (line[len-1] == '\r') { line[len-1] = 0; len--; } }
      while (((line[0] == ' ')||(line[0] == '\t'))&&(len>=0)) { line[0] = 0; line++; len--; }
      if (len > 0) {
        l=0;
        char *x, *x0=line;
        do {
          x = strchr(x0,',');
          if (x!=NULL) {
            *(x++)=0;
          }
          if (field[i]) { // if this field is numeric/selected
            // convert value in x0
            char *ep=NULL;
            double val = strtod( x0, &ep );
            if ((val==0.0)&&(ep==x0)) { SMILE_ERR(1,"error parsing value in arff file '%s' (line %i), expected double value (element %i).",filename,lineNr,i); }
            if (ncnt < vec_->N) vec_->dataF[ncnt++] = (FLOAT_DMEM)val;
            else { SMILE_IERR(1,"more elements in field selection (%i) than allocated in vector (%i)!",ncnt,vec_->N); } // <- should never happen?
          }
          if (i==strField) { // if instance label string field .. add name to vector meta data
            if (vec_->tmeta->metadata.text != NULL) {
              free( vec_->tmeta->metadata.text);
            } 
            vec_->tmeta->metadata.text = strdup(x0);
          }
          if (x!=NULL) {
            // TODO: check for end of numeric attributes.... and add rest as "tmeta->metadata.custom pointer!"
            if ((saveClassesAsMetadata)&&(i>=lastNumeric)&&(lastNumeric>=1)) {
              // add the remaining string to the custom info
              vec_->tmeta->metadata.iData[1] = 1234;
              long ll = (long)(strlen(x)+1);
              if (ll > vec_->tmeta->metadata.customLength) {
                if (vec_->tmeta->metadata.custom != NULL) free(vec_->tmeta->metadata.custom);
                vec_->tmeta->metadata.custom = strdup(x);
                vec_->tmeta->metadata.customLength = ll;
              } else {
                memcpy(vec_->tmeta->metadata.custom, x, ll);
              }
              x = NULL;
            } else {
              i++;
              x0=x;
            }
          }
        } while (x!=NULL);
      }
      //free(line); line = NULL;
    } else {
      l=0; // EOF....  signal EOF...???
      eof=1;
    }
  } while (l);

  if (!eof) {
//    SMILE_IMSG(1,"metadata.text = '%s'",vec->tmeta->metadata.text);
    writer_->setNextFrame(vec_);
    return 1;
  }
  
  return 0;
}


cArffSource::~cArffSource()
{
  if (filehandle!=NULL) fclose(filehandle);
  if (field != NULL) free(field);
  if (origline != NULL) free(origline);
}
