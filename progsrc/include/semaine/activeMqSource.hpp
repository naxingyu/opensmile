/*F***************************************************************************
 * openSMILE - the Munich open source Multimedia Interpretation by Large-scale
 * Extraction toolkit
 * 
 * (c) 2008-2011, Florian Eyben, Martin Woellmer, Bjoern Schuller @ TUM-MMK
 * (c) 2012-2013, Florian Eyben, Felix Weninger, Bjoern Schuller @ TUM-MMK (c)
 * 2013-2014 audEERING UG, haftungsbeschränkt. All rights reserved.
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

example dataSource
writes data to data memory...

*/


#ifndef __CACTIVEMQSOURCE_HPP
#define __CACTIVEMQSOURCE_HPP

#include <core/smileCommon.hpp>
#undef class

#ifdef HAVE_SEMAINEAPI

//#ifdef HAVE_SEMAINE_CONFIG_H  // <- this causes problems in linux
#ifndef _MSC_VER
#include <semaine/config.h>
#endif

#include <cms/CMSException.h>
#include <semaine/components/Component.h>
#include <semaine/cms/receiver/FeatureReceiver.h>

#include <core/dataSource.hpp>

#define COMPONENT_DESCRIPTION_CACTIVEMQSOURCE "dataSource which reads data vectors from the SEMAINE ActiveMQ server"
#define COMPONENT_NAME_CACTIVEMQSOURCE "cActiveMqSource"

class cActiveMqSource : public cDataSource {
  private:
    int nValues;
    float fStep;
    const char *vadRecv;

    MetaMessenger *meta; 
    int lag; int warned;
    int running;

    cVector *vec;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);
    
    virtual int setupNewNames(long nEl);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cActiveMqSource(const char *_name);
    cDataWriter * getWriter() { return writer; }

    cVector *getVec() { return vec; }

    // this method sets the semaine feature sender to use... do not call it when the system is running, only call it during setup!!
    //void setFeatureReceiver(FeatureReceiver *_receiver, MetaMessenger *_meta=NULL) { featureReceiver = _receiver; meta=_meta; }

    void waveFileReceive(void *data, long long N);

    int isRunning() { return running; }

    virtual ~cActiveMqSource();
};


#endif // HAVE_SEMAINEAPI

#endif // __CACTIVEMQSOURCE_HPP
