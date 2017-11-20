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


/*  openSMILE component: resmapler

resampling using fft and ideal sin/cos interpolation

*/


#include <dsp/smileResample.hpp>
#include <dspcore/fftXg.h>

#define MODULE "cSmileResample"


SMILECOMPONENT_STATICS(cSmileResample)

SMILECOMPONENT_REGCOMP(cSmileResample)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CSMILERESAMPLE;
  sdescription = COMPONENT_DESCRIPTION_CSMILERESAMPLE;

  // we inherit cWindowProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")

  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("targetRate","The target sampling frequency in Hz",16000);  
    ct->setField("resampleRatio","A fixed resample ratio a (a=fsNew/fsCurrent). If set, this overrides targetFs",1.0,0,0);
    ct->setField("pitchRatio","Low-quality pitch scaling factor, if != 1.0 ",1.0);
    ct->setField("useQuickAlgo","Use a quick algo for low-quality integer-ratio DOWN(!)sampling.",0);
    ct->setField("winSize","Internal window size in seconds (will be rounded to nearest power of 2 framesize internally). This affects the quality of the resampling and the accuracy of the target sampling rate. Larger window sizes allow for a more accurate target sampling frequency, i.e. less pitch distortion.",0.030);  
    // NOTE: blocksize will be determined by winSize
    //ct->setField("blocksize", "size of data blocks to process in frames", 0);
    ct->disableField("blocksize");
    ct->disableField("blocksizeR");
    ct->disableField("blocksizeW");
  )

  SMILECOMPONENT_MAKEINFO(cSmileResample);
}

SMILECOMPONENT_CREATE(cSmileResample)

//-----

cSmileResample::cSmileResample(const char *_name) :
  cDataProcessor(_name),   
  inputBuf(NULL), outputBuf(NULL), lastOutputBuf(NULL),
  resampleWork(NULL), rowout(NULL), matnew(NULL), row(NULL),
  flushed(0)
{
}


void cSmileResample::fetchConfig()
{
  cDataProcessor::fetchConfig();
  
   if (isSet("resampleRatio")) {
    resampleRatio = getDouble("resampleRatio");
    if (resampleRatio <= 0.0) {
      SMILE_IERR(1,"invalid resampling ratio (%f) ! must be > 0.0",resampleRatio);
      resampleRatio = 1.0;
    }
    SMILE_IDBG(2,"resampleRatio = '%s'",resampleRatio);
  } else {
    targetFs = getDouble("targetRate");
    if (targetFs <= 0.0) {
      SMILE_IERR(1,"invalid target sampling frequency (targetFs=%f) ! must be > 0.0",targetFs);
      targetFs = 1.0;
    }
    SMILE_IDBG(2,"targetRate = '%s'",targetFs);
    resampleRatio = -1.0;
  }

 pitchRatio = getDouble("pitchRatio");
 SMILE_IDBG(2,"pitchRatio = '%s'",pitchRatio);

 winSize = getDouble("winSize");
 SMILE_IDBG(2,"winSize = '%s'",winSize);

 useQuickAlgo = getInt("useQuickAlgo");
 SMILE_IDBG(2,"useQuickAlgo = %i",useQuickAlgo);

}

