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

Simple message sender for debugging and testing purposes. 
Reads data from an input level,
sends a message based on a threshold event in a selectable input channel, 
or sends all read data periodically as a message.

*/


#ifndef __SIMPLE_MESSAGE_SENDER_HPP
#define __SIMPLE_MESSAGE_SENDER_HPP

#include <core/smileCommon.hpp>
#include <core/dataSink.hpp>

#define COMPONENT_DESCRIPTION_CSIMPLEMESSAGESENDER "This is an example of a cDataSink descendant. It reads data from the data memory and prints it to the console. This component is intended as a template for developers."
#define COMPONENT_NAME_CSIMPLEMESSAGESENDER "cSimpleMessageSender"
#define BUILD_COMPONENT_SimpleMessageSender

#undef class

enum cSimpleMessageSender_Conditions {
  COND_UNDEF=0,
  COND_GTEQ=1, // trigger event, always when input greater or equal to the reference (threshold parameter)
  COND_GT=2,   // trigger event, always when greater
  COND_EQ=3,   // trigger event, always when equal
  COND_LE=4,   // trigger event, always when lesser
  COND_LEEQ=5, // trigger event, always when lesser or equal
  COND_GTEQ_S=101, // trigger event, only the first time the value is greater or equal
  COND_GT_S=102,   // trigger event, only the first time the value is greater
  COND_EQ_S=103,   // trigger event, only the first time the value is equal
  COND_LE_S=104,   // trigger event, only the first time the value is lesser
  COND_LEEQ_S=105, // trigger event, only the first time the value is lesser or equal
};

class DLLEXPORT cSimpleMessageSender : public cDataSink {
  private:
    const char *messageRecp;  // the comma separated list of message receipients (component names)
    const char *messageName;
    const char *messageType;
    bool showCustDataAsText, showCustData2AsText;
    bool sendPeriodically;        // Flag to enable/disable sending of periodic messages (sending of every value read from the input)
    bool enableDebugReceiver;     // Flag to enable/disable the debug print functionality for received(!) messages.
    bool enableDebugSender;       // Flag to enable/disable the debug print functionality for sent messages (before sending).
    const char *dataElementName;  // the name of the input element to send periodically or base event decisions on
    long dataElementIndex;
    FLOAT_DMEM threshold;         // the threshold for triggering an event
    cSimpleMessageSender_Conditions condition;  // the operation to apply to the element with dataElementName to trigger event based messages

    bool condFlag;   // used as state variable for the _S event conditions

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    // Implements a debug message receiver here. It prints all received messages to the log.
    virtual int processComponentMessage(cComponentMessage *_msg);

    void sendMessage(cComponentMessage *msg);
    void printMessage(cComponentMessage *msg);
    void sendPeriodicMessage(FLOAT_DMEM *v, int Nv, long vi, double tm);
    void sendEventMessage(FLOAT_DMEM *v, int Nv, const char * text, FLOAT_DMEM ref, long vi, double tm);
    void eventMessage(FLOAT_DMEM v, long vi, double tm);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cSimpleMessageSender (const char *_name);

    virtual ~cSimpleMessageSender();
};




#endif // __SIMPLE_MESSAGE_SENDER_HPP
