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


#ifndef __DATA_MEMORY_HPP
#define __DATA_MEMORY_HPP

#include <core/smileCommon.hpp>
#include <core/smileComponent.hpp>
#include <math.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

// temporal frame ID
#define DMEM_IDX_ABS    -1   // no special index
#define DMEM_IDX_CURR   -11   // current read index
#define DMEM_IDX_CURW   -12   // current write index
#define DMEM_IDXR_CURR  -13   // start is relative to current read index
#define DMEM_IDXR_CURW  -14   // start is relative to current write index
#define DMEM_PAD_ZERO   -101   // pad with zeros
#define DMEM_PAD_FIRST  -102   // pad with first/last available frame (default)
#define DMEM_PAD_NONE   -103   // truncate frame (matrix) to actual available length

/*
#define FLOAT_DMEM_FLOAT  0
#define FLOAT_DMEM_DOUBLE  1

// this defines the float type used throughout the data memory, either 'float' or 'double'
#define FLOAT_DMEM  float
#define FLOAT_DMEM_NUM  FLOAT_DMEM_FLOAT   // this numeric constant MUST equal the float type set above ...
                                           // 0 = float, 1 = double:
// this defines the int type used throughout the data memory, either 'short', 'int' or 'long'
#define INT_DMEM    int
*/

#define DMEM_FLOAT   0
#define DMEM_INT     1

#define LOOKAHEAD_ALLOC  20

/***** frame field dataType definitions *********/

#define DATATYPE_UNKNOWN  0
#define DATATYPE_SIGNAL   0x0100    // raw signal (pcm, etc.)
#define DATATYPE_SIGNAL_PCM  0x0101
#define DATATYPE_SIGNAL_IMG  0x0102
#define DATATYPE_SPECTRUM  0x0200
#define DATATYPE_SPECTRUM_BINS_FIRST    0x201
#define DATATYPE_SPECTRUM_BINS_COMPLEX  0x0201
#define DATATYPE_SPECTRUM_BINS_MAG  0x0202   /* magnitude spectrum */
#define DATATYPE_SPECTRUM_BINS_MAGPHASE  0x0242   /* magnitude spectrum and phase in one array (mag first half, phase second half) */
#define DATATYPE_SPECTRUM_BINS_PHASE  0x0203 /* spectral phase */
#define DATATYPE_SPECTRUM_BINS_DBPSD  0x0204 /* power spectral density in dB SPL */
#define DATATYPE_SPECTRUM_BINS_POWSPECDENS  0x0205   /* power spectral density */
#define DATATYPE_SPECTRUM_BINS_POWSPEC  0x0206 /* power spectrum */
#define DATATYPE_SPECTRUM_BINS_SPECDENS  0x0207 /* magnitude spectral density */
#define DATATYPE_SPECTRUM_BINS_LAST    0x207
#define DATATYPE_SPECTRUM_BANDS_MAG  0x0220
#define DATATYPE_COEFFICIENTS  0x0300
#define DATATYPE_ACF        0x0380
#define DATATYPE_CEPSTRAL  0x0400
#define DATATYPE_MIXED_LLD  0x1000  // mixed low-level descriptors (e.g. pitch, etc.)
#define DATATYPE_FUNCTIONALS  0x2000  // functionals of lld, etc.


/**** frame meta information *********
   names, indicies (for each row)
 **************************************/

#undef class
// meta information for one field...
class DLLEXPORT FieldMetaInfo {
  public:

  char *name;
  int Nstart;  // start index (position in FrameMetaInfo
  int N;  //  number of elements (>1 ==> array)
  int dataType; // dataType flag indicating type of data in this field (NEW feature, see dataType definitions in dataMemory.hpp)
  long infoSize; // size of info struct in BYTES!
  int infoSet;  // flag that indicates whether the info has been set or not (dataType, *info, infoSize)
  void *info;   // custom info data (e.g. spectral scaling for spectra) 
  int arrNameOffset;

  FieldMetaInfo() : name(NULL), info(NULL), infoSize(0), dataType(0), N(0),arrNameOffset(0) {}
  void copyFrom(FieldMetaInfo *f) {
    if (f!=NULL) {
      Nstart=f->Nstart;
      N = f->N;
      arrNameOffset = f->arrNameOffset;
      dataType = f->dataType;
      infoSize = f->infoSize;
      if (info != NULL) free(info);
      info = malloc(infoSize);
      memcpy(info, f->info, infoSize);
      if (name != NULL) free(name);
      if (f->name != NULL) name = strdup(f->name);
      else name = NULL;
    }
  }
  ~FieldMetaInfo() { 
    if (name!=NULL) free(name); 
    if (info!=NULL) free(info);
  }
};

class DLLEXPORT cVectorMeta { public:
  int ID; /* ID identifying the meta data type */
  int iData[8]; /* freely usable ints */
  FLOAT_DMEM fData[8]; /* freely usable floats */
  char * text; /* custom meta-data text, will be auto-freed using free() if not NULL */
  void * custom; /* custom meta data, will be auto-freed using free() if not NULL */
  long customLength; /* length of data in custom (in bytes!), this is required for cloning a cVectorMeta data object! Set this to <= 0 to only copy the pointer and not free the pointer when the cVectorMeta object is freeed!*/
  cVectorMeta() : ID(0), text(NULL), custom(NULL), customLength(0)
  {
    iData[0] = iData[1] = iData[2] = iData[3] = 0;
    iData[4] = iData[5] = iData[6] = iData[7] = 0;
    fData[0] = fData[1] = fData[2] = fData[3] = 0.0;
    fData[4] = fData[5] = fData[6] = fData[7] = 0.0;
  }

  void clear() {
    ID = 0;
    if (text != NULL) {
      free(text);
      text = NULL;
    }
    if (custom != NULL) {
      free(custom);
      custom = NULL;
    }
    customLength = 0;
    iData[0] = iData[1] = iData[2] = iData[3] = 0;
    iData[4] = iData[5] = iData[6] = iData[7] = 0;
    fData[0] = fData[1] = fData[2] = fData[3] = 0.0;
    fData[4] = fData[5] = fData[6] = fData[7] = 0.0;
  }

  void cloneFrom(const cVectorMeta *v)
  {
    if (v!=NULL) {
      int i;
      ID = v->ID;
      for (i=0; i<8; i++) iData[i] = v->iData[i];
      for (i=0; i<8; i++) fData[i] = v->fData[i];
      if (v->text != NULL) {
        if (text != NULL) free(text);
        text = strdup(v->text);
      }
      if ((v->customLength > 0)&&(v->custom != NULL)) {
        if ((custom != NULL)&&(customLength>0)) free(custom);
        custom = malloc(v->customLength);
        memcpy(custom, v->custom, v->customLength);
        customLength = v->customLength;
      }
    }
  }
  ~cVectorMeta() { 
    if (text != NULL) free(text); 
    if ((customLength>0)&&(custom != NULL)) free(custom); 
  }
};


// meta information for a complete vector 
class DLLEXPORT FrameMetaInfo {
  public:
    smileMutex myMtx;
    long N, Ne;   // number of fields..
    FieldMetaInfo *field;
    
    // global metadata for the whole level (use tmeta->metadata for per frame meta data)
    cVectorMeta metaData;

    FrameMetaInfo() : N(0), Ne(0), field(NULL) 
    {
      smileMutexCreate(myMtx);
    }
    
    /* find a field by its full name (optionally including array index */
    // returned *arrIdx will be the real index in the data structure, i.e. named index - arrNameOffset
    // if more != NULL, then *more (if > 0) will be used as a start field index for the search (i>=*more)
    //                  and *more will be filled with the number of additional matches found (0 if the returned field is the only match)
    int findField(const char*_fieldName, int *arrIdx=NULL, int *more=NULL) const;
    /* find a field by a part of its name (optionally including array index) */
    // returned *arrIdx will be the real index in the data structure, i.e. named index - arrNameOffset
    // NOTE: if multiple names contain the same part, only the first one found will be returned! Solution: use the 'more' parameter:
    //   if more != NULL, then *more (if > 0) will be used as a start field index for the search (i>=*more)
    //                    and *more will be filled with the number of additional matches found (0 if the returned field is the only match)
    int findFieldByPartialName(const char*_fieldNamePart, int *arrIdx=NULL, int *more=NULL) const;

    long fieldToElementIdx(long _field, long _arrIdx=0) const;
    long elementToFieldIdx(long _element, long *_arrIdx=NULL) const;

    void printFieldNames() const {
      SMILE_PRINT("  Field name & dimension:");
      for (int i = 0; i < N; i++) {
        SMILE_PRINT("    %s %i", field[i].name, field[i].N);
      }
    }

    const char *getName(int n, int *arrIdx=NULL); // n is element(!) index
    ~FrameMetaInfo() {
      if (field != NULL) {
        int i;
        for (i=0; i<N; i++) { 
          if (field[i].name != NULL) free(field[i].name); 
          if (field[i].info!=NULL) free(field[i].info);
        }
        free(field);
      }
      smileMutexDestroy(myMtx);
    }
};

/**** temporal meta information *********
   length, times, etc. (for each column)
 **************************************/

class DLLEXPORT TimeMetaInfo;