int cSmileResample::configureWriter(sDmLevelConfig &c)
{
  double bT = c.T;
  double sr;

  if (bT > 0.0) sr = 1.0/bT;
  else {
    SMILE_IERR(1,"unable to determine sample rate of input! basePeriod <= 0.0 (=%f)!",bT);
    sr = 1.0;
  }

  /* compute resampling parameters: */
  if (resampleRatio == -1.0) { // convert targetFs
    resampleRatio = targetFs/sr;
    SMILE_IDBG(2,"resampleRatio (computed) = %f",resampleRatio);
  } else {
    // compute targetFs from resampling ratio
    targetFs = resampleRatio * sr;
  }

  if (useQuickAlgo) {
    if (resampleRatio > 1.0) {
      COMP_ERR("cannot use quick resampling algo for upsampling!");
    }
    double rr = 1.0/resampleRatio;
    rr = round(rr); // round to nearest int...
    resampleRatio = 1.0/rr;
    SMILE_IDBG(2,"resampleRatio (rounded) = %f",resampleRatio);

    //TODO: winSizeFrames must be a multiple of the integer resampleRatio !
    winSizeFramesTarget = (long)ceil( (double)winSizeFrames * resampleRatio );
    winSizeTarget = (double)winSizeFramesTarget * bT;

  } else {


    // convert winSize (sec) to winSizeFrames:
    winSizeFrames = smileMath_ceilToNextPowOf2( (long)round(winSize/bT) );
    winSizeFramesTarget = (long)ceil( (double)winSizeFrames * resampleRatio );
    ND = (double)winSizeFrames * resampleRatio / pitchRatio;
    if (winSizeFramesTarget&1) winSizeFramesTarget = (long)floor( (double)winSizeFrames * resampleRatio ); // odd->even!
    if (winSizeFramesTarget&1) winSizeFramesTarget++;
    winSize = (double)winSizeFrames * bT;
    winSizeTarget = (double)winSizeFramesTarget * bT;
    SMILE_IMSG(3,"using actual frame size (pow 2): %i samples (%f seconds)",winSizeFrames,winSize);
    SMILE_IDBG(2,"target frame size: %i samples (%f seconds)",winSizeFramesTarget,winSizeTarget);
    double newRatio = (double)winSizeFramesTarget / (double)winSizeFrames;
    if (newRatio != resampleRatio) {
      SMILE_IMSG(2,"NOTE: actual output rate is targetRate* = %f (increase winSize for more accuracy!)",sr*newRatio);
    }

  }

  blocksizeR_ = winSizeFrames;
  blocksizeR_sec_ = winSize;
  if (useQuickAlgo) {
    blocksizeW_ = winSizeFramesTarget;
    blocksizeW_sec_ = winSizeTarget;
  } else {
    blocksizeW_ = winSizeFramesTarget/2;
    blocksizeW_sec_ = winSizeTarget/2.0;
  }

  c.blocksizeWriter = blocksizeW_;
  c.T = 1.0/targetFs;
  if (useQuickAlgo) {
    reader_->setupSequentialMatrixReading(winSizeFrames, winSizeFrames /* * pitchRatio */);
  } else {
    reader_->setupSequentialMatrixReading(winSizeFrames/2, winSizeFrames /* * pitchRatio */);
  }

  return 1;
}

int cSmileResample::dataProcessorCustomFinalise()
{
  double bT = reader_->getLevelT();
  double sr;

  if (bT > 0.0) sr = 1.0/bT;
  else {
    SMILE_IERR(1,"unable to determine sample rate of input! basePeriod <= 0.0 (=%f)!",bT);
    sr = 1.0;
  }

  // allocate buffers:
  Ni = reader_->getLevelN();
  outputBuf = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*Ni*winSizeFramesTarget);
  lastOutputBuf = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*Ni*(winSizeFramesTarget/2+1));
  inputBuf = (FLOAT_TYPE_FFT*)calloc(1,sizeof(FLOAT_TYPE_FFT)*Ni*winSizeFrames);

  return 1;
}



int cSmileResample::getOutput(FLOAT_DMEM *cur, FLOAT_DMEM *last, long N, FLOAT_DMEM *out, long Nout)
{
  long i;
  for (i=0; i<Nout; i++) {
    // the overlap add:
    out[i] = ( cur[i]+last[i] );
  }

  // cur N/2..N -> last
  for (i=N-Nout; i<N; i++) {
    last[i-(N-Nout)] = cur[i];
  }

  return 1;
}

