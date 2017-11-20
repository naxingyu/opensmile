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

dataSource
write data to data memory...

*/


#include <core/dataSource.hpp>

#define MODULE "cDataSource"


SMILECOMPONENT_STATICS(cDataSource)

SMILECOMPONENT_REGCOMP(cDataSource)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CDATASOURCE;
  sdescription = COMPONENT_DESCRIPTION_CDATASOURCE;

  // configure your component's configType:
  SMILECOMPONENT_CREATE_CONFIGTYPE
  if (ct->setField("writer", "The configuration of the cDataWriter subcomponent, which handles the dataMemory interface for data output",
                  sconfman->getTypeObj("cDataWriter"), NO_ARRAY, DONT_FREE) == -1) {
     rA=1; // if subtype not yet found, request , re-register in the next iteration
  }

  ct->setField("buffersize", "The buffer size for the output level in frames (this overwrites buffersize_sec)", 0,0,0);
  ct->setField("buffersize_sec", "The buffer size for the output level in seconds", 0.0);
  ct->setField("blocksize", "A size of data blocks to write at once, in frames (same as blocksizeW for source only components, this overwrites blocksize_sec, if set)", 0,0,0);
  ct->setField("blocksizeW", "The size of data blocks to write in frames (this overwrites blocksize and blocksize_sec, if it is set) (this option is provided for compatibility only... it is exactly the same as 'blocksize')", 0,0,0);
  ct->setField("blocksize_sec", "The size of data blocks to write at once, in seconds", 0.0);
  ct->setField("blocksizeW_sec", "The size of data blocks to write at once, in seconds (this option overwrites blocksize_sec!) (this option is provided for compatibility only... it is exactly the same as 'blocksize')", 0.0,0,0);
  ct->setField("period", "(optional) The period of the input frames (in seconds), if it cannot be determined from the input file format. (if set and != 0, this will overwrite any automatically set values, e.g. from samplerate and writer.levelconf etc. (NOTE: this behaviour also depends on the source component, whether it overwrites period in configureWriter() or whether it keeps the period if it is != 0.0))", 0.0,0,0);
  ct->setField("basePeriod", "(optional) The base period of the input data (in seconds). The default -1.0 will force the code to use the value of period (unless overwritten by configureWriter().)", -1.0);
  SMILECOMPONENT_IFNOTREGAGAIN( {} )
  SMILECOMPONENT_MAKEINFO_ABSTRACT(cDataSource);
}

SMILECOMPONENT_CREATE_ABSTRACT(cDataSource)

//-----

cDataSource::cDataSource(const char *_name) :
  cSmileComponent(_name),
  writer_(NULL),
  vec_(NULL),
  mat_(NULL),
  blocksizeW_(1),
  blocksizeW_sec(0.0),
  buffersize_(10),
  buffersize_sec_(0.0),
  period_(0.0),
  basePeriod_(0.0),
  namesAreSet_(0)
{
  char *tmp = myvprint("%s.writer",getInstName());
  writer_ = (cDataWriter *)(cDataWriter::create(tmp));
  if (writer_ == NULL) {
    COMP_ERR("Error creating dataWriter '%s'",tmp);
  }
  if (tmp!=NULL) free(tmp);
}

void cDataSource::mySetEnvironment()
{
  writer_->setComponentEnvironment(getCompMan(), -1, this);
}

void cDataSource::allocVec(int n)
{
  if (vec_ != NULL) { delete vec_; }
  vec_ = new cVector(n);
}

void cDataSource::allocMat(int n, int t)
{
  if (mat_ != NULL) { delete mat_; }
  mat_ = new cMatrix(n,t);
}

