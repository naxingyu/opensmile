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


#include <core/dataMemory.hpp>
#include <core/componentManager.hpp>

#define MODULE "dataMemory"

SMILECOMPONENT_STATICS(cDataMemory)

SMILECOMPONENT_REGCOMP(cDataMemory)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CDATAMEMORY;
  sdescription = COMPONENT_DESCRIPTION_CDATAMEMORY;

  // dataMemory level's config type:
  ConfigType *dml = new ConfigType("cDataMemoryLevel");
  if (dml == NULL) OUT_OF_MEMORY;
  dml->setField("name", "The name of this data memory level, must be unique within one data memory instance.", (char*)NULL);
  dml->setField("type", "The data type of the level [can be: 'float' or 'int'(eger) , currently only float is supported by the majority of processing components ]", "float");
  dml->setField("isRb", "Flag that indicates whether this level is a ring-buffer level (1) or not (0). I.e. this level stores the last 'nT' frames, and discards old data as new data comes in (if the old data has already been read by all registered readers; if this is not the case, the level will report that it is full to the writer attempting the write operation)", 1);
  dml->setField("nT", "The size of the level buffer in frames (this overwrites the 'lenSec' option)", 100,0,0);
  dml->setField("T", "The frame period of this level in seconds. Use a frame period of 0 for a-periodic levels (i.e. data that does not occur periodically)", 0.0);
  dml->setField("lenSec", "The size of the level buffer in seconds", 0.0);
  dml->setField("frameSizeSec", "The size of one frame in seconds. (This is generally NOT equal to 1/T, because frames may overlap)", 0.0);
  dml->setField("growDyn", "Supported currently only if 'ringbuffer=0'. If this option is set to 1, the level grows dynamically, if more memory is needed. You can use this to store live input of arbirary length in memory. However, be aware that if openSMILE runs for a long time, it will allocate more and more memory!", 0);
  dml->setField("noHang", "This option controls the 'hang' behaviour for ring-buffer levels, i.e. the behaviour exhibited, when the level is full because data from the ring-buffer has not been marked as read because not all readers registered to read from this level have read data. Valid options are, 0, 1, 2 :\n   0 = always wait for readers, mark the level as full and make writes fail until all readers have read at least the next frame from this level\n   1 = don't wait for readers, if no readers are registered, i.e. this level is a dead-end (this is the default) / 2 = never wait for readers, writes to this level will always succeed (reads to non existing data regions might then fail), use this option with a bit of caution.", 1);

  // dataMemory's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE

  ct->setField("isRb", "The default for the isRb option for all levels.", 1,0,0);
  ct->setField("nT", "The default level buffer size in frames for all levels.", 100,0,0);
  if (ct->setField("level", "An associative array containing the level configuration (obsolete, you should use the cDataWriter configuration in the components that write to the dataMemory to properly configure the dataMemory!)",
                  dml, 1) == -1) {
     rA=1; // if subtype not yet found, request , re-register in the next iteration
  }

  SMILECOMPONENT_IFNOTREGAGAIN( {} )
  
  SMILECOMPONENT_MAKEINFO(cDataMemory);
}

SMILECOMPONENT_CREATE(cDataMemory)



void TimeMetaInfoMinimal::cloneFrom(const TimeMetaInfo *tm)
{
  if (tm != NULL) {
    vIdx = tm->vIdx;
    period = (float)tm->period;
  }
}

/**** frame meta information *********
   names, indicies (for each row)
 **************************************/

// the returned name is the base name belonging to the expanded field n
// arrIdx will be the index in the array (if an array)
//   or -1 if no array!
//-> n is element(!) index
const char * FrameMetaInfo::getName(int n, int *_arrIdx)
{
  smileMutexLock(myMtx);

  long  lN=0;
  lN = Ne; 

  static long nn=0; ////
  static long ff=0; ////

  if ((n>=0)&&(n<lN)) {
    long f=0,tmp=0;

    if (n>nn) {f=ff; tmp=nn; } //// fast, but unsafe? TODO: check!
    //TODO: this code is NOT thread safe. solutions:
    // a) use a mutex... this actually should do the job...
    // b) use a lookup table for n->f which must be built earlier or here if it doesn exist (but then we must use mutexes again to ensure exclusive building and waiting of other threads for the built index)
    
    for(; f<N; f++) {
      nn=tmp; ff=f; ////
      tmp += field[f].N;
      if (tmp>n) break;
    }
    if (_arrIdx != NULL) {
      if (field[f].N > 1)
        *_arrIdx = n-(tmp-field[f].N) + field[f].arrNameOffset;
      else
        *_arrIdx = -1;
    }
    smileMutexUnlock(myMtx);
    return field[f].name;
  }
  smileMutexUnlock(myMtx);
  return NULL;
}

long FrameMetaInfo::fieldToElementIdx(long _field, long _arrIdx) const
{
  int i;
  int lN=0;
  if (_field >= N) _field = lN-1;
  for(i=0; i<_field; i++) {
    lN += field[i].N;
  }
  return lN + _arrIdx;
}

long FrameMetaInfo::elementToFieldIdx(long _element, long *_arrIdx) const
{
  int i;
  int  lN=0, _prevN = 0;
  for(i=0; i<N; i++) {
    lN += field[i].N;
    if (lN > _element) {
      if (_arrIdx != NULL) *_arrIdx = field[i].N - (lN - _element);
      return i;
    }
  }
  if (_arrIdx != NULL) *_arrIdx = 0;
  return -1;
}


// returned *arrIdx will be the real index in the data structure, i.e. named index - arrNameOffset
int FrameMetaInfo::findField(const char*_fieldName, int *arrIdx, int *more) const
{
  if (_fieldName == NULL) return -1;

  // check for [] in _fieldName => array field
  long ai = 0;
  char *fn = strdup(_fieldName);
  char *a  = strchr(fn,'[');
  if (a != NULL) {  // yes, array
    a[0] = 0;
    a++;
    char *b = strchr(fn,']');
    if (b == NULL) { COMP_ERR("findField: invalid array field name '%s', expected ']' at the end!",_fieldName); }
    b[0] = 0;
    char *eptr = NULL;
    ai = strtol(a,&eptr,10);
    if ((ai==0)&&(eptr == a)) { COMP_ERR("findField: error parsing array index in name '%s', index is not a number!",_fieldName); }
  } 
  if (arrIdx != NULL) *arrIdx = ai;

  int i;
  int idx = -1;
  int _start = 0;
  if ((more != NULL)&&(*more>0)) { _start = *more; *more = 0; }
  int foundFlag = 0;
  for (i=_start; i<N; i++) {
    int myAi;
    if (!foundFlag) {
      if (a != NULL) // array name
        myAi = ai - field[i].arrNameOffset;
      else // non array name (first field of array, always index 0 in data structure)
        myAi = 0;
    }
    if (!strcmp(fn,field[i].name)) {
      if (!foundFlag) {
        idx = i;
        if (myAi < field[i].N) {
          //idx = field[i].Nstart + myAi;
          if (arrIdx != NULL) *arrIdx = myAi; // update *arrIdx with correct index (real index in data structure, not 'named' index)
          if (more == NULL) break;
          else foundFlag=1;
        } else {
          COMP_ERR("array index out of bounds (field '%s') %i > %i (must from %i - %i) (NOTE: first index is 0, not 1!)",_fieldName,ai,field[i].N-1+field[N].arrNameOffset,field[N].arrNameOffset,field[i].N-1+field[N].arrNameOffset);
        }
      } else {
        if (more != NULL) (*more)++;
      }
    }
  }
  free(fn);
  return idx;
}