int cSmileResample::myTick(long long t)
{
  long i,j;

  SMILE_IDBG(4,"tick # %i, running resampler",t);
  //if ((isEOI())&&(noPostEOIprocessing)) return 0;
  if ((isEOI())) {
    if (!flushed) {
    for (i=0; i<matnew->N; i++)  {
      FLOAT_DMEM *outBuf = outputBuf+i*winSizeFramesTarget;
      FLOAT_DMEM *lastOutBuf = lastOutputBuf + i*(winSizeFramesTarget/2+1);
      FLOAT_TYPE_FFT *inBuf = inputBuf+i*winSizeFrames;
      for (j=0; j<winSizeFramesTarget; j++) { outBuf[j]=0.0; }
      getOutput(outBuf, lastOutBuf, winSizeFramesTarget, rowout->dataF, rowout->nT);
      //rowout->dataF[0]= outBuf[0];
      matnew->setRow(i,rowout);
    }
    writer_->setNextMatrix(matnew);
    flushed = 1;
    return 1;
    } else { return 0; }
  }

  int ret = 1;
  
  if (!(writer_->checkWrite(blocksizeW_))) return 0;

  // get next block from dataMemory
  cMatrix *mat = reader_->getNextMatrix();
  
  // TODO: if blocksize< order!! also check if we need to increase the read counter!
  if (mat != NULL) {
    if (mat->type!=DMEM_FLOAT) COMP_ERR("dataType (%i) != DMEM_FLOAT not yet supported!",mat->type);

    // TODO: test quick resample algo here...!
    if (useQuickAlgo) {

      if (matnew == NULL) matnew = new cMatrix(mat->N, winSizeFramesTarget,DMEM_FLOAT);
      //if (rowout == NULL) rowout = new cMatrix(1,winSizeFramesTarget/2, DMEM_FLOAT);
      //if (row == NULL) row = new cMatrix(1,winSizeFrames, DMEM_FLOAT);
      long i,j,n,r=0;
      int rr = (int)(1.0/resampleRatio);
      if (rr<1) rr=1;
      for (i=0; i<mat->nT; i+=rr) {
        for (j=0; j<mat->N; j++) {
          FLOAT_DMEM *out = matnew->dataF + (r*matnew->N+j);
          *out = 0;
          for (n=0; n<rr; n++) {
            *out += mat->dataF[(i+n)*mat->N+j];
          }
          *out /= rr;
        }
        r++;
      }

    } else {
    
    /*
    if (row == NULL) row = new cMatrix(1,mat->nT, mat->type);
*/
    if (matnew == NULL) matnew = new cMatrix(mat->N, winSizeFramesTarget/2,DMEM_FLOAT);
    if (rowout == NULL) rowout = new cMatrix(1,winSizeFramesTarget/2, DMEM_FLOAT);
    if (row == NULL) row = new cMatrix(1,winSizeFrames, DMEM_FLOAT);

    for (i=0; i<mat->N; i++)  {
      FLOAT_DMEM *outBuf = outputBuf+i*winSizeFramesTarget;
      FLOAT_DMEM *lastOutBuf = lastOutputBuf + i*(winSizeFramesTarget/2+1);
      FLOAT_TYPE_FFT *inBuf = inputBuf+i*winSizeFrames/* *pitchRatio */;

      // get matrix row...
      cMatrix *rowr = mat->getRow(i,row);
      if (rowr==NULL) COMP_ERR("cWindowProcessor::myTick : Error getting row %i from matrix! (return obj = NULL!)",i);

      for (j=0; j<rowr->nT; j++) {
        inBuf[j] = (FLOAT_TYPE_FFT) rowr->dataF[j];
      }
      
      smileDsp_doResample(inBuf, row->nT, outBuf, winSizeFramesTarget, ND, &resampleWork);
      getOutput(outBuf, lastOutBuf, winSizeFramesTarget, rowout->dataF, rowout->nT);
      //rowout->dataF[0]= outBuf[0];
      matnew->setRow(i,rowout);
    }
    
    // set next matrix...
    //if (toSet==1)  {
      
    }
      //matnew->tmetaReplace(mat->tmeta); 
    // TODO: tmeta adjust...
      //mat->tmeta -= pre;
    writer_->setNextMatrix(matnew);
    //}
  } else {
//         printf("WINPROC '%s' mat==NULL tickNr=%i EOI=%i\n",getInstName(),t,isEOI());
    return 0;
  }

//  isFirstFrame = 0;
  
  return ret;

}



cSmileResample::~cSmileResample()
{
  if (inputBuf != NULL) free(inputBuf);
  if (outputBuf != NULL) free(outputBuf);
  if (lastOutputBuf != NULL) free(lastOutputBuf);
  if (row != NULL) delete row;
  if (rowout != NULL) delete rowout;
  if (matnew != NULL) delete matnew;
  smileDsp_resampleWorkFree(resampleWork);
}