// used for non-overlapping frames, at fixed rate
// i.e. for audio samples
class DLLEXPORT TimeMetaInfoMinimal { public:
  long vIdx;     // index of this frame in data memory level ?
  float period;  // frame period of this level
//  double time;    // real start time in seconds  = 0
//  double lengthSec; // real length in seconds  = 1/T
//  double vLengthSec;  // actual length in seconds (e.g. zero padded fft)   = 1/T
//  long lengthFrames;  // real length in frames  = 1
//  long vLengthFrames;  // actual length in frames  (e.g. zero padded fft)  = 1
//  double framePeriod;      // frame period of previous level  = T
//  long lengthSamples;  // real length in samples (samplerate = frame rate of top most level)  = 1
//  long vLengthSamples;  // actual length in samples (samplerate =  frame rate of top most level)  = 1
//  double samplePeriod;      // sampleperiod = 1/samplerate (top most level)  =  T
//  double smileTime;    // smile time at which the frame (or a parent) was initially written to the data memory by a source (time measured in seconds since creation of component manager object)
//  int noAutoSmileTime; // if this is set to 1, smile time will not be updated (unless it is -1.0) when this frame is written to the datamemory (this can be used by components (esp. sources) in offline mode, to set valid timestamps)
//  cVectorMeta metadata; // custom meta-data for each vector/matrix
  TimeMetaInfoMinimal() : vIdx(0), period(0.0) {}

  void cloneFrom(const TimeMetaInfoMinimal *tm) {
    if (tm != NULL) {
      vIdx = tm->vIdx;
      period = tm->period;
    }
  }
  void cloneFrom(const TimeMetaInfo *tm);
};

class DLLEXPORT TimeMetaInfo { public:
  int filled;    // whether info in this struct was already completed by setTimeMeta or the calling code
  int level;    // originating level idx
  long vIdx;     // index of this frame in data memory level ?
  double period; // frame period of this level
  double time;    // real start time in seconds
  double lengthSec; // real length in seconds
  double vLengthSec;  // actual length in seconds (e.g. zero padded fft)
  long lengthFrames;  // real length in frames
  long vLengthFrames;  // actual length in frames  (e.g. zero padded fft)
  double framePeriod;      // frame period of previous level
  long lengthSamples;  // real length in samples (samplerate = frame rate of top most level)
  long vLengthSamples;  // actual length in samples (samplerate =  frame rate of top most level)
  double samplePeriod;      // sampleperiod = 1/samplerate (top most level)
  double smileTime;    // smile time at which the frame (or a parent) was initially written to the data memory by a source (time measured in seconds since creation of component manager object)
  int noAutoSmileTime; // if this is set to 1, smile time will not be updated (unless it is -1.0) when this frame is written to the datamemory (this can be used by components (esp. sources) in offline mode, to set valid timestamps)
  cVectorMeta metadata; // custom meta-data for each vector/matrix
  TimeMetaInfo() :
    filled(0), level(0), vIdx(0), period(0.0), time(0.0),
    lengthSec(0.0), vLengthSec(0.0), lengthFrames(0),
    vLengthFrames(0), framePeriod(0.0), lengthSamples(0),
    vLengthSamples(0), samplePeriod(0.0), smileTime(-1.0), noAutoSmileTime(0)
  {}

  void cloneFrom(const TimeMetaInfo *tm) {
    if (tm!=NULL) {
      filled = tm->filled;
      level = tm->level;
      vIdx = tm->vIdx;
      period = tm->period;
      time = tm->time;
      lengthSec = tm->lengthSec;
      vLengthSec = tm->vLengthSec;
      lengthFrames = tm->lengthFrames;
      vLengthFrames = tm->vLengthFrames;
      framePeriod = tm->framePeriod;
      lengthSamples = tm->lengthSamples;
      vLengthSamples = tm->vLengthSamples;
      samplePeriod = tm->samplePeriod;
      smileTime = tm->smileTime;
      metadata.cloneFrom(&(tm->metadata));
    }
  }

  void cloneFrom(const TimeMetaInfoMinimal *tm, int originating_level = -1) {
    if (tm!=NULL) {
      filled = 1;
      level = originating_level;
      vIdx = tm->vIdx;
      period = tm->period;
      time = (double)tm->vIdx * (double)tm->period;
      lengthSec = 1.0 / (double)period;
      vLengthSec = lengthSec;
      lengthFrames = 1;
      vLengthFrames = 1;
      framePeriod = period;
      lengthSamples = 1;
      vLengthSamples = 1;
      samplePeriod = period;
      smileTime = time;
      metadata.clear();
    }
  }
};



/**** dataMemory datatypes *********
 **************************************/

class DLLEXPORT cVector { public:
  // frame data + meta information
  long N;  // number of elements (?)
  int type;
  int tmetaArr; /* 1=tmeta is array, 0=tmeta is single object */
  TimeMetaInfo *tmeta; // usually tmeta is allocated individually for each vector
  TimeMetaInfoMinimal *tmeta_minimal; // usually tmeta is allocated individually for each vector
  double tmeta_minimal_period; // usually tmeta is allocated individually for each vector
  int tmetaAlien; // .. except if tmetaAlien is set, then tmeta is a pointer to a struct allocated elsewhere
  FrameMetaInfo *fmeta; // fmeta is always a pointer to memory in the dataMemory
  FLOAT_DMEM *dataF;
  INT_DMEM *dataI;
  char *ntmp; // temporary name for name(n) function
  // (obsolete: ? TODO:  add name of origin level to his vector in order to build unique field names!)
  
  // TODO: add clear() method!
  
  cVector(int lN, int ltype=DMEM_FLOAT);
  // get name of expanded element n
  const char *name(int n, int *lN=NULL);
  // TODO: getval/setval functions for Int and Float
  FLOAT_DMEM getF(int n) { return dataF[n]; } // WARNING: index n is not checked!
  INT_DMEM getI(int n) { return dataI[n]; } // WARNING: index n is not checked!

  void tmetaReplace(TimeMetaInfo *xtmeta) {
    if ((tmeta != NULL)&&(!tmetaAlien)) {
      if (tmetaArr) delete[] tmeta;
      else delete tmeta;
    }
    tmetaAlien = 1;
    tmetaArr = 0;
    tmeta = xtmeta;
  }
  void tmetaReplace(TimeMetaInfoMinimal *xtmeta, double period = 0.0) {
    if ((tmeta_minimal != NULL)&&(!tmetaAlien)) {
      if (tmetaArr) delete[] tmeta_minimal;
      else delete tmeta_minimal;
    }
    tmetaAlien = 1;
    tmetaArr = 0;
    tmeta_minimal = xtmeta;
    tmeta_minimal_period = period;
  }
  void tmetaReplaceFromMatrix(TimeMetaInfo *xtmeta) {
    tmetaReplace(xtmeta);
    tmetaArr = 1;
  }
  void tmetaReplaceFromMatrix(TimeMetaInfoMinimal *xtmeta, double period = 0.0) {
    tmetaReplace(xtmeta);
    tmetaArr = 1;
    tmeta_minimal_period = period;
  }

  void tmetaClone(TimeMetaInfo *xtmeta) {
    if ((tmeta != NULL)&&(!tmetaAlien)) {
      if (tmetaArr) delete[] tmeta;
      else delete tmeta;
    }
    tmeta = new TimeMetaInfo();
    tmeta->cloneFrom(xtmeta);
    tmetaAlien = 0;
    tmetaArr=0;
  }
  void tmetaClone(TimeMetaInfoMinimal *xtmeta) {
    if ((tmeta_minimal != NULL)&&(!tmetaAlien)) {
      if (tmetaArr) delete[] tmeta_minimal;
      else delete tmeta_minimal;
    }
    tmeta_minimal = new TimeMetaInfoMinimal();
    tmeta_minimal->cloneFrom(xtmeta);
    tmetaAlien = 0;
    tmetaArr = 0;
  }
  void tmetaCloneToFull(TimeMetaInfoMinimal *xtmeta) {
    if ((tmeta_minimal != NULL)&&(!tmetaAlien)) {
      if (tmetaArr) delete[] tmeta_minimal;
      else delete tmeta_minimal;
    }
    if ((tmeta != NULL)&&(!tmetaAlien)) {
      if (tmetaArr) delete[] tmeta;
      else delete tmeta;
    }
    tmeta = new TimeMetaInfo();
    tmeta->cloneFrom(xtmeta);
    tmetaAlien = 0;
    tmetaArr = 0;
  }
  void tmetaCloneToMinimal(TimeMetaInfo *xtmeta) {
    if ((tmeta_minimal != NULL)&&(!tmetaAlien)) {
      if (tmetaArr) delete[] tmeta_minimal;
      else delete tmeta_minimal;
    }
    if ((tmeta != NULL)&&(!tmetaAlien)) {
      if (tmetaArr) delete[] tmeta;
      else delete tmeta;
    }
    tmeta_minimal = new TimeMetaInfoMinimal();
    tmeta_minimal->cloneFrom(xtmeta);
    tmetaAlien = 0;
    tmetaArr = 0;
  }

  void tmetaCloneFromMatrix(TimeMetaInfo *xtmeta) {
    tmetaClone(xtmeta);
    /*if ((tmeta != NULL)&&(!tmetaAlien)) {
      if (tmetaArr) delete[] tmeta;
      else delete tmeta;
    }
    tmeta = new TimeMetaInfo();
    tmeta->cloneFrom(xtmeta);
    tmetaAlien = 0;
    tmetaArr = 0;*/
  }
  void tmetaCloneFromMatrix(TimeMetaInfoMinimal *xtmeta) {
    tmetaClone(xtmeta);
    /*
    if ((tmeta_minimal != NULL)&&(!tmetaAlien)) {
      if (tmetaArr) delete[] tmeta_minimal;
      else delete tmeta_minimal;
    }
    //tmeta = new TimeMetaInfo(_tmeta[0]); // copy constructor
    tmeta_minimal = new TimeMetaInfo();
    tmeta_minimal->cloneFrom(xtmeta);
    tmetaAlien = 0; tmetaArr=0; //??
    */
  }
  void tmetaCloneFromMatrixToFull(TimeMetaInfoMinimal *xtmeta) {
    tmetaCloneToFull(xtmeta);
  }
  void tmetaCloneFromMatrixToMinimal(TimeMetaInfo *xtmeta) {
    tmetaCloneToMinimal(xtmeta);
  }

