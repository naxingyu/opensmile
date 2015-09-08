/*F***************************************************************************
 * openSMILE - the open-Source Multimedia Interpretation by Large-scale
 * feature Extraction toolkit
 * 
 * (c) 2008-2011, Florian Eyben, Martin Woellmer, Bjoern Schuller: TUM-MMK
 * 
 * (c) 2012-2013, Florian Eyben, Felix Weninger, Bjoern Schuller: TUM-MMK
 * 
 * (c) 2013-2014 audEERING UG, haftungsbeschränkt. All rights reserved.
 * 
 * Any form of commercial use and redistribution is prohibited, unless another
 * agreement between you and audEERING exists. See the file LICENSE.txt in the
 * top level source directory for details on your usage rights, copying, and
 * licensing conditions.
 * 
 * See the file CREDITS in the top level directory for information on authors
 * and contributors. 
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
