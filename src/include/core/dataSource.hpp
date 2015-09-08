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
