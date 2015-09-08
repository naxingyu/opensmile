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

example dataSink:
reads data from data memory and outputs it to console/logfile (via smileLogger)
this component is also useful for debugging

*/


#ifndef __CACTIVEMQSINK_HPP
#define __CACTIVEMQSINK_HPP

#include <core/smileCommon.hpp>
#undef class

//#define HAVE_SEMAINEAPI
#ifdef HAVE_SEMAINEAPI

//#ifdef HAVE_SEMAINE_CONFIG_H  // <- this causes problems in linux
#ifndef _MSC_VER
#include <semaine/config.h>
#endif

#include <cms/CMSException.h>
#include <semaine/components/Component.h>
#include <semaine/cms/sender/FeatureSender.h>

#include <core/dataSink.hpp>

#define COMPONENT_DESCRIPTION_CACTIVEMQSINK "activeMq dataSink, this component sends data as messages to an ActiveMQ server"
#define COMPONENT_NAME_CACTIVEMQSINK "cActiveMqSink"

class cActiveMqSink : public cDataSink {
private:
  FeatureSender *featureSender;   
  MetaMessenger *meta; 
  int lag; int warned;

protected:
  SMILECOMPONENT_STATIC_DECL_PR

  virtual void fetchConfig();
  //virtual int myConfigureInstance();
  //virtual int myFinaliseInstance();
  virtual int myTick(long long t);

public:
  SMILECOMPONENT_STATIC_DECL

  cActiveMqSink(const char *_name);
  cDataReader * getReader() { return reader; }

  // this method sets the semaine feature sender to use... do not call it when the system is running, only call it during setup!!
  void setFeatureSender(FeatureSender *_sender, MetaMessenger *_meta=NULL) { featureSender = _sender; meta=_meta; }

  virtual ~cActiveMqSink();
};


#endif // HAVE_SEMAINEAPI

#endif // __CACTIVEMQSINK_HPP
