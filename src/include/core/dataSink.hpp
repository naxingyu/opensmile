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

dataSink

*/


#ifndef __DATA_SINK_HPP
#define __DATA_SINK_HPP

#include <core/smileCommon.hpp>
#include <core/smileComponent.hpp>
#include <core/dataReader.hpp>

#define COMPONENT_DESCRIPTION_CDATASINK "This is a base class for components reading from (and not writing to) the dataMemory and dumping/passing data to external entities."
#define COMPONENT_NAME_CDATASINK "cDataSink"

#undef class
class DLLEXPORT cDataSink : public cSmileComponent {
  private:

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    int errorOnNoOutput_;
    long nWritten_;
    long blocksizeR_;
    double blocksizeR_sec_;
    cDataReader *reader_;

    virtual void fetchConfig();
    virtual void mySetEnvironment();
    virtual int myRegisterInstance(int *runMe=NULL);
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();

    // *runMe return value for component manager : 0, don't call my tick of this component, 1, call myTick
    virtual int runMeConfig() { return 1; }

    // Configures the reader, i.e. set blocksize requests etc.
    virtual int configureReader();

    // Proper handling of EOI events, passing them to the reader.
    virtual int setEOIcounter(int cnt) {
      int ret = cSmileComponent::setEOIcounter(cnt);
      if (reader_!=NULL) return reader_->setEOIcounter(cnt);
      return ret;
    }

    // Proper handling of EOI events, passing them to the reader.
    virtual void setEOI() {
      cSmileComponent::setEOI();
      if (reader_!=NULL) reader_->setEOI();
    }

    // Proper handling of EOI events, passing them to the reader.
    virtual void unsetEOI() {
      cSmileComponent::unsetEOI();
      if (reader_!=NULL) reader_->unsetEOI();
    }

  public:
    SMILECOMPONENT_STATIC_DECL

    cDataSink(const char *_name);
    virtual ~cDataSink();
};




#endif // __DATA_SINK_HPP