// returned *arrIdx will be the real index in the data structure, i.e. named index - arrNameOffset
int FrameMetaInfo::findFieldByPartialName(const char*_fieldNamePart, int *arrIdx, int *more) const
{
  if (_fieldNamePart == NULL) return -1;

  // check for [] in _fieldName => array field
  long ai = 0;
  char *fn = strdup(_fieldNamePart);
  char *a  = strchr(fn,'[');
  if (a != NULL) {  // yes, array
    a[0] = 0;
    a++;
    char *b = strchr(fn,']');
    if (b == NULL) { COMP_ERR("findField: invalid array field name part '%s', expected ']' at the end!",_fieldNamePart); }
    b[0] = 0;
    char *eptr = NULL;
    ai = strtol(a,&eptr,10);
    if ((ai==0)&&(eptr == a)) { COMP_ERR("findField: error parsing array index in name part '%s', index is not a number!",_fieldNamePart); }
  }
  if (arrIdx != NULL) *arrIdx = ai;

  int i;
  int idx = -1;
  int _start = 0;
  if ((more != NULL)&&(*more>0)) { _start = *more; *more = 0; }
  int foundFlag = 0;
  for (i=_start; i<N; i++) {
    int myAi;
    if (!foundFlag) {
      if (a != NULL) // array name
        myAi = ai - field[i].arrNameOffset;
      else // non array name (first field of array, always index 0 in data structure)
        myAi = 0;
    }
    //    printf("name(%i): '%s'\n",i,field[i].name);
    if (strstr(field[i].name,fn)) {
      if (!foundFlag) {
        idx = i;
        if (myAi < field[i].N) {
          if (arrIdx != NULL) *arrIdx = myAi; // update *arrIdx with correct index (real index in data structure, not 'named' index)
          if (more == NULL) break;
          else foundFlag=1;
        } else {
          COMP_ERR("array index out of bounds (partial field name '%s') %i > %i (must from %i - %i) (NOTE: first index is 0, not 1!)",_fieldNamePart,ai,field[i].N-1+field[N].arrNameOffset,field[N].arrNameOffset,field[i].N-1+field[N].arrNameOffset);
        }
      } else {
        if (more != NULL) *more++;
      }
    }
  }
  free(fn);
  return idx;
}

/******* datatypes ************/

cVector::cVector(int lN, int _type) :
  N(0), dataF(NULL), dataI(NULL), fmeta(NULL), tmeta(NULL), ntmp(NULL), tmetaAlien(0), tmetaArr(0)
{
  if (lN>0) {
    switch (_type) {
      case DMEM_FLOAT:
        dataF = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*lN);
        if (dataF==NULL) OUT_OF_MEMORY;
        break;
      case DMEM_INT:
        dataI = (INT_DMEM*)calloc(1,sizeof(INT_DMEM)*lN);
        if (dataI==NULL) OUT_OF_MEMORY;
        break;
      default:
        COMP_ERR("cVector: unknown data type encountered in constructor! (%i)",_type);
    }
    N = lN;
    type = _type;
    tmeta = new TimeMetaInfo(); //(TimeMetaInfo *)calloc(1,sizeof(TimeMetaInfo));
    if (tmeta == NULL) OUT_OF_MEMORY;
  }
}

const char *cVector::name(int n, int *lN)
{
  if ((fmeta!=NULL)&&(fmeta->field!=NULL)) {
    if (ntmp != NULL) free(ntmp);
    int llN=-1;
    const char *t = fmeta->getName(n,&llN);
    if (lN!=NULL) *lN = llN;
    if (llN>=0) {
      ntmp=myvprint("%s[%i]",t,llN);
      return ntmp;
    } else {
      ntmp=NULL;
      return t;
    }
  }
  return NULL;
}

cVector::~cVector() {
  if (dataF!=NULL) free(dataF);
  if (dataI!=NULL) free(dataI);
  if ((tmeta!=NULL)&&(!tmetaAlien)) {
    if (tmetaArr) delete[] tmeta;
    else delete tmeta;
  }
  if (ntmp!=NULL)  free(ntmp);
}

cMatrix::~cMatrix()
{
  if ((tmeta!=NULL)&&(!tmetaAlien)) {
//    if (nT > 1)
    if (tmetaArr) delete[] tmeta;
    else delete tmeta;
//    else
//      delete tmeta;
  }
  tmeta = NULL;
}

cMatrix::cMatrix(int lN, int lnT, int ltype) :
  cVector(0), nT(0)
{
  if ((lN>0)&&(lnT>0)) {
    switch (ltype) {
      case DMEM_FLOAT:
        dataF = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*lN*lnT);
        if (dataF==NULL) OUT_OF_MEMORY;
        break;
      case DMEM_INT:
        dataI = (INT_DMEM*)calloc(1,sizeof(INT_DMEM)*lN*lnT);
        if (dataI==NULL) OUT_OF_MEMORY;
        break;
      default:
        COMP_ERR("cMatrix: unknown data type encountered in constructor! (%i)",ltype);
    }
    N = lN;
    nT = lnT;
    type = ltype;
    tmetaArr = 1;
    tmeta = new TimeMetaInfo[lnT]; //(TimeMetaInfo *)calloc(1,sizeof(TimeMetaInfo)*_nT);
    if (tmeta == NULL) OUT_OF_MEMORY;
  }
}

/* helper function for transposing a matrix */
static void quickTranspose(void *src, void *dst, long R, long C, int elSize)
{
  int r,c;
  char *s=(char*)src, *d=(char*)dst;
  for (r=0; r<R; r++) {  // read each row
    for (c=0; c<C; c++) { // and copy it as column
      memcpy( d+(r*C+c)*elSize, s+(c*R+r)*elSize, elSize );
    }
  }
}

// transpose matrix
void cMatrix::transpose()
{
  FLOAT_DMEM *f = NULL;
  INT_DMEM *i = NULL;
  // transpose data:
  switch (type) {
    case DMEM_FLOAT:
      f = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*N*nT);
      if (f==NULL) OUT_OF_MEMORY;
      // transpose:
      quickTranspose(dataF,f,N,nT,sizeof(FLOAT_DMEM));
      free(dataF);
      dataF = f;
      break;
    case DMEM_INT:
      i = (INT_DMEM*)calloc(1,sizeof(INT_DMEM)*N*nT);
      if (i==NULL) OUT_OF_MEMORY;
      // transpose:
      quickTranspose(dataI,i,N,nT,sizeof(INT_DMEM));
      free(dataI);
      dataI = i;
      break;
    default:
      COMP_ERR("cMatrix::transpose: unknown data type (%i) encountered, cannot transpose this matrix!",type);
  }
  // swap dimensions:
  long tmp = N;
  N = nT;
  nT = tmp;
}

// reshape to given dimensions, perform sanity check...
void cMatrix::reshape(long R, long C)
{
  long len = N*nT;
  if (R*C > len) SMILE_ERR(1,"reshape: cannot reshape matrix, new size (%i*%i=%i) is greater than current size (%i*%i=%i)! this is not yet implemented! \n   (maybe there should be a resize method in the future...)",R,C,R*C,N,nT,len);
  if (R*C < len) SMILE_WRN(1,"reshape: new matrix will be smaller than current matrix, not all data will be accessible (maybe use resize() in future releases!");
  R=N;
  C=nT;
}

// convert matrix to long vector with all columns concatenated ...
// return a compatible cVector object
const cVector * cMatrix::expand()
{
  N = N*nT;
  return (cVector*)this;
}


/******* dataMemory level class ************/


