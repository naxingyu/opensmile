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

The SEMAINE dimensional affect/emotion sender

*/


#ifndef __CSEMAINEEMOTIONSENDER_HPP
#define __CSEMAINEEMOTIONSENDER_HPP

#include <core/smileCommon.hpp>

#ifdef HAVE_SEMAINEAPI

#include <core/smileComponent.hpp>
#include <semaine/semaineEmmaSender.hpp>
#include <classifiers/libsvmliveSink.hpp>

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

#define COMPONENT_DESCRIPTION_CSEMAINEEMOTIONSENDER "the SEMAINE Emma message sender for sending keywords (ASR output)"
#define COMPONENT_NAME_CSEMAINEEMOTIONSENDER "cSemaineEmotionSender"

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
class cSemaineEmotionSender : public cSemaineEmmaSender {
  private:
    const char *intensityStr;
    const char *unpredictabilityStr;

    void sendInterestC( cComponentMessage *_msg );

    void sendDimensionFSRE( cComponentMessage *_msg, std::string dimensionStr )
    {
      sendDimension( _msg , dimensionStr, EmotionML::VOC_FSRE_DIMENSION_DEFINITION );
    }

    void sendDimension( cComponentMessage *_msg, std::string dimensionStr, std::string dimensionVoc );
    void sendDimensionsFSRE_I( cComponentMessage *_msg );

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    virtual int processComponentMessage( cComponentMessage *_msg );

   
  public:
    SMILECOMPONENT_STATIC_DECL

    cSemaineEmotionSender(const char *_name);
    virtual ~cSemaineEmotionSender() {}

};


#endif // HAVE_SEMAINEAPI

#endif // __CSEMAINEEMOTIONSENDER_HPP