  void tmetaResample(long sourceRate, long targetRate, long vIdxStart) {
   // TODO
  }

  void setF(int n, FLOAT_DMEM v) { dataF[n]=v; } // WARNING: index n is not checked!
  void setI(int n, INT_DMEM v) { dataI[n]=v; } // WARNING: index n is not checked!

  virtual ~cVector();
};


// memory organsisation of cMatrix:
// array index x = col*N + row   ( t*N + n )
class DLLEXPORT cMatrix : public cVector { public:
  long nT;

  cMatrix(int lN, int lnT, int ltype=DMEM_FLOAT);
  // TODO: overwritten getval/setval functions for Int and Float
  FLOAT_DMEM getF(int n, int t) { return dataF[n+t*N]; } // WARNING: index n is not checked!
  INT_DMEM getI(int n, int t) { return dataI[n+t*N]; } // WARNING: index n is not checked!
  void setF(int n, int t, FLOAT_DMEM v) { dataF[n+t*N]=v; } // WARNING: index n is not checked!
  void setI(int n, int t, INT_DMEM v) { dataI[n+t*N]=v; } // WARNING: index n is not checked!
  // transpose matrix
  void transpose();
  // reshape to given dimensions, perform sanity check...
  void reshape(long R, long C);
  cMatrix * getRow(long R) {
    cMatrix *r = new cMatrix(1,nT,type);
    cMatrix *ret = getRow(R,r);
    if (ret==NULL) delete r;
    return ret;
/*    long i;
    if (type==DMEM_FLOAT) for (i=0; i<nT; i++) { r->dataF[i] = dataF[i*N+R]; }
    else if (type==DMEM_INT) for (i=0; i<nT; i++) { r->dataI[i] = dataI[i*N+R]; }
    else { delete r; return NULL; }
    r->tmetaReplace(tmeta);
    return r;*/
  }

  void tmetaReplace(TimeMetaInfo *_tmeta) {
    if ((tmeta != NULL)&&(!tmetaAlien)) {
      if (tmetaArr) delete[] tmeta;
      else delete tmeta;
      tmetaAlien = 1;
    }
    tmeta = _tmeta; tmetaArr=1;
  }
  void tmetaClone(TimeMetaInfo *_tmeta, long _nT=-1) {
    if (_nT == -1) _nT = nT;
    if ((tmeta != NULL)&&(!tmetaAlien)) {
      if (tmetaArr) delete[] tmeta;
      else delete tmeta;
    }
    tmeta = new TimeMetaInfo[nT];
    // TODO: use copy constructors???
    memcpy(tmeta,_tmeta,sizeof(TimeMetaInfo)*MIN(nT,_nT));
    // strdup the metadata->text field??
    if (_nT < nT) {
      // TODO: manually free the variables in metadata here!!!
      bzero( tmeta + _nT, sizeof(TimeMetaInfo)*(nT-_nT) );
    }
    tmetaAlien = 0; tmetaArr=1;
  }
  void tmetaResample(long sourceRate, long targetRate, long vIdxStart) {
    // TODO
  }


  cMatrix * getRow(long R, cMatrix *r) {
    long i;
    if (r==NULL) r = new cMatrix(1,nT,type);
    else {
      if (r->nT != nT) { delete r; r = new cMatrix(1,nT,type); }
      if (type != r->type) return NULL;
    }
    long nn = MIN(nT,r->nT);
    if (type==DMEM_FLOAT) {
      FLOAT_DMEM *df = dataF+R;
      for (i=0; i<nn; i++) { r->dataF[i] = *df; df += N; /*dataF[i*N+R];*/ }
      for ( ; i<r->nT; i++) { r->dataF[i] = 0.0; }
    } else if (type==DMEM_INT) {
      INT_DMEM *di = dataI+R;
      for (i=0; i<nn; i++) { r->dataI[i] = *di; di += N; /*dataI[i*N+R];*/ }
    } else { /*delete r;*/ return NULL; }
    r->tmetaReplace(tmeta);
    return r;
  }

  cVector* getCol(long C) {
    cVector *c = new cVector(N,type);
    long i;
    if (type==DMEM_FLOAT) for (i=0; i<N; i++) { c->dataF[i] = dataF[N*C+i]; }
    else if (type==DMEM_INT) for (i=0; i<N; i++) { c->dataI[i] = dataI[N*C+i]; }
    else { delete c; return NULL; }
    c->tmetaReplace(tmeta);
    return c;
  }
  cVector* getCol(long C, cVector *c) {
    if (c==NULL) c = new cVector(N,type);
    long i;
    if (type==DMEM_FLOAT) for (i=0; i<N; i++) { c->dataF[i] = dataF[N*C+i]; }
    else if (type==DMEM_INT) for (i=0; i<N; i++) { c->dataI[i] = dataI[N*C+i]; }
    else { delete c; return NULL; }
    c->tmetaReplace(tmeta);
    return c;
  }

  void setRow(long R, cMatrix *row) { // NOTE: set row does not change tmeta!!
    long i;
    if (row!=NULL) {
      long nn = MIN(nT,row->nT);
      if (row->type == type) {
        if (type==DMEM_FLOAT) for (i=0; i<nn; i++) { dataF[i*N+R] = row->dataF[i]; }
        else if (type==DMEM_INT) for (i=0; i<nn; i++) { dataI[i*N+R] = row->dataI[i]; }
      } else {
        if (type==DMEM_FLOAT) for (i=0; i<nn; i++) { dataF[i*N+R] = (FLOAT_DMEM)(row->dataI[i]); }
        else if (type==DMEM_INT) for (i=0; i<nn; i++) { dataI[i*N+R] = (INT_DMEM)(row->dataF[i]); }
      }
    }
  }
  // convert a matrix tmeta to a vector tmeta spanning the whole matrix by adjusting size, period, etc.
  void tmetaSquash(double period=-1.0) {
    if (tmeta != NULL) {
      tmeta[0].framePeriod = tmeta[0].period;
      if (period != -1.0)
        tmeta[0].period = period;
      //else   // TODO::: there is bug here......
      //  tmeta[0].period = tmeta[nT-1].time - tmeta[0].time + tmeta[nT-1].lengthSec;
      tmeta[0].lengthSec = tmeta[nT-1].time - tmeta[0].time + tmeta[nT-1].lengthSec;
      tmeta[0].vLengthSec = tmeta[0].lengthSec;
      tmeta[0].lengthFrames = (long)ceil(tmeta[0].lengthSec / tmeta[0].framePeriod);
      tmeta[0].vLengthFrames = (long)ceil(tmeta[0].vLengthSec / tmeta[0].framePeriod);
      tmeta[0].lengthSamples = (long)ceil(tmeta[0].lengthSec / tmeta[0].samplePeriod);
      tmeta[0].vLengthSamples = (long)ceil(tmeta[0].vLengthSec / tmeta[0].samplePeriod);
    }
  }
  
  int resize(long _new_nT) {
    int ret=1;
    if (_new_nT < nT) return 1;

    // dataF / dataI
    if (type == DMEM_FLOAT) {
      FLOAT_DMEM *tmp = (FLOAT_DMEM *)crealloc(dataF, _new_nT*sizeof(FLOAT_DMEM)*N, nT*sizeof(FLOAT_DMEM)*N);
      if (tmp==NULL) ret = 0;
      else dataF = tmp;
    } else if (type == DMEM_INT) {
      INT_DMEM *tmp = (INT_DMEM *)crealloc(dataI, _new_nT*sizeof(INT_DMEM)*N, nT*sizeof(INT_DMEM)*N);
      if (tmp==NULL) ret = 0;
      else dataI = tmp;
    }

    if (ret) {
      // tmeta
      TimeMetaInfo *old = tmeta;
      tmeta = new TimeMetaInfo[_new_nT];
      if (tmeta == NULL) { ret=0; tmeta = old; }
      else {
        memcpy(tmeta,old,sizeof(TimeMetaInfo)*nT);
        memset(tmeta+nT,0,sizeof(TimeMetaInfo)*(_new_nT-nT));
        if ((old != NULL)&&(!tmetaAlien)) {
          delete[] old;
        }
        tmetaAlien = 0;
      }
    }

    if (ret) nT = _new_nT;

    return ret;
  }

  // convert matrix to long vector with all columns concatenated ...
  const cVector* expand();
  
  ~cMatrix();
};

/******* dataMemory level class ************/

#ifndef MODULE
#define MODULE "dataMemory"
#define UNDEFMOD
#endif

/* result codes for getFrame/Matrix setFrame/Matrix */
#define DMRES_OK        0
#define DMRES_ERR       1
#define DMRES_OORleft   4
#define DMRES_OORright  8
#define DMRES_OORbs     16

/* level configuration */
class DLLEXPORT sDmLevelConfig { public:
  /* timing configuration */
  double T;       // period of frames in this level ("frameStep" in seconds)
  double frameSizeSec; // size of ONE frame in seconds
  double lastFrameSizeSec; // size of ONE frame in seconds, previous level (or level before frameSizeSec was modifed)
  long nT;        // level bufferSize in frames
  double lenSec;  // length of buffer in seconds ("frameStep"*nT)
  double basePeriod; // period of top most level, if available, else 0

  long blocksizeWriter, blocksizeReader; // blocksize of writer, max. blocksize of reader(s)

  /* level type */
  int isRb;
  int noHang;
  int growDyn;
  int type;