void cDataMemoryLevel::frameWr(long rIdx, FLOAT_DMEM *_data)
{
  FLOAT_DMEM *f = data->dataF + rIdx*lcfg.N;
  FLOAT_DMEM *end = f+lcfg.N;
  for ( ; f < end; f++) {
    *f = *_data;
    _data++;
  }
}

void cDataMemoryLevel::frameWr(long rIdx, INT_DMEM *_data)
{
  INT_DMEM *f = data->dataI + rIdx*lcfg.N;
  INT_DMEM *end = f+lcfg.N;
  for ( ; f < end; f++) {
    *f = *_data;
    _data++;
  }
}

void cDataMemoryLevel::frameRd(long rIdx, FLOAT_DMEM *_data)
{
  FLOAT_DMEM *f = data->dataF + rIdx*lcfg.N;
  FLOAT_DMEM *end = f+lcfg.N;
  for ( ; f < end; f++) {
    *_data = *f;
    _data++;
  }
}

void cDataMemoryLevel::frameRd(long rIdx, INT_DMEM *_data)
{
  INT_DMEM *f = data->dataI + rIdx*lcfg.N;
  INT_DMEM *end = f+lcfg.N;
  for ( ; f < end; f++) {
    *_data = *f;
    _data++;
  }
}

/*
const sDmLevelConfig * cDataMemoryLevel::getConfig()
{
  if (lcfg == NULL) {
    lcfg = new sDmLevelConfig();
    if (lcfg==NULL) OUT_OF_MEMORY;
  }
  if (!finalised) {
    lcfg->growDyn = growDyn;
    lcfg->isRb = isRb;
    lcfg->N = N;
    lcfg->nT = nT;
    lcfg->T = T;
    lcfg->lenSec = T*(double)nT;
    lcfg->frameSizeSec = frameSizeSec;
    lcfg->type = type;
    lcfg->fmeta = &fmeta;
    lcfg->finalised = finalised;
  } else {
    lcfg->finalised = finalised;
//    lcfg->N = N;
//    lcfg->fmeta = &fmeta;
  }
  return lcfg;
}
*/

void cDataMemoryLevel::printLevelStats(int detail)
{
  if (detail) {
    SMILE_PRINT("==> LEVEL '%s'  +++  Buffersize(frames) = %i  +++  nReaders = %i",getName(),lcfg.nT,nReaders);
    if (detail >= 2) {
    // TODO: more details  AND warn if nReaders == 0 or size == 0, etc.
      SMILE_PRINT("     Period(in seconds) = %f \t frameSize(in seconds) = %f (last: %f)",lcfg.T,lcfg.frameSizeSec,lcfg.lastFrameSizeSec);
      SMILE_PRINT("     BlocksizeRead(frames) = %i \t BlocksizeWrite(frames) = %i",lcfg.blocksizeReader,lcfg.blocksizeWriter);
      if (detail >= 3) {
        SMILE_PRINT("     Number of elements: %i \t Number of fields: %i",lcfg.N,lcfg.Nf);
        if (detail >= 4) {
          const char * typeStr = "unknown";
          if (lcfg.type==DMEM_FLOAT) { typeStr="float"; }
          else if (lcfg.type==DMEM_INT) { typeStr="int"; }
          SMILE_PRINT("     type = %s   noHang = %i   isRingbuffer(isRb) = %i   growDyn = %i",typeStr,lcfg.noHang,lcfg.isRb,lcfg.growDyn);
          if (detail >= 5) {
            // TODO: print data element names ??
            int i; long idx = 0;
            SMILE_PRINT("     Fields: index (range) : fieldname[array indicies]  (# elements)");
            for (i=0; i<fmeta.N; i++) {
              if (fmeta.field[i].N > 1) {
                SMILE_PRINT("      %2i. - %2i. : %s[%i-%i]  (%i)",idx,idx+fmeta.field[i].N-1,fmeta.field[i].name,fmeta.field[i].arrNameOffset,fmeta.field[i].N+fmeta.field[i].arrNameOffset-1,fmeta.field[i].N);
                idx += fmeta.field[i].N;
              } else {
                SMILE_PRINT("      %2i.       : %s",idx,fmeta.field[i].name);
                idx++;
              }
            }
          }
          if (detail >= 6) {
            SMILE_PRINT("     Fields with info struct set: (index (range) : info struct size in bytes (dt = datatype))");
            long idx = 0;
            for (int i = 0; i < fmeta.N; i++) {
              if (fmeta.field[i].infoSet) {
                if (fmeta.field[i].N > 1) {
                  SMILE_PRINT("       %2i. - %2i. : infoSize = %i (dt = %i)", idx, idx+fmeta.field[i].N-1, fmeta.field[i].infoSize, fmeta.field[i].dataType);
                  idx += fmeta.field[i].N;
                } else {
                  SMILE_PRINT("       %2i.       : infoSize = %i (dt = %i)", idx, fmeta.field[i].infoSize, fmeta.field[i].dataType);
                  idx++;
                }
              }
            }
          }
        }
      }
    }
  }
  if (nReaders <= 0) SMILE_WRN(1,"   Level '%s' might be a DEAD-END (nReaders <= 0!)",getName());
}

const char * cDataMemoryLevel::getFieldName(int n, int *lN, int *_arrNameOffset)
{
      //TODO::: use fmeta getName and implement expanding of array indcies... (see cVector::name)
  if ( (n>=0)&&(n<lcfg.N) ) {
    if (lN!=NULL) *lN = fmeta.field[n].N;
	if (_arrNameOffset!=NULL) *_arrNameOffset = fmeta.field[n].arrNameOffset;
    return fmeta.field[n].name;
  }
  else return NULL;
}

char * cDataMemoryLevel::getElementName(int n)
{
  if ( (n>=0)&&(n<lcfg.N) ) { // TODO:: is N correct? number of elements??
    int llN=0;
    const char *tmp = fmeta.getName(n,&llN);
    char *ret;
    if (llN >= 0) ret = myvprint("%s[%i]",tmp,llN);
    else ret = strdup(tmp); //myvprint("%s",tmp,__N);
    return ret;
  }
  else return NULL;
}

void cDataMemoryLevel::allocReaders() {
  // allocate and initialize *curRr
  if (nReaders > 0) { // if registered readers are present...
    curRr = (long*)calloc(1,sizeof(long)*nReaders);
  }
}


