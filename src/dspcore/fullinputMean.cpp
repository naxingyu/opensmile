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


*/


#include <dspcore/fullinputMean.hpp>

#define MODULE "cFullinputMean"

SMILECOMPONENT_STATICS(cFullinputMean)

SMILECOMPONENT_REGCOMP(cFullinputMean)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CFULLINPUTMEAN;
  sdescription = COMPONENT_DESCRIPTION_CFULLINPUTMEAN;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")
  
    SMILECOMPONENT_IFNOTREGAGAIN( {}
    //ct->setField("expandFields", "expand fields to single elements, i.e. each field in the output will correspond to exactly one element in the input [not yet implemented]", 0);
    //ct->setField("htkcompatible", "1 = HTK compatible mean subtraction (this ignores (i.e. does not subtract the mean of) the 0th MFCC or energy field (and its deltas) specified by the option 'idx0')", 0);
    //ct->setField("idx0", "if > 0, index of field to ignore for mean subtraction (attention: 1st field is 0)", 0);
    ct->setField("meanNorm", "Type of mean normalisation: amean, rqmean, absmean  (arithmetic, root squared/quadratic, absolute value mean).", "amean");
    ct->setField("symmSubtract", "1 = Perform symmetric subtraction of rqmean or absmean u. I.e. for negative values add u and for positive values subtract u.", 0);
    ct->setField("subtractClipToZero", "1 = If symmSubtract is enabled and a value would change sign, clip it to 0. Otherwise, clip negative values to 0 when subtracting any kind of mean. 0 = do nothing special.", 0);
    ct->setField("specEnorm", "performs spectral magnitude energy normalisation", 0);
    ct->setField("htkLogEnorm","performs HTK compatible energy normalisation on all input fields instead of the default action of mean subtraction. The energy normalisation subtracts the maximum value of each value in the sequence and adds 1.",0);
    ct->setField("multiLoopMode", "1 = Support the new tick loop mode which can have unlimited EOI iterations. In this mode the means will be collected until the EOI condition is signalled. During the EOI condition nothing will be done (except computing means from the remaining data). During the next non EOI condition, the means will be subtracted from the old input, and (if new data is available - e.g. from a next segment) a new set of means will be computed. If old and new data is processed, the cycle begins anew. If this option is disabled, the means are subtracted while the first EOI condition is signalled. This is for compatibility with old behaviour of the tick loop.", 0);
    ct->setField("printMeans", "1 = print the mean vector once it has been computed.", 0);
  )

  SMILECOMPONENT_MAKEINFO(cFullinputMean);
}

SMILECOMPONENT_CREATE(cFullinputMean)


// TODO:
// option to do something with the means:
//   print
//   write to second level repeatedly ("splitter")
//   write to second level once

//-----

cFullinputMean::cFullinputMean(const char *_name) :
  cDataProcessor(_name),
  print_means_(0),
  first_frame_(1),
  reader_pointer_(0), reader_pointer2_(0),
  multi_loop_mode_(0),
  mean_type_(MEANTYPE_AMEAN),
  spec_enorm_(0),
  symm_subtract_(0),
  symm_subtract_clip_to_zero_(0),
  flag_(0),
  eoi_flag_(0),
  means_(NULL), means2_(NULL),
  n_means_(0), n_means2_(0)
{
}

void cFullinputMean::fetchConfig()
{
  cDataProcessor::fetchConfig();

  int e_norm_mode = (int)getInt("htkLogEnorm");
  print_means_ = (int)getInt("printMeans");
  multi_loop_mode_ = (int)getInt("multiLoopMode");
  symm_subtract_ = (int)getInt("symmSubtract");
  symm_subtract_clip_to_zero_ = (int)getInt("subtractClipToZero");
  spec_enorm_ = (int)getInt("specEnorm");
  const char * s = getStr("meanNorm");
  if (s != NULL) {
    if (!strncmp(s, "rqm", 3)) {
      mean_type_ = MEANTYPE_RQMEAN;
    } else if (!strncmp(s, "ame", 3)) {
      mean_type_ = MEANTYPE_AMEAN;
    } else if (!strncmp(s, "absm", 4)) {
      mean_type_ = MEANTYPE_ABSMEAN;
    } else {
      COMP_ERR("Unknown mean type set for option 'meanNorm'. See the help (-H) for supported types.");
    }
  }
  if (e_norm_mode) {
    mean_type_ = MEANTYPE_ENORM;
  }
}

