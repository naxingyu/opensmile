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

the SEMAINE Emma message sender for openSMILE


TODO: move to SEMAINExtract project ... ?

*/


#ifndef __CSEMAINEEMMASENDER_HPP
#define __CSEMAINEEMMASENDER_HPP

#include <core/smileCommon.hpp>

#ifdef HAVE_SEMAINEAPI

#include <core/smileComponent.hpp>
#ifndef _MSC_VER
#include <semaine/config.h>
#endif
#include <cms/CMSException.h>
#include <semaine/components/Component.h>
#include <semaine/cms/sender/EmmaSender.h>

#include <semaine/util/XMLTool.h>
#include <semaine/datatypes/xml/EMMA.h>
#include <semaine/datatypes/xml/EmotionML.h>
#include <semaine/datatypes/xml/SemaineML.h>

#define COMPONENT_DESCRIPTION_CSEMAINEEMMASENDER "the SEMAINE Emma message sender for openSMILE"
#define COMPONENT_NAME_CSEMAINEEMMASENDER "cSemaineEmmaSender"

using namespace semaine::cms::sender;
using namespace semaine::util;
using namespace semaine::datatypes::xml;
using namespace XERCES_CPP_NAMESPACE;

#define XERCESC_NS XERCES_CPP_NAMESPACE

/*
#define N_GENDER_HISTORY 14
#define GENDER_M  1
#define GENDER_F  2
*/

#undef class
class cSemaineEmmaSender : public cSmileComponent {
  private:
    EmmaSender *emmaSender;
    MetaMessenger *meta; 

    // gender majority vote
    /*
    int gender[N_GENDER_HISTORY];
    int gPtr;
*/

    //void sendArousalC( cComponentMessage *_msg );
    //void sendValenceC( cComponentMessage *_msg );
    //void sendInterestC( cComponentMessage *_msg );
    //void sendPowerC( cComponentMessage *_msg );
    //void sendIntensityC( cComponentMessage *_msg );
    //void sendAnticipationC( cComponentMessage *_msg );
    //void sendSingleEmotionDim( cComponentMessage *_msg, const char * set, std::string dim );
    /*
    void sendDimensionFSRE( cComponentMessage *_msg, std::string dimensionStr )
    {
      sendDimension( _msg , dimensionStr, EmotionML::VOC_FSRE_DIMENSION_DEFINITION );
    }
*/

//    void sendDimension( cComponentMessage *_msg, std::string dimensionStr, std::string dimensionVoc );
    //void sendSpeakingStatus( cComponentMessage *_msg, int status );
    //void sendGender( cComponentMessage *_msg );
    //void sendPitchDirection( cComponentMessage *_msg );
    //void sendKeywords( cComponentMessage *_msg );

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig() {}
    /*
    virtual void mySetEnvironment();
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    */
    virtual int myTick(long long t);
    //virtual int processComponentMessage( cComponentMessage *_msg );

    // get smileTime from _msg, convert to semaine time by getting current time from smile and semaine and observing the difference
    long long smileTimeToSemaineTime( double smileTime ) ;
    void sendDocument( XERCESC_NS::DOMDocument * document );
    
    EmmaSender * getEmmaSender() { return emmaSender; }

  public:
    SMILECOMPONENT_STATIC_DECL

    cSemaineEmmaSender(const char *_name);
  
    // this method sets the semaine emma sender to use... do not call it when the system is running, only call it during setup!!
    void setEmmaSender(EmmaSender *_sender, MetaMessenger *_meta=NULL) { emmaSender = _sender; meta=_meta; }

    virtual ~cSemaineEmmaSender();
};


#endif // HAVE_SEMAINEAPI

#endif // __CSEMAINEEMMASENDER_HPP