// set time meta information for frame at rIdx
void cDataMemoryLevel::setTimeMeta(long rIdx, long vIdx, const TimeMetaInfo *tm)
{
  TimeMetaInfo *cur = tmeta+rIdx;
  if (tm!=NULL) {
    //memcpy( cur, tm, sizeof(TimeMetaInfo) );
    cur->cloneFrom(tm); // <<-new!
    cur->level = myId;
    cur->vIdx =  vIdx;
    cur->period = lcfg.T;
    // keep an existing smiletime, else set the current smile time
    if (tm->smileTime >= 0.0) cur->smileTime = tm->smileTime;
    else {
      if (!tm->noAutoSmileTime || cur->smileTime == -1.0 ) {
        if (_parent != NULL) {
          cComponentManager * cm = (cComponentManager *)_parent->getCompMan();
          if (cm != NULL) {
            cur->smileTime = cm->getSmileTime();
          } else { cur->smileTime = -1.0; }
        } else { cur->smileTime = -1.0; }
      }
    }
    if (cur->time == 0.0)
      if (lcfg.T!=0.0) cur->time = (double)vIdx * lcfg.T;
      else {
        cur->time = tmeta[(rIdx-1+lcfg.nT)%lcfg.nT].time + tmeta[(rIdx-1+lcfg.nT)%lcfg.nT].lengthSec;
      }
    if (!(cur->filled)) {
      if (cur->lengthSec == 0.0) cur->lengthSec = tmeta[(rIdx-1+lcfg.nT)%lcfg.nT].lengthSec;
      if (cur->lengthSec == 0.0) cur->lengthSec = (double)(cur->lengthSamples) * cur->samplePeriod;
      if (cur->lengthSec == 0.0) cur->lengthSec = (double)(cur->lengthFrames) * cur->framePeriod;
      if (cur->lengthSec == 0.0) cur->lengthSec = lcfg.T;
      if (cur->vLengthSec == 0.0) cur->vLengthSec = cur->lengthSec;

      if ((cur->lengthSamples == 0)&&(cur->samplePeriod != 0.0)) cur->lengthSamples = (int)(cur->lengthSec/cur->samplePeriod)+1;
      if ((cur->vLengthSamples == 0)&&(cur->samplePeriod != 0.0)) cur->vLengthSamples = (int)(cur->vLengthSec/cur->samplePeriod)+1;

      if ((cur->lengthFrames == 0)&&(cur->framePeriod != 0.0)) cur->lengthFrames = (int)(cur->lengthSec/cur->framePeriod)+1;
      if ((cur->vLengthFrames == 0)&&(cur->framePeriod != 0.0)) cur->vLengthFrames = (int)(cur->vLengthSec/cur->framePeriod)+1;

      if (cur->vLengthFrames == 0) cur->vLengthFrames = cur->lengthFrames;
      if (cur->vLengthSamples == 0) cur->vLengthSamples = cur->lengthSamples;

      if ((cur->samplePeriod == 0.0)&&(cur->lengthSamples!=0)) cur->samplePeriod = cur->lengthSec/(double)(cur->lengthSamples);
      if ((cur->framePeriod == 0.0)&&(cur->lengthSamples!=0)) cur->framePeriod = cur->lengthSec/(double)(cur->lengthFrames);

      cur->filled = 1;
    }
  } else {
    // zero current time meta??
  }
}

// get time meta information for frame at rIdx
void cDataMemoryLevel::getTimeMeta(long rIdx, TimeMetaInfo *tm)
{
  TimeMetaInfo *cur = tmeta+rIdx;
  if (tm!=NULL) {
    tm->cloneFrom(cur);
    //memcpy( tm, cur, sizeof(TimeMetaInfo) );
  }
}

// Note: the memory pointed to by _info is freed upon destruction of the data memory level by the framemeta object!
int cDataMemoryLevel::setFieldInfo(int i, int _dataType, void *_info, long _infoSize)
{
  if ((i>=0)&&(i < fmeta.N)) {
    if (_dataType >= 0) fmeta.field[i].dataType = _dataType;
    if (_infoSize >= 0) fmeta.field[i].infoSize = _infoSize;
    if (_info != NULL) {
      if (fmeta.field[i].info != NULL) free(fmeta.field[i].info);
      fmeta.field[i].info = _info;
    }
    fmeta.field[i].infoSet = 1;
    return 1;
  }
  return 0;
}

int cDataMemoryLevel::addField(const char *_name, int lN, int arrNameOffset)
{
  if (lcfg.finalised) {
    SMILE_ERR(2,"cannot add field '%s' to level '%s' , level is already finalised!",_name,getName());
    return 0;
  }
  //TODO: check for uniqueness of name!!

  if (fmeta.N >= fmetaNalloc) { //realloc:
    FieldMetaInfo *f;
    f = (FieldMetaInfo *)realloc(fmeta.field,sizeof(FieldMetaInfo)*(fmeta.N+LOOKAHEAD_ALLOC));
    if (f==NULL) OUT_OF_MEMORY;
    // zero newly allocated data
    int i;
    for (i=fmeta.N; i<fmeta.N+LOOKAHEAD_ALLOC; i++) {
      f[i].name = NULL;
      f[i].N = 0;
      f[i].dataType = 0;
      f[i].info = NULL;
      f[i].infoSize = 0;
      f[i].infoSet = 0;
    }
    fmeta.field = f;
    fmetaNalloc = fmeta.N + LOOKAHEAD_ALLOC;
  }
  if (lN <= 0) 
    lN = 1;
  fmeta.field[fmeta.N].N = lN;
  fmeta.field[fmeta.N].Nstart = lcfg.N;
  fmeta.field[fmeta.N].name = strdup(_name);
  fmeta.field[fmeta.N].arrNameOffset = arrNameOffset;
  fmeta.N++; 
  lcfg.Nf++; 
  fmeta.Ne += lN;
  lcfg.N += lN;
    
  SMILE_DBG(4,"added field '%s' to level '%s' (size %i).",_name,getName(),lN);
  return 1;
}

void cDataMemoryLevel::setArrNameOffset(int arrNameOffset)
{
  if ((fmeta.N-1 < fmetaNalloc)&&(fmeta.N-1 >= 0)) {
    fmeta.field[fmeta.N-1].arrNameOffset = arrNameOffset;
  }
}

int cDataMemoryLevel::configureLevel()   // finalize config and allocate data memory
{
  /*
  if (lcfg.blocksizeIsSet) return 1;

  // TODO: what is the actual minimum buffersize, given blocksizeRead and blocksizeWrite??
  long minBuf;  // = blocksizeRead + 2*blocksizeWrite;
  if (lcfg.blocksizeReader <= lcfg.blocksizeWriter) {
    minBuf = 2*lcfg.blocksizeWriter + 1;  // +1 just for safety...
  } else {
    minBuf = MAX( 2*lcfg.blocksizeWriter, (lcfg.blocksizeReader/lcfg.blocksizeWriter) * lcfg.blocksizeWriter ) + 1; // +1 just for safety...
  }

  // adjust level buffersize based on blocksize from write requests...
  if (lcfg.nT<minBuf) {
    lcfg.nT = minBuf;
  }

  lcfg.blocksizeIsSet = 1;
  */
    return 1;
}

int cDataMemoryLevel::finaliseLevel()   // finalize config and allocate data memory
{
    //if (lcfg.blocksizeIsSet) return 1;
  if (lcfg.finalised) return 1;

  // TODO: what is the actual minimum buffersize, given blocksizeRead and blocksizeWrite??
  long minBuf;  // = blocksizeRead + 2*blocksizeWrite;
  if (lcfg.blocksizeReader <= lcfg.blocksizeWriter) {
    minBuf = 2*lcfg.blocksizeWriter + 1;  // +1 just for safety...
  } else {
    //minBuf = MAX( 2*lcfg.blocksizeWriter, (lcfg.blocksizeReader/lcfg.blocksizeWriter) * lcfg.blocksizeWriter ) + 1; // +1 just for safety...
    minBuf = lcfg.blocksizeReader + 2 * lcfg.blocksizeWriter; // +1 just for safety...
  }

  // adjust level buffersize based on blocksize from write requests...
  if (lcfg.nT<minBuf) {
    lcfg.nT = minBuf;
  }

  lcfg.blocksizeIsSet = 1;
  
  //if (lcfg.finalised) return 1;

  if ( (!lcfg.blocksizeIsSet) || (!lcfg.namesAreSet) ) {
    COMP_ERR("cannot finalise level '%s' : blocksizeIsSet=%i, namesAreSet=%i (both should be 1...)",getName(),lcfg.blocksizeIsSet,lcfg.namesAreSet);
  }

  // allocate data matrix
  if ((lcfg.N<=0)||(lcfg.nT<=0)) COMP_ERR("cDataMemoryLevel::finaliseLevel: cannot allocate matrix with one (or more) dimensions == 0. did you add fields to this level ['%s']? (N=%i, nT=%i)",getName(),lcfg.N,lcfg.nT);
  data = new cMatrix(lcfg.N,lcfg.nT,lcfg.type);
  if (data==NULL) COMP_ERR("cannot allocate level of unknown type %i or out of memory!",lcfg.type);
  
  // allocate tmeta
  //tmeta = (TimeMetaInfo *)calloc(1,sizeof(TimeMetaInfo) * lcfg.nT);
  tmeta = new TimeMetaInfo[lcfg.nT]();
  if (tmeta == NULL) OUT_OF_MEMORY;
  
  // initialise mutexes:
  smileMutexCreate(RWptrMtx);
  smileMutexCreate(RWmtx);
  smileMutexCreate(RWstatMtx);
  
  lcfg.finalised = 1;
  return 1;
}