// Reads new data, computes stats in means_ and n_means_.
int cFullinputMean::readNewData()
{
  cVector *vec = reader_->getNextFrame();
  if (vec != NULL) {
    if (vec->type != DMEM_FLOAT) { COMP_ERR("only float data-type is supported by cFullinputMean!"); }
    if (means_ == NULL) {
      means_ = new cVector(vec->N, vec->type);
      for (int i = 0; i < vec->N; i++) {
        means_->dataF[i] = vec->dataF[i];
      }
      n_means_ = 1;
    } else {
      if (mean_type_ == MEANTYPE_ENORM) {
        for (int i = 0; i < vec->N; i++) {
          if (vec->dataF[i] > means_->dataF[i]) means_->dataF[i] = vec->dataF[i];
        }
      } else if (mean_type_ == MEANTYPE_AMEAN) {
        for (int i = 0; i < vec->N; i++) {
          means_->dataF[i] += vec->dataF[i];
        }
        n_means_++;
      } else if (mean_type_ == MEANTYPE_RQMEAN) {
        for (int i = 0; i < vec->N; i++) {
          means_->dataF[i] += vec->dataF[i] * vec->dataF[i];
        }
        n_means_++;
      } else if (mean_type_ == MEANTYPE_ABSMEAN) {
        for (int i = 0; i < vec->N; i++) {
          means_->dataF[i] += (FLOAT_DMEM)fabs(vec->dataF[i]);
        }
        n_means_++;
      }
    }
    return 1;
  }
  return 0;
}

void cFullinputMean::meanSubtract(cVector *vec)
{
  if (mean_type_ == MEANTYPE_ENORM) {
    for (int i = 0; i < means2_->N; i++) {
      vec->dataF[i] -= means2_->dataF[i] - (FLOAT_DMEM)1.0;
    }
  } else if (mean_type_ == MEANTYPE_AMEAN) {
    for (int i = 0; i < means2_->N; i++) {
      vec->dataF[i] -= means2_->dataF[i];
      if (symm_subtract_clip_to_zero_ && vec->dataF[i] < 0) vec->dataF[i] = 0.0;
    }
  } else if (mean_type_ == MEANTYPE_RQMEAN || mean_type_ == MEANTYPE_ABSMEAN) {
    if (symm_subtract_) {
      for (int i = 0; i < means2_->N; i++) {
        if (vec->dataF[i] >= 0) {
          vec->dataF[i] -= means2_->dataF[i];
        } else {
          vec->dataF[i] += means2_->dataF[i];
        }
      }
    } else if (symm_subtract_clip_to_zero_) {
      for (int i = 0; i < means2_->N; i++) {
        if (vec->dataF[i] >= means2_->dataF[i]) {
          vec->dataF[i] -= means2_->dataF[i];
        } else {
          if (vec->dataF[i] <= -means2_->dataF[i]) {
            vec->dataF[i] += means2_->dataF[i];
          } else {
            vec->dataF[i] = 0.0;
          }
        }
      }
    } else {
      for (int i = 0; i < means2_->N; i++) {
        vec->dataF[i] -= means2_->dataF[i];
      }
    }
  }
}

int cFullinputMean::finaliseMeans()
{
  if (mean_type_ != MEANTYPE_ENORM) {
    if (n_means_ > 0) {
      FLOAT_DMEM nM = (FLOAT_DMEM)n_means_;
      for (int i = 0; i < means_->N; i++) {
        means_->dataF[i] /= nM;
      }
      if (print_means_) {
        for (int i = 0; i < means_->N; i++) {
          SMILE_PRINT("means[%i] = %f  (n = %ld)", i, means_->dataF[i], n_means_);
        }
      }
    }
  }
  // copy means_ to means2_
  if (means2_ != NULL) {
    delete means2_;
  }
  // swap
  means2_ = means_;
  means_ = NULL;
  n_means2_ = n_means_;
  n_means_ = 0;
  return n_means2_;
}

int cFullinputMean::doMeanSubtract()
{
  if (writer_->checkWrite(1)) {
    cVector * vec = reader_->getFrame(reader_pointer2_);
    if (vec != NULL) {
      // TODO: make sure we can leave our read pointer (in the data memory) behind to ensure that the data does not get overwritten!!
      // NOTE: the easiest way to do this is to add a second data reader...
      //       the best way is to add multi read pointer functionality to the dataReader class
      meanSubtract(vec);
      writer_->setNextFrame(vec);
      reader_pointer2_++;
      return 1;
    } // TODO: else { // find out if the frame is lost, if so, skip up to next readable frame and print error message }
  }
  return 0;
}