  /* config state: if no flag is set: only  timing and type config is set */
  int finalised; /* timing, type, blocksize, and names config is ready AND level mem is allocated */
  int blocksizeIsSet; /* timing, type, and blocksize config is ready */
  int namesAreSet; /* timing, type, blocksize, and names config is ready */

  long N, Nf;         // number of elements, number of fields (Nf is same as fmeta->N)
  FrameMetaInfo *fmeta; // pointer to fmeta, which is allocated in the corresponding dataMemory level
  char *name;  // level name

  sDmLevelConfig(double _T, double _frameSizeSec, long _nT=10, int _type=DMEM_FLOAT, int _isRb=1) :
    T(_T), frameSizeSec(_frameSizeSec), lastFrameSizeSec(_frameSizeSec), nT(_nT), lenSec(0.0), basePeriod(0.0),
    blocksizeWriter(1), blocksizeReader(1),
    isRb(_isRb), noHang(1), growDyn(0),
    type(_type),
    finalised(0), blocksizeIsSet(0), namesAreSet(0),
    N(0), Nf(0), 
    fmeta(NULL),
    name(NULL) 
  { 
    lenSec = T*(double)nT; 
  }

  sDmLevelConfig(double _T, double _frameSizeSec, double _lenSec=1.0, int _type=DMEM_FLOAT, int _isRb=1) :
    T(_T), frameSizeSec(_frameSizeSec), lastFrameSizeSec(_frameSizeSec), nT(0), lenSec(_lenSec), basePeriod(0.0),
    blocksizeWriter(1), blocksizeReader(1),
    isRb(_isRb), noHang(1), growDyn(0),
    type(_type),
    finalised(0), blocksizeIsSet(0), namesAreSet(0),
    N(0), Nf(0), 
    fmeta(NULL),
    name(NULL) 
  { 
    if (T!=0.0) nT = (long)ceil(lenSec/T); 
  }

  sDmLevelConfig(const char *_name, double _T, double _frameSizeSec, long _nT=10, int _type=DMEM_FLOAT, int _isRb=1) :
    T(_T), frameSizeSec(_frameSizeSec), lastFrameSizeSec(_frameSizeSec), nT(_nT), lenSec(0.0), basePeriod(0.0),
    blocksizeWriter(1), blocksizeReader(1),
    isRb(_isRb), noHang(1), growDyn(0),
    type(_type),
    finalised(0), blocksizeIsSet(0), namesAreSet(0),
    N(0), Nf(0), 
    fmeta(NULL),
    name(NULL) 
  { 
    lenSec = T*(double)nT; 
    if (_name != NULL) name = strdup(_name);
  }

  sDmLevelConfig(const char *_name, double _T, double _frameSizeSec, double _lenSec=1.0, int _type=DMEM_FLOAT, int _isRb=1) :
    T(_T), frameSizeSec(_frameSizeSec), lastFrameSizeSec(_frameSizeSec), nT(0), lenSec(_lenSec), basePeriod(0.0),
    blocksizeWriter(1), blocksizeReader(1),
    isRb(_isRb), noHang(1), growDyn(0),
    type(_type),
    finalised(0), blocksizeIsSet(0), namesAreSet(0),
    N(0), Nf(0), 
    fmeta(NULL),
    name(NULL) 
  { 
    if (T!=0.0) nT = (long)ceil(lenSec/T); 
    if (_name != NULL) name = strdup(_name);  
  }
//lcfg(_name, 0.0, 0.0, _nT, _type, rb),

  sDmLevelConfig() :
    T(0.0), frameSizeSec(0.0), lastFrameSizeSec(0.0), nT(0), lenSec(0.0), basePeriod(0.0),
    blocksizeWriter(1), blocksizeReader(1),
    isRb(1), noHang(1), growDyn(0),
    type(DMEM_FLOAT),
    finalised(0), blocksizeIsSet(0), namesAreSet(0),
    N(0), Nf(0), 
    fmeta(NULL),
    name(NULL) {}

  sDmLevelConfig(sDmLevelConfig const &orig) :
    T(orig.T), frameSizeSec(orig.frameSizeSec), lastFrameSizeSec(orig.lastFrameSizeSec), nT(orig.nT), lenSec(orig.lenSec), basePeriod(orig.basePeriod),
    blocksizeWriter(orig.blocksizeWriter), blocksizeReader(orig.blocksizeReader),
    isRb(orig.isRb), noHang(orig.noHang), growDyn(orig.growDyn),
    type(orig.type),
    finalised(orig.finalised), blocksizeIsSet(orig.blocksizeIsSet), namesAreSet(orig.namesAreSet),
    N(orig.N), Nf(orig.Nf), 
    fmeta(orig.fmeta),
    name(NULL) { 
      if (orig.name != NULL) name = strdup(orig.name); 
    }


  sDmLevelConfig(const char *_name, sDmLevelConfig &orig) :
    T(orig.T), frameSizeSec(orig.frameSizeSec), lastFrameSizeSec(orig.lastFrameSizeSec), nT(orig.nT), lenSec(orig.lenSec), basePeriod(orig.basePeriod),
    blocksizeWriter(orig.blocksizeWriter), blocksizeReader(orig.blocksizeReader),
    isRb(orig.isRb), noHang(orig.noHang), growDyn(orig.growDyn),
    type(orig.type),
    finalised(orig.finalised), blocksizeIsSet(orig.blocksizeIsSet), namesAreSet(orig.namesAreSet),
    N(orig.N), Nf(orig.Nf), 
    fmeta(orig.fmeta),
    name(NULL) { 
      if (_name != NULL) name = strdup(_name); 
      else if (orig.name != NULL) name = strdup(orig.name); 
    }

  void updateFrom(sDmLevelConfig const &orig) 
  {
    T = orig.T;
    frameSizeSec = orig.frameSizeSec;
    lastFrameSizeSec = orig.lastFrameSizeSec;
    nT = orig.nT;
    lenSec = orig.lenSec;
    basePeriod = orig.basePeriod;
    blocksizeWriter = orig.blocksizeWriter;
    blocksizeReader = orig.blocksizeReader;
    isRb = orig.isRb;
    noHang = orig.noHang;
    growDyn = orig.growDyn;
    type = orig.type;
    finalised = orig.finalised;
    blocksizeIsSet = orig.blocksizeIsSet;
    namesAreSet = orig.namesAreSet;
    N = orig.N; 
    Nf = orig.Nf; 
    fmeta = orig.fmeta;
    if (name != NULL) {
      free(name);
      name = NULL;
    }  
    if (orig.name != NULL) name = strdup(orig.name);
  }

  void setName(const char *_name) 
  {
    if (_name != NULL) {
      if (name != NULL) free(name);
      name = strdup(_name);
    }
  }

  ~sDmLevelConfig() { if (name != NULL) free(name); }
};

class DLLEXPORT cDataMemory;

class DLLEXPORT cDataMemoryLevel {
  private:
    //char *name;

    int myId;
    cDataMemory * _parent;

// -------- variables for mutex access and locking:
   smileMutex RWptrMtx;  // mutex to lock global curW / curR pointers
   smileMutex RWmtx;     // mutex to lock data area during read / write
   smileMutex RWstatMtx; // mutex to lock nCurRdr and writeReq variables for mut.ex. write/read op. while allowing mutliple parallel reads
   int nCurRdr;
   int writeReqFlag;
// --------
    
    /* level configuration */
    sDmLevelConfig lcfg;
    FrameMetaInfo fmeta;
    int fmetaNalloc;

    /* THIS is now obsolete... it is stored in lcfg 
    FrameMetaInfo fmeta;
int  noHang;
    int  type;   // DMEM_INT or DMEM_FLOAT
    long N;    // number of fields?? (elements??)
    long nT;   // number of timesteps  (for ringbuffers T: actual size, else T=max size)
    int isRb;      // ringbuffer flag..
    int growDyn; // grow dynamically in time..?
    int finalised, namesSet;
    double T;  // period of frames, 0.0 for unperiodic
    double frameSizeSec;  // frameSize (N) in seconds
    */

    /* level buffer */
    cMatrix *data;
    /* level buffer status */
    long curW,curR;  //current write pos, current read pos    (min (read) over all readers / max (write))
    long *curRr;  //current current read pos for each registered reader
    int nReaders;    // number of registered readers (all registered readers will be "waited" for! if you don't want that, don't register your reader)

    /* timing information for every frame in the buffer */ /* ISN'T THIS IN *data ?? */
    TimeMetaInfo  *tmeta;


    int EOI;
    int EOIcondition;
    


    /*
    // ???: is this still used??? TODO!
    long rbClip(long vIdx) {  // convert indicies if ringbuffer , otherwise perform range check, return -1 if out of range
      if (isRb) return vIdx%nT;
      else {
           if (vIdx < 0) return -1;
           if (vIdx >= nT) return -1;
           return vIdx;
      }
    }
*/

    /* check current read indicies of all registered readers fro this level and update global (minimal) read index curRm
       also update single reader's read index if the have fallen behind the global read index for whatever reason. */
    void checkCurRr() {
      if (nReaders>0) {
        long newCurR=curRr[0];
        int i;
        for (i=1; i<nReaders; i++) {
          if (curRr[i] < curR) {
            SMILE_DBG(3,"(level='%s') auto increasing reader %i 's read index to %i",getName(),i,curR);
            curRr[i] = curR;
          }
          if (curRr[i] < newCurR) newCurR=curRr[i];
        }
        curR = newCurR;
      }
    }
    