void cDataMemoryLevel::catchupCurR(int rdId, int _curR) 
{
  smileMutexLock(RWptrMtx);
  if ((rdId < 0)||(rdId >= nReaders)) { 
    if ((_curR >= 0)&&(_curR <= curW)) curR = _curR;
    else curR = curW-1;
    checkCurRr(); // sync new global with readers' indicies
  } else {
    if ((_curR >= 0)&&(_curR <= curW)) curRr[rdId] = _curR;
    else curRr[rdId] = curW-1;
    checkCurRr(); // sync new readers' indicies with global
  }
  smileMutexUnlock(RWptrMtx);
}


int cDataMemoryLevel::setFrame(long vIdx, const cVector *vec, int special)  // id must already be resolved...!
{
  if (!lcfg.finalised) { COMP_ERR("cannot set frame in non-finalised level! call finalise() first!"); }
  if (vec == NULL) {
    SMILE_ERR(3,"cannot set frame in dataMemory from a NULL cVector object!");
    return 0;
  }

  if (lcfg.N != vec->N) { COMP_ERR("setFrame: cannot set frame in level '%s', framesize mismatch: %i != %i (expected)",getName(),vec->N,lcfg.N); }
  if (lcfg.type != vec->type) { COMP_ERR("setFrame: frame type mismtach between frame and level (frame=%i, level=%i)",vec->type,lcfg.type); }

//****** acquire write lock.... *******
  smileMutexLock(RWstatMtx);
  // set write request flag, incase the level is currently locked for reading
  writeReqFlag = 1;
  smileMutexUnlock(RWstatMtx);
  smileMutexLock(RWmtx); // get exclusive lock for writing..
  smileMutexLock(RWstatMtx);
  writeReqFlag = 0;
  smileMutexUnlock(RWstatMtx);
//****************

  smileMutexLock(RWptrMtx);
#ifdef DM_DEBUG_LOGGER
  long vIdx0=vIdx;
#endif
  long rIdx = validateIdxW(&vIdx,special);
#ifdef DM_DEBUG_LOGGER
  // logging in setFrame:
  datamemoryLogger(this->lcfg.name, vIdx0, vIdx, rIdx, lcfg.nT, special, this->curR, this->curW, this->EOI, this->nReaders, this->curRr, vec);
#endif
  smileMutexUnlock(RWptrMtx);
  
  int ret = 0;
  if (rIdx>=0) {
    if (lcfg.type == DMEM_FLOAT) frameWr(rIdx, vec->dataF);
    else if (lcfg.type == DMEM_INT)   frameWr(rIdx, vec->dataI);
    setTimeMeta(rIdx,vIdx,vec->tmeta);
    //setMetaData(rIdx,vIdx,vec->metadata);
    ret= 1;
  } else {
    SMILE_ERR(4,"setFrame: frame index (vIdx %i -> rIdx %i) out of range, frame was not set (level '%s')!",vIdx,rIdx,getName());
  }

  smileMutexUnlock(RWmtx);
  return ret;
}

int cDataMemoryLevel::setMatrix(long vIdx, const cMatrix *mat, int special)  // id must already be resolved...!
{
  if (!lcfg.finalised) { COMP_ERR("cannot set matrix in non-finalised level '%s'! call finalise() first!",getName()); }
  if (mat == NULL) {
    SMILE_ERR(3,"cannot set frame in dataMemory from a NULL cMatrix object!");
    return 0;
  }

  if (lcfg.N != mat->N) { COMP_ERR("setMatrix: cannot set frames in level '%s', framesize mismatch: %i != %i (expected)",getName(),mat->N,lcfg.N); }
  if (lcfg.type != mat->type) { COMP_ERR("setMatrix: frame type mismtach between frame and level (frame=%i, level=%i)",mat->type,lcfg.type); }

//****** acquire write lock.... *******
  smileMutexLock(RWstatMtx);
  // set write request flag, incase the level is currently locked for reading
  writeReqFlag = 1;
  smileMutexUnlock(RWstatMtx);
  smileMutexLock(RWmtx); // get exclusive lock for writing..
  smileMutexLock(RWstatMtx);
  writeReqFlag = 0;
  smileMutexUnlock(RWstatMtx);
//****************

  // validate start index
  smileMutexLock(RWptrMtx);
  long rIdx = validateIdxRangeW(&vIdx,vIdx+mat->nT,special);
  smileMutexUnlock(RWptrMtx);

  int ret = 0;
  if (rIdx>=0) {
    long i;
   /* double smileTm = -1.0;
    if (_parent != NULL) {
      cComponentManager * cm = (cComponentManager *)_parent->getCompMan();
      if (cm != NULL) {
        smileTm = cm->getSmileTime();
      }
    }
    */

    if (lcfg.type == DMEM_FLOAT) for (i=0; i<mat->nT; i++) { 
      /*if (((mat->tmeta+i)->noAutoSmileTime)) {
        //(mat->tmeta + i)->smileTime = -1.0;
        printf("XXXXXXXXXXXXXXXx clear.... %i\n",(mat->tmeta+i)->noAutoSmileTime);
      }*/
      //if (!noAutoSmileTime || (mat->tmeta + i)->smileTime == -1.0 ) (mat->tmeta + i)->smileTime = smileTm;
      frameWr((rIdx+i)%lcfg.nT, mat->dataF + i*lcfg.N); setTimeMeta((rIdx+i)%lcfg.nT,vIdx+i,mat->tmeta + i ); 
    }
    else if (lcfg.type == DMEM_INT) for (i=0; i<mat->nT; i++) { 
      if (!((mat->tmeta+i)->noAutoSmileTime)) (mat->tmeta + i)->smileTime = -1.0;
      //if (!noAutoSmileTime || (mat->tmeta + i)->smileTime == -1.0 ) (mat->tmeta + i)->smileTime = smileTm;
      frameWr((rIdx+i)%lcfg.nT, mat->dataI + i*lcfg.N); setTimeMeta((rIdx+i)%lcfg.nT,vIdx+i,mat->tmeta + i);
    }
    ret = 1;
  } else {
    SMILE_DBG(4,"ERROR, setMatrix: frame index range (vIdxStart %i - vIdxEnd %i  => rIdxStart %i) out of range, frame was not set (level '%s')!",vIdx,vIdx+mat->nT,rIdx,getName());
  }

  smileMutexUnlock(RWmtx);
  return ret;
}


//TODO: implement concealment strategies, when frames are not available (also report concealment method to caller)
//    for vector: return 0'ed frame instead of NULL pointer
//                return last available frame (beginning / end)
//    for matrix: return NULL pointer if complete matrix is out of range
//                fill partially (!) missing frames with 0es
//                repeat first/last possible frames...

