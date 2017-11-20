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

// TODO:!! check what happens at EOI when blocksize is large (it seems the matrix gets zero padded at the end, but it would be better to have it truncated)


/*  openSMILE component:

filter :  (abstract class only)
       linear N-th order filter for single value data streams
       this class processed every element of a frame independently
       derived classes only need to implement the filter algorithm

*/


#include <core/windowProcessor.hpp>

#define MODULE "cWindowProcessor"

SMILECOMPONENT_STATICS(cWindowProcessor)

SMILECOMPONENT_REGCOMP(cWindowProcessor)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CWINDOWPROCESSOR;
  sdescription = COMPONENT_DESCRIPTION_CWINDOWPROCESSOR;

  // we inherit cDataProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor");

  SMILECOMPONENT_IFNOTREGAGAIN(
//    ct->setField("pre","number of past frames required",1);
//    ct->setField("post","number of post frames required",1);
    ct->setField("noPostEOIprocessing","1 = do not process incomplete windows at the end of the input",0);
    //ct->setField("blocksize","# of samples to process at once (for computational speed-up)",256);
  )

  SMILECOMPONENT_MAKEINFO(cWindowProcessor);
}

SMILECOMPONENT_CREATE_ABSTRACT(cWindowProcessor)

//-----

cWindowProcessor::cWindowProcessor(const char *_name, int _pre, int _post) :
  cDataProcessor(_name),
  matnew(NULL),
  isFirstFrame(1),
  row(NULL),
  rowout(NULL), rowsout(NULL),
  pre(_pre),
  post(_post),
  winsize(0),
  //blocksize(256),
  noPostEOIprocessing(0),
  multiplier(1)
{
  winsize = _pre + _post;
}

void cWindowProcessor::fetchConfig()
{
  cDataProcessor::fetchConfig();
/*
  blocksize = getInt("blocksize");
  SMILE_DBG(2,"blocksize = %i",blocksize);
  */

  noPostEOIprocessing = getInt("noPostEOIprocessing");
  if (noPostEOIprocessing) { SMILE_IDBG(2,"not processing incomplete frames at end of input"); }
}

void cWindowProcessor::setWindow(int _pre, int _post)
{
  pre = _pre;
  post = _post;
  winsize = pre+post;
}

int cWindowProcessor::configureWriter(sDmLevelConfig &c)
{
  // blocksize should not be larger than c->nT / 2
/*
if (blocksize >= c->nT/2) {
    blocksize = c->nT / 2 - 1;
    SMILE_MSG(3,"auto-adjusting blocksize to %i",blocksize);
  }
  if (blocksize < 1) blocksize = 1;
*/  

  /*
  if (!writer->isManualConfigSet()) {
    long bs;
    if (buffersize > 0) bs = buffersize;
    else bs=c->nT;
    if (bs < 2*blocksize+1) bs=2*blocksize+1;
    writer->setConfig(c->isRb, bs, c->T, c->lenSec, c->frameSizeSec, c->growDyn, c->type, 0);
  } else {
    if (writer->getBuffersize() < 2*blocksize+1) writer->setBuffersize(2*blocksize+1);
  }
*/

  if (blocksizeW_ != blocksizeR_) {
    //blocksizeW = blocksizeR;
    blocksizeW_sec_ = blocksizeR_sec_;
    c.blocksizeWriter = blocksizeR_;
  }

  if (winsize < pre+post) winsize = pre+post;
  // TODO: seconds/frames??
  reader_->setupSequentialMatrixReading(blocksizeR_,blocksizeR_+winsize,-pre); // winsize = pre + post

  return 1;
}


// order is the amount of memory (overlap) that will be present in _buf
// buf will have nT timesteps, however also order negative indicies (i.e. you may access a negative array index up to -order!)
int cWindowProcessor::processBuffer(cMatrix *_in, cMatrix *_out, int _pre, int _post )
{

  // return value: 
  //   0: error, myTick will return 0, and no frame will be set
  //   1: ok, next frame will be set
  //   -1: ok, myTick will return 1, but no new frame will be set 
  //          (use this for analysers, for example)
  return 0;
}