    // validate write index, and if applicable increase curW write counter
    long validateIdxW(long *vIdx, int special=-1)
    {
      // TODO: growdyn for ringbuffers
      SMILE_DBG(5,"validateIdxW ('%s')\n         vidx=%i special=%i curW=%i curR=%i nT=%i",getName(),*vIdx,special,curW,curR,lcfg.nT);

      if (special == DMEM_IDX_CURW) *vIdx = curW;
      else if (special != -1) return -1;
      if (*vIdx < 0) return -1;
      int nh=0;
      if (lcfg.noHang==1) { if (nReaders == 0) nh = 1; }
      else if (lcfg.noHang==2) nh = 1;

      if ( (lcfg.isRb)&&(*vIdx <= curW)&&(nh||((lcfg.nT - (curW-curR)) > 0)) )
      { 

        if (*vIdx==curW) curW++; 
        if ((lcfg.noHang==2)&&((lcfg.nT - (curW-curR)) <= 0)) {
          SMILE_DBG(3,"data lost while writing value to ringbuffer level '%s'",getName());
          curR = curW-lcfg.nT+1;
        }
        return *vIdx%lcfg.nT; 
      
      } else {

        if ((!lcfg.isRb)&&(*vIdx <= curW)) {
          if (*vIdx >= lcfg.nT) { 
            // if growDyn is set, realloc level size....
            if (lcfg.growDyn) {
              SMILE_DBG(3,"increasing buffer size of level '%s' from nT=%i to nT=%i",getName(),lcfg.nT,lcfg.nT*2);

              if (data->resize(lcfg.nT*2)) {
                // new code for "crealloc" of tmeta to properly deal with c++ class array...
                TimeMetaInfo * tmeta_new = new TimeMetaInfo[lcfg.nT*2]();
                long ii;
                for (ii=0; ii<lcfg.nT; ii++) { tmeta_new[ii].cloneFrom(tmeta+ii); }
                delete [] tmeta;
                tmeta = tmeta_new;
                // OLD: tmeta = (TimeMetaInfo *)crealloc(tmeta,sizeof(TimeMetaInfo) * lcfg.nT*2, sizeof(TimeMetaInfo) * lcfg.nT);
                lcfg.nT *= 2;
                lcfg.lenSec *= 2.0;
                if (*vIdx==curW) curW++; return *vIdx;
              }
            } else {
              // else
              SMILE_DBG(3,"data lost while writing value to level '%s'",getName());
            }
          } else
          { if (*vIdx==curW) curW++; return *vIdx; }
        }
        return -1;

      }
    }

    // validate write index range, and if applicable set curW write counter to end of range+1
    long validateIdxRangeW(long *vIdx, long vIdxEnd, int special=-1)
    {
      // TODO: growdyn for ringbuffers
      SMILE_DBG(5,"validateIdxRangeW ('%s')\n         vidx=%i vidxend=%i special=%i curW=%i curR=%i nT=%i",getName(),*vIdx,vIdxEnd,special,curW,curR,lcfg.nT);

      if (vIdxEnd < *vIdx) { SMILE_ERR(3,"validateIdxRangeW: vIdxEnd (%i) cannot be smaller than vIdx (%i)!",vIdxEnd,*vIdx); return -1; }
      if (special == DMEM_IDX_CURW) { vIdxEnd -= *vIdx; *vIdx = curW; vIdxEnd += curW; }
      else if (special != -1) return -1;
      if (*vIdx < 0) return -1;
      SMILE_DBG(5,"validateIdxRangeW(2) vidx=%i vidxend=%i special=%i curW=%i curR=%i nT=%i",*vIdx,vIdxEnd,special,curW,curR,lcfg.nT);

      int nh=0;
      if (lcfg.noHang==1) { if (nReaders == 0) nh = 1; }
      else if (lcfg.noHang==2) nh = 1;

      if ( (lcfg.isRb)&&(*vIdx <= curW)&&(nh||(vIdxEnd-*vIdx <= (lcfg.nT - (curW-curR)))) ) {

        if (vIdxEnd>=curW) curW = vIdxEnd;
        if ((lcfg.noHang==2)&&(vIdxEnd-*vIdx >= (lcfg.nT - (curW-curR)))) {
          SMILE_DBG(3,"data lost while writing matrix to ringbuffer level '%s' (vIdxEnd %i, *vIdx %i, lcfg.nT %i, curW %i, curR %i)",getName(),vIdxEnd,*vIdx,lcfg.nT,curW,curR);
        }
        return *vIdx%lcfg.nT;

      } else {

        if ((!lcfg.isRb)&&(*vIdx <= curW)) {
          if ((*vIdx >= lcfg.nT)||(vIdxEnd >= lcfg.nT)) {
            // if growDyn is set, realloc level size....
            if (lcfg.growDyn) {
              long newS = lcfg.nT*2;
              if (newS < vIdxEnd) newS = vIdxEnd+10;
              if (newS < *vIdx) newS = *vIdx+10;

              SMILE_DBG(3,"increasing buffer size of level '%s' from nT=%i to nT=%i",getName(),lcfg.nT,newS);
              if (data->resize(newS)) {
                // new code for "crealloc" of tmeta to properly deal with c++ class array...
                TimeMetaInfo * tmeta_new = new TimeMetaInfo[newS]();
                for (long ii = 0; ii < lcfg.nT; ii++) {
                  tmeta_new[ii].cloneFrom(tmeta + ii);
                }
                delete[] tmeta;
                tmeta = tmeta_new;
                // OLD: // tmeta = (TimeMetaInfo *)crealloc(tmeta,sizeof(TimeMetaInfo) * newS, sizeof(TimeMetaInfo) * lcfg.nT);
                lcfg.nT = newS;
                lcfg.lenSec *= (double)newS/(double)lcfg.nT;
                if (vIdxEnd>=curW) curW = vIdxEnd;
                return *vIdx;
              }
            } else {
              // else
              SMILE_ERR(3, "Data lost while writing matrix of size %ld to level '%s'", vIdxEnd - *vIdx, getName());
            }
          } 
          else
          { if (vIdxEnd>=curW) curW = vIdxEnd; return *vIdx; }
        }
        return -1;
      }
    }

    // validate read index, 
    // return value: -1 invalid param, -2 vidx OOR_left, -3 vidx OOR_right, -4 vidx OOR_buffersize(noRb)
    long validateIdxR(long *vIdx, int special=-1, int rdId=-1, int noUpd=0)
    {

      long *_curR;
      if ((rdId >= 0)&&(rdId<nReaders)) _curR = curRr+rdId;
      else _curR=&curR;
      SMILE_DBG(5,"validateIdxR ('%s')\n         vidx=%i special=%i curW=%i curR=%i nT=%i",getName(),*vIdx,special,curW,*_curR,lcfg.nT);

      if ((lcfg.isRb) && (*_curR < curW-lcfg.nT)) { *_curR = curW-lcfg.nT; SMILE_DBG(3,"validateIdxR: rb data possibly lost, curR < curW-nT, curR was automatically increased!"); }
      if (special == DMEM_IDX_CURR) *vIdx = *_curR;
      else if (special != -1) return -1;
      if (*vIdx < 0) return -2;
      // TODO:: set curR to min curRr
      if (lcfg.isRb) {
        if ((*vIdx < curW)&&(*vIdx >= curW-lcfg.nT)) { 
          if (!noUpd) { 
            if ((*vIdx>=*_curR)&&(!noUpd)) *_curR = *vIdx+1; 
            if (rdId >= 0) checkCurRr(); 
          } 
          return *vIdx%lcfg.nT; 
        } else if (*vIdx >= curW) { return -3; } // OOR_right
        else if (*vIdx < curW-lcfg.nT) { return -2; } // OOR_left
      }
      else { // no ringbuffer
        if ((*vIdx < curW)&&(*vIdx < lcfg.nT)) { 
          if (!noUpd) { 
            if (*vIdx>=*_curR) *_curR = *vIdx+1; 
            if (rdId >= 0) checkCurRr(); 
          } 
          return *vIdx; 
        } else if (*vIdx >= curW) { return -3; } // OOR_right
        else if (*vIdx >= lcfg.nT) { return -4; } // OOR_buffersize
      }
      return -1;
    }

    //validate read index range, vIdxEnd   is the index after the last index to read... (i.e. vIdx + len)
    // TODO: error codes
    long validateIdxRangeR(long actualVidx, long *vIdx, long vIdxEnd, int special=-1, int rdId=-1, int noUpd=0, int *padEnd=NULL)
    {
      SMILE_DBG(5,"validateIdxRangeR ('%s')\n         vidx=%i vidxend=%i special=%i curW=%i curR=%i nT=%i",getName(),*vIdx,vIdxEnd,special,curW,curR,lcfg.nT);
      long *_curR;
      if ((rdId >= 0)&&(rdId<nReaders)) _curR = curRr+rdId;
      else _curR=&curR;
      SMILE_DBG(5,"validateIdxRangeR(2) '%s' vidx=%i vidxend=%i special=%i curW=%i _curR=%i nT=%i",this->lcfg.name,*vIdx,vIdxEnd,special,curW,*_curR,lcfg.nT);

      if ((lcfg.isRb) && (*_curR < curW-lcfg.nT)) {
        *_curR = curW-lcfg.nT;
        SMILE_WRN(4, "level: '%s': validateIdxRangeR: rb data possibly lost, curR < curW-nT, curR was automatically increased!", lcfg.name);
      }
      if (vIdxEnd < *vIdx) { SMILE_ERR(2,"validateIdxRangeR: vIdxEnd (%i) cannot be smaller than vIdx (%i)!",vIdxEnd,*vIdx); return -1; }
      if (special == DMEM_IDX_CURR) { vIdxEnd -= *vIdx; actualVidx = *vIdx = *_curR; vIdxEnd += *_curR; }
      else if ((special != -1)&&(special!=DMEM_PAD_ZERO)&&(special!=DMEM_PAD_FIRST)&&(special!=DMEM_PAD_NONE)) return -1;
      if (*vIdx < 0) return -1;

      if ((vIdxEnd > curW)&&(isEOI())) { // pad
        if (padEnd != NULL) {
          *padEnd = vIdxEnd - curW;
          if (*padEnd >= vIdxEnd-*vIdx) { *padEnd = vIdxEnd-*vIdx;  return -1; }
        }
      	// TODO: pad option for "truncate" at the end of input!
        vIdxEnd = curW;
      }
      if ((lcfg.isRb)&&(*vIdx < curW)&&(vIdxEnd <= curW)&&(*vIdx >= curW-lcfg.nT))
      { 
        if (!noUpd) { 
          //if (vIdxEnd>=*_curR) *_curR = *vIdx+1; if (rdId >= 0) checkCurRr(); 
          if (vIdxEnd>=*_curR) *_curR = actualVidx+1; if (rdId >= 0) checkCurRr(); 
        } 
        return *vIdx%lcfg.nT; 
      } else                                              // +1 ????? XXX
        if ((!lcfg.isRb)&&(*vIdx < curW)&&(*vIdx < lcfg.nT)&&(vIdxEnd <= curW)&&(vIdxEnd <= lcfg.nT))
        { 
          if (!noUpd) { 
            if (vIdxEnd>=*_curR) *_curR = actualVidx+1; if (rdId >= 0) checkCurRr(); 
            //if (vIdxEnd>=*_curR) *_curR = *vIdx+1; if (rdId >= 0) checkCurRr(); 
          } 
          return *vIdx; 
        }
      if (padEnd != NULL) *padEnd = 0;
      return -1;
    }