// NOTE: caller must free the returned vector!!
cVector * cDataMemoryLevel::getFrame(long vIdx, int special, int rdId, int *result)
{
  if (!lcfg.finalised) { COMP_ERR("cannot get frame from non-finalised level '%s'! call finalise() first!",getName()); }

//****** acquire read lock.... *******
  smileMutexLock(RWstatMtx);
  // check for urgent write request:
  while (writeReqFlag) { // wait until write request has been served!
    smileMutexUnlock(RWstatMtx);
//    usleep(1000);
    smileYield();
    smileMutexLock(RWstatMtx);
  }
  if (nCurRdr == 0) {
    nCurRdr++;
    smileMutexUnlock(RWstatMtx);
    smileMutexLock(RWmtx); // no other readers, so lock mutex to exclude writes...
    smileMutexLock(RWstatMtx);
  } else {
    nCurRdr++;
  }
//      nCurRdr++;
  smileMutexUnlock(RWstatMtx);
//****************

  smileMutexLock(RWptrMtx);
  long rIdx = validateIdxR(&vIdx,special,rdId);
  smileMutexUnlock(RWptrMtx);

  cVector *vec=NULL;
  if (rIdx>=0) {
    vec = new cVector(lcfg.N,lcfg.type);
    if (vec == NULL) OUT_OF_MEMORY;
    if (lcfg.type == DMEM_FLOAT) frameRd(rIdx, vec->dataF);
    else if (lcfg.type == DMEM_INT) frameRd(rIdx, vec->dataI);
    getTimeMeta(rIdx,vec->tmeta);
    vec->fmeta = &(fmeta);
    if (result!=NULL) *result=DMRES_OK;
  } else {
    SMILE_DBG(4,"getFrame: frame index (vIdx %i -> rIdx %i) out of range, frame cannot be read (level '%s')!",vIdx,rIdx,getName());
    if (result!=NULL) {
      if (rIdx == -2) *result=DMRES_OORleft|DMRES_ERR;
      else if (rIdx == -3) *result=DMRES_OORright|DMRES_ERR;
      else if (rIdx == -4) *result=DMRES_OORbs|DMRES_ERR;
      else *result=DMRES_ERR;
    }
  }

  //**** now unlock ******
  smileMutexLock(RWstatMtx);
  nCurRdr--;
  if (nCurRdr < 0) { // ERROR!!
    SMILE_ERR(1,"nCurRdr < 0  while unlocking dataMemory!! This is a BUG!!!");
    nCurRdr = 0;
  }
  if (nCurRdr==0) smileMutexUnlock(RWmtx);
  smileMutexUnlock(RWstatMtx);
  //********************

  return vec;
}

//TODO: add an optimized 'simple' level for high performance and low overhead wave handling
//  no tmeta, very simple access functions etc.
//  tmeta if accessed will be emultated?
cMatrix * cDataMemoryLevel::getMatrix(long vIdx, long vIdxEnd, int special, int rdId, int *result)
{
  if (!lcfg.finalised) { COMP_ERR("cannot get matrix from non-finalised level! call finalise() first!"); }

  long vIdxold=vIdx;
//  long vIdxEndO = vIdxEnd;
  if (vIdx < 0) vIdx = 0;
  int padEnd = 0; // will be filled with the number of samples at the end of the matrix to be padded

//****** acquire read lock.... *******
  smileMutexLock(RWstatMtx);
  // check for urgent write request:
  while (writeReqFlag) { // wait until write request has been served!
    smileMutexUnlock(RWstatMtx);
    smileYield();
    smileMutexLock(RWstatMtx);
  }
  if (nCurRdr == 0) {
    nCurRdr++;
    smileMutexUnlock(RWstatMtx);
    smileMutexLock(RWmtx); // no other readers, so lock mutex to exclude writes...
    smileMutexLock(RWstatMtx);
  } else {
    nCurRdr++;
  }
  smileMutexUnlock(RWstatMtx);
//****************

  smileMutexLock(RWptrMtx);
  long rIdx = validateIdxRangeR(vIdxold, &vIdx, vIdxEnd, special, rdId, 0, &padEnd);  // TODO : if EOI state, then allow vIdxEnd out of range! pad frame...
  smileMutexUnlock(RWptrMtx);

  cMatrix *mat=NULL;
  if (rIdx>=0) {
    if (vIdxold < 0) mat = new cMatrix(lcfg.N,vIdxEnd-vIdxold,lcfg.type);
    else mat = new cMatrix(lcfg.N,vIdxEnd-vIdx,lcfg.type);
    SMILE_DBG(4,"creating new data matrix (%s)  vIdxold=%i , vIdx=%i, vIdxEnd=%i, lcfg.N=%i",this->getName(),vIdxold,vIdx,vIdxEnd,lcfg.N)
    if (mat == NULL) OUT_OF_MEMORY;
    long i,j;
    if (vIdxold < 0) {
      long i0 = 0-vIdxold;
      if (lcfg.type == DMEM_FLOAT) {
        for (i=0; i<i0; i++) {
          if (special == DMEM_PAD_ZERO) for (j=0; j<mat->N; j++) mat->dataF[i*lcfg.N+j] = 0.0; // pad with value
          else frameRd((rIdx)%lcfg.nT, mat->dataF + (i*lcfg.N)); // fill with first frame
          getTimeMeta((rIdx)%lcfg.nT,mat->tmeta + i);
        }
        for (i=0; i<vIdxEnd; i++) { frameRd((rIdx+i)%lcfg.nT, mat->dataF + (i+i0)*lcfg.N); getTimeMeta((rIdx+i)%lcfg.nT,mat->tmeta + i +i0); }
      } else if (lcfg.type == DMEM_INT) {
        for (i=0; i<i0; i++) {
          if (special == DMEM_PAD_ZERO) for (j=0; j<mat->N; j++) mat->dataI[i*lcfg.N+j] = 0; // pad with value
          else frameRd((rIdx)%lcfg.nT, mat->dataI + (i*lcfg.N)); // fill with first frame
          getTimeMeta((rIdx)%lcfg.nT,mat->tmeta + i);
        }
        for (i=0; i<vIdxEnd; i++) { frameRd((rIdx+i)%lcfg.nT, mat->dataI + (i+i0)*lcfg.N); getTimeMeta((rIdx+i)%lcfg.nT,mat->tmeta + i +i0); }
      }
    } else if (padEnd>0) {
      if (lcfg.type == DMEM_FLOAT) {
        for (i=0; i<(vIdxEnd-vIdx)-padEnd; i++) { frameRd((rIdx+i)%lcfg.nT, mat->dataF + (i*lcfg.N)); getTimeMeta((rIdx+i)%lcfg.nT,mat->tmeta + i); }
        long i0 = i-1;
        for (; i<(vIdxEnd-vIdx); i++) {
          if (special == DMEM_PAD_ZERO) for (j=0; j<mat->N; j++) mat->dataF[i*lcfg.N+j] = 0.0; // pad with value
          else frameRd((rIdx+i0)%lcfg.nT, mat->dataF + (i*lcfg.N)); // fill with last frame
          getTimeMeta((rIdx+i0)%lcfg.nT,mat->tmeta + i);
        }
        // TODO: Test DMEM_PAD_NONE option to truncate the frame!!
        if (special == DMEM_PAD_NONE) {
          mat->nT = (vIdxEnd-vIdx)-padEnd;
        }
      } else if (lcfg.type == DMEM_INT) {
        for (i=0; i<(vIdxEnd-vIdx)-padEnd; i++) { frameRd((rIdx+i)%lcfg.nT, mat->dataI + (i*lcfg.N)); getTimeMeta((rIdx+i)%lcfg.nT,mat->tmeta + i); }
        long i0 = i-1;
        for (; i<(vIdxEnd-vIdx); i++) {
          if (special == DMEM_PAD_ZERO) for (j=0; j<mat->N; j++) mat->dataI[i*lcfg.N+j] = 0; // pad with value
          else frameRd((rIdx+i0)%lcfg.nT, mat->dataI + (i*lcfg.N)); // fill with last frame
          getTimeMeta((rIdx+i0)%lcfg.nT,mat->tmeta + i);
        }
        // TODO: Test DMEM_PAD_NONE option to truncate the frame!!
        if (special == DMEM_PAD_NONE) {
          mat->nT = (vIdxEnd-vIdx)-padEnd;
        }
      }

    } else {
      if (lcfg.type == DMEM_FLOAT) {
        for (i=0; i<mat->nT; i++) {
          frameRd((rIdx+i)%lcfg.nT, mat->dataF + i*lcfg.N);
          getTimeMeta((rIdx+i)%lcfg.nT,mat->tmeta + i);
        }
      }
      else if (lcfg.type == DMEM_INT) {
        for (i=0; i<mat->nT; i++) {
          frameRd((rIdx+i)%lcfg.nT, mat->dataI + i*lcfg.N);
          getTimeMeta((rIdx+i)%lcfg.nT,mat->tmeta + i);
        }
      }
    }
    mat->fmeta = &(fmeta);
  } else {
    SMILE_DBG(4,"ERROR, getMatrix: frame index range (vIdxStart %i - vIdxEnd %i  => rIdxStart %i) out of range, matrix cannot be read (level '%s')!",vIdx,vIdxEnd,rIdx,getName());
  }
  
  //**** now unlock ******
  smileMutexLock(RWstatMtx);
  nCurRdr--;
  if (nCurRdr < 0) { // ERROR!!
    SMILE_ERR(1,"nCurRdr < 0  while unlocking dataMemory!! This is a BUG!!!");
    nCurRdr = 0;
  }
  if (nCurRdr==0) smileMutexUnlock(RWmtx);
  smileMutexUnlock(RWstatMtx);
  //********************
  
  return mat;
}

