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


#ifndef __DATA_SOURCE_HPP
#define __DATA_SOURCE_HPP

#include <core/smileCommon.hpp>
#include <core/smileComponent.hpp>
#include <core/dataWriter.hpp>

#define COMPONENT_DESCRIPTION_CDATASOURCE "This is a base class for components, which write data to dataMemory, but do not read from it."
#define COMPONENT_NAME_CDATASOURCE "cDataSource"

#undef class
class DLLEXPORT cDataSource : public cSmileComponent {
  private:

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    cDataWriter *writer_;
    cVector     *vec_;    // current vector to be written
    cMatrix     *mat_;    // current matrix to be written
    double buffersize_sec_; /* buffersize of write level, as requested by config file (in seconds)*/
    double blocksizeW_sec; /* blocksizes for block processing (in seconds)*/
    long buffersize_; /* buffersize of write level, as requested by config file (in frames)*/
    long blocksizeW_; /* blocksizes for block processing (in frames)*/
    double period_; /* period, as set by config */
    double basePeriod_; 
    int namesAreSet_;

    virtual void fetchConfig();
    virtual void mySetEnvironment();
    virtual int myRegisterInstance(int *runMe=NULL);
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    void allocVec(int n);
    void allocMat(int n, int t);

    // *runMe return value for component manager : 0, don't call my tick of this component, 1, call myTick
    virtual int runMeConfig() { return 1; }
    virtual int configureWriter(sDmLevelConfig &c) { return 1; }
    virtual int setupNewNames(long nEl=0) { return 1; }

    virtual int setEOIcounter(int cnt) {
      int ret = cSmileComponent::setEOIcounter(cnt);
      if (writer_!=NULL) return writer_->setEOIcounter(cnt);
      return ret;
    }

    virtual void setEOI() {
      cSmileComponent::setEOI();
      if (writer_!=NULL) writer_->setEOI();
    }

    virtual void unsetEOI() {
      cSmileComponent::unsetEOI();
      if (writer_!=NULL) writer_->unsetEOI();
    }

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cDataSource(const char *_name);
    virtual ~cDataSource();
};




#endif // __DATA_SOURCE_HPP