    // write frame data from *_data to level's data matrix at pos rIdx
    void frameWr(long rIdx, FLOAT_DMEM *_data);
    void frameWr(long rIdx, INT_DMEM *_data);

    // write frame data from level's data matrix at pos rIdx to *_data
    void frameRd(long rIdx, FLOAT_DMEM *_data);
    void frameRd(long rIdx, INT_DMEM *_data);

    void setTimeMeta(long rIdx, long vIdx, const TimeMetaInfo *tm);
    void getTimeMeta(long rIdx, TimeMetaInfo *tm);

  public:

    // create level from given level configuration struct, the name in &cfg will be overwritten via _name parameter
    cDataMemoryLevel(int _levelId, sDmLevelConfig &cfg, const char *_name = NULL) :
      myId(_levelId), _parent(NULL),
      lcfg(_name, cfg), fmetaNalloc(0),
      data(NULL), tmeta(NULL), EOI(0), EOIcondition(0),
      curW(0), curR(0), curRr(NULL), nReaders(0), 
      nCurRdr(0), writeReqFlag(0)
    {
      //if ((nT == 0)&&(cfg.lenSec > 0.0)&&(cfg.T>0.0)) { nT = (long)ceil( cfg.lenSec / cfg.T ); }
      if (lcfg.T < 0.0) COMP_ERR("cannot create dataMemoryLevel with period (%f) < 0.0",lcfg.T);
      if (lcfg.nT <= 0) COMP_ERR("cannot create empty dataMemoryLevel nT = %i <= 0",lcfg.nT);
      lcfg.fmeta = &fmeta;
    }

    // create level from minimal set of parameters  // ??? is this still used ?? shouldn't there be a _T at least!!?? //
    cDataMemoryLevel(int _levelId, const char *_name, long _nT, int rb=1, int dyn=0, int _type=DMEM_FLOAT) :
      myId(_levelId), _parent(NULL),
      lcfg(_name, 0.0, 0.0, _nT, _type, rb),
        //sDmLevelConfig(const char *_name, double _T, double _frameSizeSec, long _nT=10, int _type=DMEM_FLOAT, int _isRb=1) :
      fmetaNalloc(0),
      data(NULL),  tmeta(NULL), EOI(0), EOIcondition(0),
      curW(0), curR(0), curRr(NULL), nReaders(0),
      //,RWptrMtx(NULL), RWstatMtx(NULL), RWmtx(NULL),
      nCurRdr(0), writeReqFlag(0)
    {
      if (lcfg.nT <= 0) COMP_ERR("temporal size of dataMemoryLevel cannot be <= 0  (= %i)!",lcfg.nT);
      if (lcfg.nT < 2) lcfg.nT = 2;
      lcfg.fmeta = &fmeta;
    }

    // get level name
    const char *getName() const { return lcfg.name; }

    // set level ID
    void setMyId(int _id) { myId = _id; }
    void setEOI() { EOIcondition = 1; }
    void setEOIcounter(int cnt) { EOI = cnt; };
    int getEOIcounter() { return EOI; }
    void unsetEOI() { EOIcondition = 0; }
    int isEOI() const { return EOIcondition; }

    // query, if names are set, i.e. if level was fixated
    int namesAreSet() const { return lcfg.namesAreSet; }
    // after the names were set by the writer, this must be indicated to the level via a call to fixateLevel()
    void fixateLevel() { 
      lcfg.namesAreSet = 1; 
//      lcfg.fmeta = &fmeta;
    }

    // set parent dataMemory object
    void setParent(cDataMemory * __parent) { _parent = __parent; }

    // adds a field to this level, _N is the number of elements in an array field, set to 0 or 1 for scalar field
    // arrNameOffset: start index for creating array element names
    int addField(const char *lname, int lN, int arrNameOffset=0);
    
    // set info for field i (dataType and custom data pointer)
    int setFieldInfo(int i, int _dataType, void *_info, long _infoSize);
    
    // set info for field 'name' (dataType and custom data pointer). Do NOT include array indicies in field name!
    int setFieldInfo(const char *__name, int _dataType, void *_info, long _infoSize) {
      return setFieldInfo(fmeta.findField(__name), _dataType, _info, _infoSize);
    }

    // register a new data reader... IS THAT ALL ??? yes: we only need to know how many there are, the rest is done in allocReaders()
    int registerReader() { return nReaders++; }

    // allocate config for the readers and initialize it with standard values
    void allocReaders();

    // configure level (check buffersize and config)
    int configureLevel();   
    // finalize config and allocate data memory
    int finaliseLevel();   


    // find a field by it's full name in a finalised level:
    // *arrIdx (if not NULL) will be set to the array index of the requested element, if it is in an array field
    int findField(const char*_fieldName, int *arrIdx) const { return fmeta.findField( _fieldName, arrIdx ); }

    // sets the arrNameOffset (see addField) of the field that was added last
    void setArrNameOffset(int arrNameOffset);
   
    // get a pointer to the custom level meta data (in fmeta), a cVetorMeta class (same class as for each frame in tmeta, but different data!)
    cVectorMeta * getLevelMetaDataPtr() { return &(fmeta.metaData); }


    /* set a frame or matrix beginnig at vIdx, or special index (overrides value of vIdx, see below) */
    // per level always a complete frame must be set...
    // int special = special index, e.g. DMEM_IDX_CURR, DMEM_IDX_CURW
    int setFrame(long vIdx, const cVector *vec, int special=-1);  
    int setMatrix(long vIdx, const cMatrix *mat, int special=-1); 


    /* get a frame or matrix beginning at vIdx, or special index (overrides value of vIdx, see below) */
    // int special = special index, e.g. DMEM_IDX_CURR, DMEM_IDX_CURW
    // *result (if not NULL) will contain a result code indicating success or reason of failure (left or right buffer margin exceeded, etc.)
    // rdId is the id of the current reader (or -1 for an unregistered or global reader)
    cVector * getFrame(long vIdx, int special=-1, int rdId=-1, int *result=NULL);  
    cMatrix * getMatrix(long vIdx, long vIdxEnd, int special=-1, int rdId=-1, int *result=NULL);  

    /* check if a read of length "len" at vIdx or "special" will succeed for reader rdId */
    // *result (if not NULL) will contain a result code indicating success or reason of failure (left or right buffer margin exceeded, etc.)
    // possible result values: (Doc TODO) -1 invalid param, -2 vidx OOR_left, -3 vidx OOR_right, -4 vidx OOR_buffersize(noRb)
    int checkRead(long vIdx, int special=-1, int rdId=-1, long len=1, int *result=NULL) 
    {
      long rIdx;
      long vIdxEnd = vIdx + len;
      long vIdxold = vIdx;
      if ((vIdx < 0)&&(vIdxEnd > 0)) vIdx = 0;
      if (len < 0) return 0;

      smileMutexLock(RWptrMtx);
      if (len<=1) rIdx = validateIdxR(&vIdx,special,rdId,1);
      else rIdx = validateIdxRangeR(vIdxold,&vIdx,vIdxEnd,special,rdId,1);

      smileMutexUnlock(RWptrMtx);
      if (result!=NULL) {
        if (rIdx == -2) *result=DMRES_OORleft|DMRES_ERR;
        else if (rIdx == -3) *result=DMRES_OORright|DMRES_ERR;
        else if (rIdx == -4) *result=DMRES_OORbs|DMRES_ERR;
        else if (rIdx >= 0) *result=DMRES_OK;
        else *result=DMRES_ERR;
      }
      //XXX TODO: it might be possible that checkRead returns ok, then another reader reads data, 
      //          a writer writes new data, and the read will fail which was confirmed by checkRead.... 
      //          ?? what do we do about it ??
      //        maybe use level lock() functions, that the caller may (or may not) use...

      if (rIdx >= 0) return 1;  // a read will likely succeed (see above TODO notice)
      else return 0;  // an error occurred, read is not possible
    }


    /* set read index of reader rdId (or global read index, if rdId=-1) 
       to user defined _curR or to write index curW+1, if _curR=-1 or omitted.
       if _curR=-1 or omitted, thus, the level buffer will be cleared (not physically but logically)
       use this, if your reader only requires data "part time" (e.g. reading only voiced segments of speech, etc.)
      */
    void catchupCurR(int rdId=-1, int _curR = -1);