// methods to get info about current level fill status (e.g. number of frames written, curW, curR(global) and freeSpace, etc.)
long cDataMemoryLevel::getMaxR() 
{ 
  smileMutexLock(RWptrMtx);
  long res = curW-1;
  smileMutexUnlock(RWptrMtx);
  return res;
}

long cDataMemoryLevel::getMinR() {  // minimum readable index (relevant only for ringbuffers, otherwise it will always return 0)
  long res=0;
  if (lcfg.isRb) {
    smileMutexLock(RWptrMtx);
    if (curW > lcfg.nT)
      res = curW-lcfg.nT;
    smileMutexUnlock(RWptrMtx);
  }
  return res;
}

// functions to convert absolute times (in seconds) to level frame indicies!
long cDataMemoryLevel::secToVidx(double sec) { // returns a vIdx
   //(for periodic levels this function is easy, however for aperiodic levels we must iterate through all frames... maybe also for periodic to compensate round-off errors?)
  if (lcfg.T!=0.0) {
    return (long)round(sec/lcfg.T);
  } else {
    SMILE_WRN(0,"cDataMemoryLevel::secToVidx: NOT YET IMPLEMENTED for variable period levels!");
  }
  return 0;
}

double cDataMemoryLevel::vIdxToSec(long vIdx) { // returns a vIdx
   //(for periodic levels this function is easy, however for aperiodic levels we must iterate through all frames... maybe also for periodic to compensate round-off errors?)
  if (lcfg.T!=0.0) {
    return ((double)(vIdx))*lcfg.T;
  } else {
    SMILE_WRN(0,"cDataMemoryLevel::vIdxToSec: NOT YET IMPLEMENTED for variable period levels!");
  }
  return 0;
}

/***************************** dataMemory ***********************************************/


void cDataMemory::_addLevel()
{
  nLevels++;
  if (nLevels>=nLevelsAlloc) {
    int lN = nLevels+LOOKAHEAD_ALLOC;
    cDataMemoryLevel **l = (cDataMemoryLevel **)realloc(level,sizeof(cDataMemoryLevel *)*lN);
    if (l==NULL) OUT_OF_MEMORY;
    // initialize newly allocated memory with NULL
    int i;
    for (i=nLevels; i<lN; i++) l[i] = NULL;
    level = l;
    nLevelsAlloc=lN;
  }
}


int cDataMemory::registerLevel(cDataMemoryLevel *l)
{
  if (l!=NULL) {
    _addLevel();
    level[nLevels] = l;
    return nLevels;
  } else { SMILE_WRN(1,"attempt to register NULL level with dataMemory!"); return -1; }
}

// get element id of level 'name', return -1 on failure (e.g. level not found)
int cDataMemory::findLevel(const char *name)
{
  int i;
  if (name==NULL) return -1;
  
  for (i=0; i<=nLevels; i++) {
    if(!strcmp(name,level[i]->getName())) return i;
  }
  return -1;
}


// get element id of level 'name', return -1 on failure (e.g. level not found)
// finalise the level, if found
int cDataMemory::finaliseLevel(const char *name)
{
  int i = findLevel(name);
  if (i>=0) {
    level[i]->finaliseLevel();
  }
  return i;
}

// finalise level n
int cDataMemory::finaliseLevel(int n)
{
  if ((n>=0)&&(n<=nLevels)) {
    level[n]->finaliseLevel();
    return 1;
  }
  return 0;
}


void cDataMemory::registerReadRequest(const char *lvl, const char *componentInstName)
{
  if (lvl == NULL) return;

  // find existing request
  int idx = rrq.findRequest(lvl, componentInstName);
  // do nothing if this request was already registered!
  if (idx) return;

  // if lvl has not yet been registered, get pointer to next empty element in list:
  sDmLevelRWRequest *rq = rrq.getElement();
  if (rq!=NULL) {
    rq->instanceName = componentInstName;
    rq->levelName = lvl;
  }

  SMILE_DBG(2,"registerReadRequest: registered read request for level '%s' by component instance '%s'",lvl,componentInstName);
}

void cDataMemory::registerWriteRequest(const char *lvl, const char *componentInstName)
{
  if (lvl == NULL) return;

  // find existing request
  int idx = wrq.findRequest(lvl, NULL);
  // check if the same component attempts to register twice (which is ok), or if two components want to write to the same level!!
  if (idx) {
    sDmLevelRWRequest *rq = wrq.getElement(idx-1);
    if ((rq!=NULL)&&(!strcmp(rq->instanceName,componentInstName))) {
      return;  // ignore duplicate request from the same component
    } else {
      // it's likely another component wanting to write to the same level...
      COMP_ERR("two components cannot write to the same level: '%s', component1='%s', component2='%s'",lvl,rq->instanceName,componentInstName);
    }
  }

  // if lvl has not yet been registered, get pointer to next empty element in list:
  sDmLevelRWRequest *rq = wrq.getElement();
  if (rq!=NULL) {
    rq->instanceName = componentInstName;
    rq->levelName = lvl;
  }

  SMILE_DBG(2,"registerWriteRequest: registered write request for level '%s' by component instance '%s'",lvl,componentInstName);
}

/* add a new level with given _lcfg parameters */
int cDataMemory::addLevel(sDmLevelConfig *_lcfg, const char *_name)
{
  if (_lcfg == NULL) return 0;

  if (_name != NULL) {
    _lcfg->setName(_name);
  }

  cDataMemoryLevel *l = new cDataMemoryLevel(-1, *_lcfg );
  if (l==NULL) OUT_OF_MEMORY;
  l->setParent( (cDataMemory*)this );
  return registerLevel(l);
}