void cDataSource::fetchConfig()
{
  buffersize_sec_ = getDouble("buffersize_sec");
  SMILE_IDBG(2, "buffersize (sec.) = %f", buffersize_sec_);
  buffersize_ = getInt("buffersize");
  SMILE_IDBG(2, "buffersize (frames.) = %i", buffersize_);
  blocksizeW_sec = getDouble("blocksize_sec");
  if ( (blocksizeW_sec <= 0.0) || (isSet("blocksizeW_sec")) ) {
    blocksizeW_sec = getDouble("blocksizeW_sec");
  }
  SMILE_IDBG(2, "blocksizeW (sec.) = %f", blocksizeW_sec);
  blocksizeW_ = getInt("blocksize");
  if ( (blocksizeW_ <= 0) || (isSet("blocksizeW")) ) {
    blocksizeW_ = getInt("blocksizeW");
  }
  SMILE_IDBG(2, "blocksizeW (frames, from config only) = %i", blocksizeW_);
  period_ = getDouble("period");
  basePeriod_ = getDouble("basePeriod");  
}

int cDataSource::myRegisterInstance(int *runMe)
{
  int ret = writer_->registerInstance();
  if ((ret)&&(runMe!=NULL)) {
    // call runMe config hook
    *runMe = runMeConfig();
  }
  return ret;
}

int cDataSource::myConfigureInstance()
{
// TODO: set writer config
// if (!writer->isManualConfigSet()) {
//     writer->setConfig(1,2*buffersize,T, 0.0, 0.0 , 0, DMEM_FLOAT);  // lenSec = (double)(2*buffersize)*T
// }
  sDmLevelConfig c2;

  c2.T = period_;
  c2.blocksizeWriter = -1;
  c2.basePeriod = basePeriod_; //-1.0;
  
  // provide a hook, to allow a derived component to modify the writer config we have obtained from the parent level:
  int ret = configureWriter(c2);
  // possible return values (others will be ignored): 
  // -1 : configureWriter has overwritten c->nT value for the buffersize, myConfigureInstance will not touch nT !
  // 0 : failure, myConfigure must exit with code 0
  // 1 : default / success
  if (!ret) {
    SMILE_IERR(1,"configureWriter() returned 0 (failure)!");
    return 0;  
  }

  // since this component is a source, it sets the base period
  // if it was not set by configureWriter, we have to set it to T here! 
  if (c2.basePeriod == -1.0) 
    c2.basePeriod = c2.T;

  // convert blocksize options, so all options are accessible, if possible:
  // 1. blocksize values in frames override those in seconds:
  // 2. now do the inverse...
  if (blocksizeW_ > 0) {
    blocksizeW_sec = (double)blocksizeW_ * c2.T;
  } else if ((blocksizeW_sec > 0.0)&&(c2.T != 0.0)) {
    blocksizeW_ = (long) ceil (blocksizeW_sec / c2.T);
  } else {
    SMILE_IDBG(3,"using fallback blocksize of 1, because blocksize or blocksize_sec was not set in config!");
    blocksizeW_ = 1;
  }

  if (c2.blocksizeWriter == -1) { // if configureWriter has not modified the blocksize ...
    // get writer blocksize from "blocksizeW" config options
    c2.blocksizeWriter = blocksizeW_;
  } else {
    blocksizeW_ = c2.blocksizeWriter;
  }

  if (ret!=-1) {
    if (buffersize_ > 0) {
      c2.nT = buffersize_;
    } else {
      if (buffersize_sec_ > 0.0) {
        if (c2.T != 0.0) {
          c2.nT = (long)ceil(buffersize_sec_/c2.T);
        } else {
          c2.nT = (long)ceil(buffersize_sec_);
        }
      } else {
        SMILE_IDBG(3,"using default buffersize 100 because buffersize or buffersize_sec was not set in config!");
        c2.nT = 100;
      }
    }
  }

  writer_->setConfig(c2,0);

  return writer_->configureInstance();
}

int cDataSource::myFinaliseInstance()
{
  // hook, to allow component to set data element names:
  if (!setupNewNames(0)) {
    SMILE_IERR(1,"setupNewNames() returned 0 (failure)!");
    return 0;
  }
  return writer_->finaliseInstance();
}

cDataSource::~cDataSource()
{
  if (writer_ != NULL) { delete writer_; }
  if (vec_ != NULL) { delete vec_; }
  if (mat_ != NULL) { delete mat_; }
}