    /* methods to get info about current level fill status */
    // (e.g. number of frames written, curW, curR(global) and freeSpace, etc.)

    /* get current write index (index that will be written to NEXT) */
    long getCurW() 
    {
      smileMutexLock(RWptrMtx);
      long res = curW;
      smileMutexUnlock(RWptrMtx);
      return res;
    }  

    /* current read index (index that will be read NEXT) of reader rdId or global (min of all readers) if rdId is omitted */
    long getCurR(int rdId=-1) 
    {
      long res;
      smileMutexLock(RWptrMtx);
      if ((rdId < 0)||(rdId >= nReaders)) { 
        res = curR;
      } else {
        res = curRr[rdId];
      }
      smileMutexUnlock(RWptrMtx);
      return res;
    }  

    /* get number of free frames in current level (for ringbuffers: nT-(curW-curR)) */
    // NOTE: this is not the actual number of free frames, but the number of frames that currently can be written to,
    //       i.e. the result depends on the value of the lcfg.noHang setting
    long getNFree(int rdId=-1) 
    { 
	    if ((lcfg.noHang == 1) && (nReaders<=0))
	      return lcfg.nT;
	    if ((lcfg.noHang == 2))
	      return lcfg.nT;
	    if ((lcfg.growDyn == 1))
	      return 1000000;  // default value, because level will grow

      long ret=0;
      smileMutexLock(RWptrMtx);
      if (lcfg.isRb) {
        if ((rdId>=0)&&(rdId<nReaders)) {
          SMILE_DBG(5,"getNFree(rdId=%i) level='%s' curW=%i curRr=%i nT=%i free=%i",rdId,getName(),curW,curRr[rdId],lcfg.nT,lcfg.nT - (curW-curRr[rdId]));
          ret = lcfg.nT - (curW-curRr[rdId]);
        } else {
          SMILE_DBG(5,"getNFree:: level='%s' curW=%i curR=%i nT=%i free=%i",getName(),curW,curR,lcfg.nT,lcfg.nT - (curW-curR));
          ret = lcfg.nT - (curW-curR);
        }
      } else {
        ret = (lcfg.nT - curW);
      }
      smileMutexUnlock(RWptrMtx);
      return ret;
    }
    
    /* get number of available frames for reader rdId */
    long getNAvail(int rdId=-1)
    { 
      long ret=0;
      smileMutexLock(RWptrMtx);
      if (lcfg.isRb) {
        if ((rdId>=0)&&(rdId<nReaders)) {
          SMILE_DBG(5,"getNAvail(rdId=%i) level='%s' curW=%i curRr=%i nT=%i avail=%i",rdId,getName(),curW,curRr[rdId],lcfg.nT,(curW-curRr[rdId]));
          ret = curW - curRr[rdId];
        } else {
          SMILE_DBG(5,"getNAvail:: level='%s' curW=%i curR=%i nT=%i avail=%i",getName(),curW,curR,lcfg.nT,(curW-curR));
          ret = curW - curR;
        }
      } else {
        ret = curW;
      }
      smileMutexUnlock(RWptrMtx);
      return ret;
    }

    /* maximum readable index ("last" index where data was written to) */
    long getMaxR();

    /* minimum readable index 
       (relevant only for ringbuffers, for regular buffers this function will always return 0) */
    long getMinR();  


    /**** get level configuration *****/
    const sDmLevelConfig * getConfig() { return &lcfg; }

    /* get number of registered readers ( if the level is not finalised, it is the number of readers *currently* registered ! ) */
    long getNreaders() { return nReaders; }

    /* check if level is ready for reading, get level config (WITHOUT NAMES!) and update blocksizeReader */
    const sDmLevelConfig * queryReadConfig( long _blocksizeReader )
    {
      if (!lcfg.blocksizeIsSet) {
        if (lcfg.blocksizeReader < _blocksizeReader) lcfg.blocksizeReader = _blocksizeReader;
        return &lcfg; 
      } else {
        SMILE_ERR(1,"attempt to update blocksizeReader, however blocksize config for level '%s' is already fixed!",lcfg.name);
        return NULL;
      }
    }

    /* check if level is ready for reading, get level config (WITHOUT NAMES!) and update blocksizeReader */
    const sDmLevelConfig * queryReadConfig( double _blocksizeReaderSeconds )
    {
      if (lcfg.T != 0.0) return queryReadConfig( (long)ceil(_blocksizeReaderSeconds/lcfg.T) );
      else return queryReadConfig( (long)ceil(_blocksizeReaderSeconds) );
    }

//OBSOLETE:    long getLevelSize() { return lcfg.nT; } // get size of level in frames

    // TODO: should these methods be passed to fmeta... or should on use getConfig() and call the proper fmeta methods??
    /* get name of field with index "n" (n = 0..lcfg.Nf-1  OR equivalent: n = 0..lcfg.fmeta.N-1), 
         *_N will be filled with number of elements in this field. 
         for *_arrNameOffset see documentation of addField */
    const char * getFieldName(int n, int *lN=NULL, int *larrNameOffset=NULL);

    /* get name of element "n"  (n= 0..lcfg.N) */
     // IMPORTANT! the caller must free() the returned string!!
    char * getElementName(int n); 
    
    /**** set config functions ***/
    /* set frame size in seconds */
    void setFrameSizeSec(double fss) { if (fss > 0.0) lcfg.frameSizeSec = fss; }
    void setBlocksizeWriter(long _bsw, int _force=0) { 
      if (_force) { lcfg.blocksizeWriter = _bsw; }
      else if (_bsw > lcfg.blocksizeWriter) { lcfg.blocksizeWriter = _bsw; }
    }

    /**** vIdx to real time conversion functions (preferred method) */
    /* seconds to vIdx (for THIS level!) */
    long secToVidx(double sec); // returns a vIdx, TODO: (for periodic levels this function is easy, however for aperiodic levels we must iterate through all frames... maybe also for periodic to compensate round-off errors?)
    /* vIdx (for THIS level!) to real time in seconds */
    double vIdxToSec(long vIdx); 

    /** print number of readers, and size of level in frames and seconds, etc.. **/
    // available levels of detail : 0, 1, 2
    void printLevelStats(int detail=1);
    

    /*** level locking functions : manually lock/unlock level for exclusive access (for multi-threaded environments) ***/
    // PLEASE USE THESE RESPONSIBLE, YOU MAY BREAK YOUR APPLICATION IF YOU FORGET TO UNLOCK A LEVEL!!!
    /* TODO */
    void lock() {} // use a fourth mutex, LVLmtx or so? hm... better to use RWstatMtx..?
    void unlock() {} // use a fourth mutex, LVLmtx or so


    /**** destructor.. clean up everything ******/
    ~cDataMemoryLevel() {
      // TODO: these might have not been initialized.... !!!???
      smileMutexDestroy(RWptrMtx);
      smileMutexDestroy(RWstatMtx);
      smileMutexDestroy(RWmtx);
      //--------
      if (tmeta != NULL) delete[] tmeta; // was: free(tmeta) !
      if (data != NULL) delete data;
      if (curRr != NULL) free(curRr);
    }

};

#ifdef UNDEFMOD
#undef MODULE
#undef UNDEFMOD
#endif


/************** dataMemory class ***********************/

class DLLEXPORT cSmileList {
private:
  long nEl, nElAlloc, blockAlloc;
  long elSize;
  void *el;
public:
  // get pointer to element n, for reading and writing, if n>nElAlloc a new element will be allocated 
  void * getElementPtr(long n=-1) {
    if (n==-1) n=nEl;
    if (n<0) return NULL;
    if (n >= nElAlloc) {
      void *tmp = (void *)crealloc(el, elSize * (n+blockAlloc), elSize*nElAlloc);
      if (tmp==NULL) OUT_OF_MEMORY;
      el = tmp;
      nElAlloc = (n+blockAlloc);
    }
    if (n>=nEl) nEl = n+1;
    return (void*)( ((char*)el)+(n*elSize) );
  }

  // get number of elements in list
  long getNEl() { return nEl; }

  cSmileList(long _elSize=1, long _blockAlloc=50) : nEl(0), nElAlloc(0), blockAlloc(_blockAlloc), elSize(_elSize), el(NULL)
    { if (elSize < 1) { elSize=1; } }
  ~cSmileList() { if (el!=NULL) free(el); }
};

class DLLEXPORT cCharStrList : public cSmileList { 
public:
  cCharStrList(long _blockAlloc=50) :
      cSmileList(sizeof(const char *), _blockAlloc)
  {
  }
 
  const char * getElement(long n=-1) {
    return (const char *)getElementPtr(n);
  }
};

// data for read or write request
class DLLEXPORT sDmLevelRWRequest{ public:
  const char * instanceName;
  const char * levelName;
};

class DLLEXPORT cDmLevelRWRequestList : public cSmileList { 
public:
  cDmLevelRWRequestList(long _blockAlloc=50) :
      cSmileList(sizeof(sDmLevelRWRequest), _blockAlloc)
  {
  }
 
  sDmLevelRWRequest * getElement(long n=-1) {
    return (sDmLevelRWRequest *)getElementPtr(n);
  }

  int findRequest(const char *lvl, const char *instName) {
    int i;
    for (i=0; i<getNEl(); i++) {
      sDmLevelRWRequest *x = getElement(i);
      if (x!=NULL) {
        if ( (!strcmp(lvl,x->levelName)) && ((instName==NULL)||(!strcmp(instName,x->instanceName))) ) return i+1;
      }
    }
    return 0;
  }
};


#define COMPONENT_DESCRIPTION_CDATAMEMORY "central data memory component"
#define COMPONENT_NAME_CDATAMEMORY "cDataMemory"