/*
int cDataMemory::registerWriteRequest(const char *lvl, const char *componentInstName)
{
  int ret=-1;
  if (lvl == NULL) return ret;
  int i; int ex=-1;
  // find existing:
  for (i=0; i<nRRq; i++) {
    if (!strcmp(lvl, writeReq[i].name)) {
      ex = i;
      SMILE_ERR(2,"multiple write requests for level '%s' registered! only one writer should write to each level!",lvl);
      return ret;
    }
  }
  if (nWRq >= nWRqAlloc) {
    sDmLevelConfig *tmp = (sDmLevelConfig *)crealloc(writeReq, sizeof(sDmLevelConfig)*(nWRq+LOOKAHEAD_ALLOC), sizeof(sDmLevelConfig)*nWRqAlloc);
    if (tmp==NULL) OUT_OF_MEMORY;
    writeReq = tmp;
    nWRqAlloc = (nWRq+LOOKAHEAD_ALLOC);
  }

    memcpy(writeReq+nWRq, &cfg, sizeof(sDmLevelConfig));
    writeReq[nWRq].name = strdup(lvl);
    SMILE_DBG(2,"registerWriteRequest: registered write request for level '%s' (rq-idx %i)",lvl,nWRq);
    // create level:
    cDataMemoryLevel *l = new cDataMemoryLevel(-1, writeReq[nWRq] );
*/

/*
    if ((writeReq[nWRq].T != 0.0)&&(writeReq[nWRq].lenSec > 0.0)) {
      l = new cDataMemoryLevel(-1, writeReq[nWRq].name, writeReq[nWRq].lenSec, writeReq[nWRq].T,
                                   writeReq[nWRq].isRb, writeReq[nWRq].growDyn, writeReq[nWRq].type );
    } else {


      l = new cDataMemoryLevel(-1, writeReq[nWRq].name, writeReq[nWRq].nT,
                                   writeReq[nWRq].isRb, writeReq[nWRq].growDyn, writeReq[nWRq].type );
    }
    */
/*
    if (l==NULL) OUT_OF_MEMORY;
    l->setParent( (cDataMemory*)this );
    ret = registerLevel(l);
    SMILE_DBG(4,"registerWriteRequest: created level '%s' and registered object with dataMemory object '%s'",lvl,getInstName());
    nWRq++;
  //}
  return ret;
}
*/


int cDataMemory::myRegisterInstance(int *runMe)
{
  int i;

  // check if a write request exists for each level where there exists a read request
  int err=0;
  for (i=0; i<rrq.getNEl(); i++) {
    sDmLevelRWRequest * x = rrq.getElement(i);
    if (x!=NULL) {
      int idx = wrq.findRequest(x->levelName, NULL);
      if (!idx) {
        SMILE_ERR(1,"level '%s' was not found! component '%s' requires it for reading.\n     it seems that no dataWriter has registered this level! check your config!",x->levelName,x->instanceName);
        err=1;
      }
    }
  }
  if (err) { COMP_ERR("there were unresolved read-requests, please check your config file for missing components /  missing dataWriters or incorrect level names (typos, etc.) !"); }

  if (runMe != NULL) *runMe = 0;

  return !err;
}

int cDataMemory::myConfigureInstance() 
{
  // check blocksizeconfig and update buffersizes of levels accordingly
  if (nLevels>=0) {
    int i;
    for (i=0; i<=nLevels; i++) {
      // do this for each level...
      SMILE_IDBG(3,"configuring level %i ('%s') (checking buffersize and config)",i,level[i]->getName());
      int ret = level[i]->configureLevel();
      if (!ret) {
        SMILE_IERR(1,"level '%s' could not be configured!");
        return 0;
      }
    }
  } else {
    SMILE_ERR(1,"it makes no sense to configure a dataMemory without levels! cannot configure dataMemory '%s'!",getInstName());
    return 0;
  }

  return 1;
}

int cDataMemory::myFinaliseInstance()
{
  // now finalise the levels (allocate storage memory and finalise config):
  if (nLevels>=0) {
    int i;
    for (i=0; i<=nLevels; i++) {
      // actually finalise now
      SMILE_DBG(3,"finalising level %i (allocating buffer, etc.)",i);
      int ret = level[i]->finaliseLevel();
      if (!ret) {
        SMILE_IERR(1,"level '%s' could not be finalised!");
        return 0;
      }
    }
    // allocate reader config array
    SMILE_DBG(4,"allocating reader positions in %i level(s)",nLevels+1);
    for (i=0; i<=nLevels; i++) {
      level[i]->allocReaders();
    }
  } else {
    SMILE_ERR(1,"it makes no sense to finalise a dataMemory without levels! cannot finalise dataMemory '%s'!",getInstName());
    return 0;
  }
  return 1;
}

cDataMemory::~cDataMemory() {
  int i;
  if (level != NULL) {
    for (i=0; i<nLevelsAlloc; i++) {
      if(level[i] != NULL) delete level[i];
    }
    free(level);
  }
}

/****************** data memory logger functions ***************************/

/*
These functions can be used for debugging the data flow. they log every read, write, and check operation, thus producing a lot of data.

Log message fields:
-level
(-component?)
-read/write/check
-write/read/check counter
-vIdx
(-rIdx?)
-length
-curR
-curW
-nT
-DATA
*/



void datamemoryLogger(const char *name, const char*dir, long cnt, long vIdx0, long vIdx, long rIdx, long nT, int special, long curR, long curW, long EOI, int nReaders, long *curRr, cVector *vec)
{
#ifdef DM_DEBUG_LOGGER
  long i;
  fprintf(stderr,"xxDMemLOGxx:: level:%s dir:%s cnt:%i vIdx0:%i vIdx:%i rIdx:%i nT:%i special:%i EOI:%i curR:%i curW:%i :: nReaders:%i ",name,dir,cnt,vIdx0,vIdx,rIdx,nT,special,EOI,curR,curW,nReaders);
  for (i=0; i<nReaders; i++) {
    fprintf(stderr,"curRr%i:%i ",i,curRr[i]);
  }
  fprintf(stderr,"::: DATA(%i) ::: ",vec->N);
  for (i=0; i<vec->N; i++) {
    fprintf(stderr,"f%i:%i ",i,vec->dataF[i]);
  }
  fprintf(stderr,"::END\n");
#endif
}

void datamemoryLogger(const char *name, long vIdx0, long vIdx, long rIdx, long nT, int special, long curR, long curW, long EOI, int nReaders, long *curRr, cMatrix *mat)
{
#ifdef DM_DEBUG_LOGGER
  long i,j;
  fprintf(stderr,"xxDMemLOGxx:: level:%s dir:%s cnt:%i vIdx0:%i vIdx:%i rIdx:%i nT:%i special:%i EOI:%i curR:%i curW:%i :: nReaders:%i ",name,dir,cnt,vIdx0,vIdx,rIdx,nT,special,EOI,curR,curW,nReaders);
  for (i=0; i<nReaders; i++) {
    fprintf(stderr,"curRr%i:%i ",i,curRr[i]);
  }
  fprintf(stderr,"::: DATA[%ix%i] ::: ",vec->N,vec->nT); // row x column
  for (j=0; j<vec->nT; j++) {
    for (i=0; i<vec->N; i++) {
      fprintf(stderr,"f%i,%i:%i ",i,j,vec->dataF[i+j*vec->N]);  // row, column
    }
    if (j<vec->nT-1) fprintf(stderr,":: ");
  }
  fprintf(stderr,"::END\n");
#endif
}