int cFullinputMean::myTick(long long t)
{
  long i;
  if (multi_loop_mode_) {
    if (isEOI()) {
      if (!multi_loop_mode_) {
        // in non-multiloop mode, du mean subtraction here for backwards compatibility...
        if (eoi_flag_ == 0) {
          finaliseMeans();
          eoi_flag_ = 1;
        }
        if (n_means2_ > 0) {
          return doMeanSubtract();
        } else {
          SMILE_IERR(1, "No mean data available, no input was read in the first tick loop.");
          return 0;
        }
      } else {
        eoi_flag_ = 1;
        // read the remaining parts of the old data until no data can be read...
        return readNewData();
      }
    } else {
      int ret = 0;
      if (eoi_flag_) {
        if (eoi_flag_ == 1) {
          // the real end of the data input
          // normalise means_
          finaliseMeans();
          // set reader0 pointer...
          reader_pointer2_ = reader_pointer_;
          eoi_flag_ = 2;
          first_frame_ = 1;
        }
        // we've got data to process (mean subtract, etc., use means2_ here)
        if (means2_ != NULL) {
          ret |= doMeanSubtract();
        }
      }
      if (first_frame_) {
        // the current read pointer, save it for later, when we need to extract means here
        reader_pointer_ = reader_->getCurR();
        first_frame_ = 0;
      }
      // TODO: set reader pointer here to reader current vidx  on first read, or first read after EOI condition
      // Reads new data, if available and update means.
      //reader_pointer2_ = reader_->getCurR();
      //doMeanSubtract();
      ret |= readNewData();
      // Returns 1 only if at least one of new or old data frames has been read.
      return (ret > 0);
    }
  } else {  // old code for compatibility...
  // TODO: remove this code and test if the above will do the same...

  if (isEOI()) {
    if (means_ == NULL) {
      SMILE_IWRN(1,"sequence too short, cannot compute statistics (mean or max value)!");
      long N = reader_->getLevelN();
      means_ = new cVector( N, DMEM_FLOAT );
      for (i=0; i<N; i++) {
        means_->dataF[i] = 0;
      }
      n_means_ = 1;
    }

    if (flag_==0) { 
      reader_->setCurR(0); flag_ = 1; 
      if (mean_type_ != MEANTYPE_ENORM) {
        FLOAT_DMEM nM = (FLOAT_DMEM)n_means_;
        if (nM <= 0.0) nM = 1.0;
        for (i=0; i<means_->N; i++) {
          means_->dataF[i] /= nM;
        }
      }
    }
    cVector *vec = reader_->getNextFrame();
    if (vec!= NULL) {
      if (mean_type_ == MEANTYPE_ENORM) {
        for (i=0; i<means_->N; i++) {
          vec->dataF[i] -= means_->dataF[i] - (FLOAT_DMEM)1.0;  // x - max + 1 
        }
      } else {
        for (i=0; i<means_->N; i++) {
          vec->dataF[i] -= means_->dataF[i];
        }
      }
      writer_->setNextFrame(vec);
      return 1;
    } 
  } else {
    // compute means, do not write data
    cVector *vec = reader_->getNextFrame();
    if (vec != NULL) {
      if (vec->type != DMEM_FLOAT) { COMP_ERR("only float data-type is supported by cFullinputMean!"); }

      if (means_ == NULL) {
        means_ = new cVector( vec->N, vec->type );
        for (i=0; i<vec->N; i++) {
          means_->dataF[i] = vec->dataF[i];
        }
        n_means_ = 1;
      } else {
        if (mean_type_ == MEANTYPE_ENORM) {
          for (i=0; i<vec->N; i++) {
            if (vec->dataF[i] > means_->dataF[i]) means_->dataF[i] = vec->dataF[i];
          }
        } else {
          for (i=0; i<vec->N; i++) {
            means_->dataF[i] += vec->dataF[i];
          }
          n_means_++;
        }
      }
      return 1;
    }
    
  }

  }
  return 0;
}


cFullinputMean::~cFullinputMean()
{
  if (means_ != NULL) delete means_;
  if (means2_ != NULL) delete means2_;
}