class DLLEXPORT cDataMemory : public cSmileComponent {
  private:
    cDataMemoryLevel **level;
    int nLevelsAlloc /*N*/, nLevels /*I*/;   // N: total number allocated, I actual number of levels initialized - 1

    cDmLevelRWRequestList rrq;  // read requests
    cDmLevelRWRequestList wrq;  // write requests

    // used internally...
    void _addLevel();

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    void fetchConfig() {}
    virtual int myRegisterInstance(int *runMe=NULL);
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t) { return 0; }
    
  public:
    SMILECOMPONENT_STATIC_DECL

    cDataMemory() : cSmileComponent("dataMemory"), level(NULL), 
      nLevelsAlloc(0), nLevels(-1) {}

    cDataMemory(const char *_name) : cSmileComponent(_name), level(NULL),
      nLevelsAlloc(0), nLevels(-1) {}

    /* register a read request (during "register" phase) */
    void registerReadRequest(const char *lvl, const char *componentInstName=NULL);
    void registerWriteRequest(const char *lvl, const char *componentInstName=NULL);

    /* register a new level, and check for uniqueness of name */
    int registerLevel(cDataMemoryLevel *l);

    /* add a new level with given _lcfg parameters */
    int addLevel(sDmLevelConfig *_lcfg, const char *_name=NULL);

    /* set end-of-input flag */
    virtual void setEOI() {
      int i;
      // set end-of-input for all levels:
      for (i=0; i<=nLevels; i++) level[i]->setEOI();
      // set end-of-input in this component:
      cSmileComponent::setEOI();
    }

    virtual void unsetEOI() {
      int i;
      // set end-of-input for all levels:
      for (i=0; i<=nLevels; i++) level[i]->unsetEOI();
      // set end-of-input in this component:
      cSmileComponent::unsetEOI();
    }

    virtual int setEOIcounter(int cnt) {
      int i;
      // set end-of-input for all levels:
      for (i=0; i<=nLevels; i++) level[i]->setEOIcounter(cnt);
      // set end-of-input in this component:
      return cSmileComponent::setEOIcounter(cnt);
    }

    // get index of level 'name'
    int findLevel(const char *name);

    /**** functions which will be forwarded to the corresponding level *****/

    /* check if level is ready for reading, get level config (WITHOUT NAMES!) and update blocksizeReader */
    const sDmLevelConfig * queryReadConfig( int _level, long _blocksizeReader=1 )
      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->queryReadConfig(_blocksizeReader); else return NULL; }
    const sDmLevelConfig * queryReadConfig( int _level, double _blocksizeReaderSeconds=1 )
      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->queryReadConfig(_blocksizeReaderSeconds); else return NULL; }

    // adds a field to level <level>, _N is the size of an array field, set to 0 or 1 for scalar field
    // the name must be unique per level...
    int addField(int llevel, const char *lname, int lN, int arrNameOffset=0)
    {  
       if ((llevel>=0)&&(llevel<=nLevels)) 
         return level[llevel]->addField(lname, lN, arrNameOffset);
       else 
         return 0; 
    }

    // set info for field i (dataType and custom data pointer)
    int setFieldInfo(int _level, int i, int _dataType, void *_info, long _infoSize) {
      if ((_level>=0)&&(_level<=nLevels)) return level[_level]->setFieldInfo(i,_dataType,_info,_infoSize); else return 0;
    }
    
    // set info for field 'name' (dataType and custom data pointer). Do NOT include array indicies in field name!
    int setFieldInfo(int _level, const char *_name, int _dataType, void *_info, long _infoSize) {
      if ((_level>=0)&&(_level<=nLevels)) return level[_level]->setFieldInfo(_name,_dataType,_info,_infoSize); else return 0;
    }

    // registers a reader for level _level, returns the reader id assigned to the new reader
    int registerReader(int _level)
      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->registerReader(); else return -1; }

  // OBOSOLETE : ???
    // get index of level 'name' and finalise this level
    int finaliseLevel(const char *name);
    // finalise level n, return 1 on success, 0 on failure
    int finaliseLevel(int n);

    int setFrame(int _level, long vIdx, const cVector *vec, int special=-1/*, int *result=NULL*/)
      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->setFrame(vIdx,vec,special/*, result*/); else return 0; }
    int setMatrix(int _level, long vIdx, const cMatrix *mat, int special=-1/*, int *result=NULL*/)
      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->setMatrix(vIdx,mat,special/*, result*/); else return 0; }

    /* check if a read will succeed */
    int checkRead(int _level, long vIdx, int special=-1, int rdId=-1, long len=1, int *result=NULL)
      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->checkRead(vIdx,special,rdId,len,result); else return -1; }

    // the memory pointed to by the return value must be freed via delete() by the calling code!!
    // TODO: optimise this... don't allocate a new frame everytime something is returned!! maybe provide an option
         // for passing a buffer frame or so...
    cVector * getFrame(int _level, long vIdx, int special=-1, int rdId=-1, int *result=NULL)
      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->getFrame(vIdx,special,rdId,result); else return NULL; }
    cMatrix * getMatrix(int _level, long vIdx, long vIdxEnd, int special=-1, int rdId=-1, int *result=NULL)
      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->getMatrix(vIdx,vIdxEnd,special,rdId,result); else return NULL; }

    // set current read index to current write index to prevent hangs, if the readers do not read data sequentially, or if the readers skip data
    void catchupCurR(int _level, int rdId=-1, long _curR=-1 /* if >= 0, value that curR[rdId] will be set to! */ ) 
      { if ((_level>=0)&&(_level<=nLevels)) level[_level]->catchupCurR(rdId,_curR); }



    // TODO: methods to get info about current level fill status (e.g. number of frames written, curW, curR(global) and freeSpace, etc.)
    long getCurW(int _level)  // current write index
      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->getCurW(); else return -1; }
    long getCurR(int _level, int rdId=-1)  // current read index
      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->getCurR(rdId); else return -1; }
    long getNFree(int _level,int rdId=-1) // get number of free frames in current level (rb: nT-(curW-curR))
      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->getNFree(rdId); else return -1; }
    long getNAvail(int _level,int rdId=-1) // get number of available frames in current level (curW-curR)
      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->getNAvail(rdId); else return -1; }
    //long getLevelSize(int _level) // get size of level in frames
//      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->getLevelSize(); else return -1; }
    long getMaxR(int _level)  // maximum readable index (index where data was written to)
      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->getMaxR(); else return -1; }
    long getMinR(int _level)  // minimum readable index (relevant only for ringbuffers, otherwise it will always return 0)
      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->getMinR(); else return -1; }
    long getNreaders(int _level)  // number of registered readers
      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->getNreaders(); else return -1; }

    // query, if names are set, i.e. if level was fixated
    int namesAreSet(int _level) 
      { if ((_level>=0)&&(_level<=nLevels)) { return level[_level]->namesAreSet(); } else return 0; }
    void fixateLevel(int _level) 
      { if ((_level>=0)&&(_level<=nLevels)) level[_level]->fixateLevel(); }

    /* functions for manually obtaining exclusive level access */
    void lockLevel(int _level)
      { if ((_level>=0)&&(_level<=nLevels)) level[_level]->lock(); }  
    void unlockLevel(int _level)
      { if ((_level>=0)&&(_level<=nLevels)) level[_level]->unlock(); }  

    const sDmLevelConfig * getLevelConfig(int _level)
      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->getConfig(); else return NULL; }
    
    const char * getLevelName(int _level)
      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->getName(); else return NULL; }

    const char * getFieldName(int _level, int n, int *lN=NULL, int *_arrNameOffset=NULL)
      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->getFieldName(n, lN, _arrNameOffset); else return NULL; }
    char * getElementName(int _level, int n) // caller must free() returned string!!
      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->getElementName(n); else return NULL; }

    cVectorMeta * getLevelMetaDataPtr(int _level)
      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->getLevelMetaDataPtr(); else return NULL; }
 	  void setArrNameOffset(int _level, int arrNameOffset)
	    { if ((_level>=0)&&(_level<=nLevels)) level[_level]->setArrNameOffset(arrNameOffset); }
    void setFrameSizeSec(int _level, double fss)
      { if ((_level>=0)&&(_level<=nLevels)) level[_level]->setFrameSizeSec(fss);  }
    void setBlocksizeWriter(int _level, long _bsw)
      { if ((_level>=0)&&(_level<=nLevels)) level[_level]->setBlocksizeWriter(_bsw);  }

    /* print an overview over registered levels and their configuration */
    void printDmLevelStats(int detail=1)
      { int i; for (i=0; i<=nLevels; i++) level[i]->printLevelStats(detail); }

    // TODO: functions to convert absolute times (in seconds) to level frame indicies!
    long secToVidx(int _level, double sec) // returns a vIdx
      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->secToVidx(sec); else return -1; }
    //(for period levels this function is easy, however for aperiodic levels we must iterate through all frames... maybe also for periodic to compensate round-off errors?)
    double vIdxToSec(int _level, long vIdx) // returns a time in seconds as double
      { if ((_level>=0)&&(_level<=nLevels)) return level[_level]->vIdxToSec(vIdx); else return -1; }
    
    virtual ~cDataMemory();
};



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

void datamemoryLogger(const char *name, const char*dir, long cnt, long vIdx0, long vIdx, long rIdx, long nT, int special, long curR, long curW, long EOI, int nReaders, long *curRr, cVector *vec);

void datamemoryLogger(const char *name, long vIdx0, long vIdx, long rIdx, long nT, int special, long curR, long curW, long EOI, int nReaders, long *curRr, cMatrix *mat);

#endif // __DATA_MEMORY_HPP