// overwrite this method in your derived class if you need to know for which input element (matrix row) 
// processBuffer was called (-> rowGlobal)
int cWindowProcessor::processBuffer(cMatrix *_in, cMatrix *_out, int _pre, int _post, int rowGlobal )
{

  // return value: 
  //   0: error, myTick will return 0, and no frame will be set
  //   1: ok, next frame will be set
  //   -1: ok, myTick will return 1, but no new frame wcMatrix * rowout;ill be set
  //          (use this for analysers, for example)
  return 0;
}

int cWindowProcessor::dataProcessorCustomFinalise()
{
  Ni = reader_->getLevelN();
  return 1;
}

/*
int cWindowProcessor::myFinaliseInstance()
{
  int ret=1;
  ret *= reader->finaliseInstance();
  if (!ret) return ret;

  
  //ok? TODO: blocksize < order????  more or less neg start offset... zero pad, => i.e. ignMissingBegin option in reader!!
  return cDataProcessor::myFinaliseInstance();
}
*/

int cWindowProcessor::myTick(long long t)
{
  SMILE_IDBG(4,"tick # %i, running window processor",t);
  if ((isEOI())&&(noPostEOIprocessing)) return 0;

  int ret = 1;
  
  if (!(writer_->checkWrite(blocksizeW_))) return 0;

  // get next block from dataMemory
  cMatrix *mat = reader_->getNextMatrix();
  // TODO: if blocksize< order!! also check if we need to increase the read counter!
  if (mat != NULL) {

    int i,j,toSet=0;
    if (matnew == NULL) {
      matnew = new cMatrix(mat->N*multiplier, mat->nT-winsize,mat->type);
//      printf("XXXaa matnew N = %i, mult = %i, matN = %i, matNt %i, ws %i\n",matnew->N,multiplier,mat->N,mat->nT,winsize);
    }

    // TODO: support multiplier for N output rows for each input row!
    if (rowsout == NULL) rowsout = new cMatrix(multiplier, mat->nT-winsize, mat->type);
    if (multiplier > 1 && rowout == NULL) rowout = new cMatrix(1, mat->nT-winsize, mat->type);
    if (row == NULL) row = new cMatrix(1,mat->nT, mat->type);
    for (i=0; i<mat->N; i++)  {
      // get matrix row...
      cMatrix *rowr = mat->getRow(i,row);
      if (rowr==NULL) COMP_ERR("cWindowProcessor::myTick : Error getting row %i from matrix! (return obj = NULL!)",i);
      if (row->dataF != NULL) row->dataF += pre;
      if (row->dataI != NULL) row->dataI += pre;
      row->nT -= winsize;
      toSet = processBuffer(row, rowsout, pre, post);
      if (toSet == 0) toSet = processBuffer(row, rowsout, pre, post, i);
      if (!toSet) ret=0;
      // copy row back into new matrix... ( NO overlap!)
      if (toSet==1) {
        if (multiplier > 1) {
          for (j=0; j<multiplier; j++) {
            rowsout->getRow(j,rowout);
            matnew->setRow(i*multiplier+j,rowout);
          }
        } else {
          matnew->setRow(i,rowsout);
        }
      }
      if (row->dataF != NULL) row->dataF -= pre;
      if (row->dataI != NULL) row->dataI -= pre;
      row->nT += winsize;
    }
    // set next matrix...
    if (toSet==1)  {
      mat->tmeta += pre; // TODO::: skip "order" elements of tmeta array ..ok?
      matnew->tmetaReplace(mat->tmeta); 
      mat->tmeta -= pre;
      writer_->setNextMatrix(matnew);
    }
  } else {
//         printf("WINPROC '%s' mat==NULL tickNr=%i EOI=%i\n",getInstName(),t,isEOI());
    return 0;
  }

  isFirstFrame = 0;
  
  return ret;

}



cWindowProcessor::~cWindowProcessor()
{
  if (row != NULL) delete row;
  if (rowout != NULL) delete rowout;
  if (rowsout != NULL) delete rowsout;
  if (matnew != NULL) delete matnew;
}

