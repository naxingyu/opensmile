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


#include <examples/simpleMessageSender.hpp>

#define MODULE "cSimpleMessageSender"


SMILECOMPONENT_STATICS(cSimpleMessageSender)

SMILECOMPONENT_REGCOMP(cSimpleMessageSender)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CSIMPLEMESSAGESENDER;
  sdescription = COMPONENT_DESCRIPTION_CSIMPLEMESSAGESENDER;

  // we inherit cDataSink configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSink")
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("messageRecp", "A comma separated list of message receipients (component names). If you leave this empty, no messages will be sent.", (const char *)NULL);
    ct->setField("messageName", "The name of the message that will be sent.", "demo_message");
    ct->setField("messageType", "The type of the message that will be sent.", "simpleMessage");
    ct->setField("dataElementName", "The name of the input level data element to send periodically or base the event decisions on. If left empty, the first element will be used.", (const char *)NULL);
    ct->setField("sendPeriodically", "1 = Enable sending of periodic messages for each sample of the input data element. 2 = ignore dataElementName and send up to the first 8 floats in the input data in the float data message array.", 0);
    ct->setField("enableDebugReceiver", "1/0 = enable/disable the debug print functionality for received(!) messages.", 1);
    ct->setField("enableDebugSender", "1/0 = enable/disable the debug print functionality for sent messages (before sending).", 1);
    ct->setField("showCustDataAsText", "1/0 = enable/disable printing of (non NULL) custData field as text string in debug message logs.", 0);
    ct->setField("showCustData2AsText", "1/0 = enable/disable printing of (non NULL) custData2 field as text string in debug message logs.", 0);
    ct->setField("threshold", "The threshold for triggering an event on the input data element.", 0.0);
    ct->setField("condition", "The condition to apply to the element with dataElementName to trigger event based messages. Choose one of the following: eq, gteq, gt, leeq, le for the conditions =, >=, >, <=, <. The event will be triggered always when the condition is met. Add the suffix _s to the condition name, to trigger the event only the first time the condition changes from false to true. The condition is applied as: <input_value> <cond> <threshold>.", "eq");
  )

  SMILECOMPONENT_MAKEINFO(cSimpleMessageSender);
}

SMILECOMPONENT_CREATE(cSimpleMessageSender)

//-----

cSimpleMessageSender::cSimpleMessageSender(const char *_name) :
  cDataSink(_name),
  condition(COND_UNDEF), sendPeriodically(false),
  enableDebugReceiver(false), enableDebugSender(false),
  dataElementIndex(0), dataElementName(NULL),
  messageRecp(NULL), condFlag(false),
  showCustDataAsText(false), showCustData2AsText(false)
{
  // ...
}

void cSimpleMessageSender::fetchConfig()
{
  cDataSink::fetchConfig();
  messageRecp = getStr("messageRecp");
  messageName = getStr("messageName");
  messageType = getStr("messageType");
  dataElementName = getStr("dataElementName");
  int tmp = getInt("sendPeriodically");
  if (tmp) {
    sendPeriodically = true;
  }
  if (tmp == 2) {
    dataElementIndex = -1;
  }
  if (getInt("enableDebugReceiver")) {
    enableDebugReceiver = true;
  }
  if (getInt("enableDebugSender")) {
    enableDebugSender = true;
  }
  if (getInt("showCustDataAsText")) {
    showCustDataAsText = true;
  }
  if (getInt("showCustData2AsText")) {
    showCustDataAsText = true;
  }
  threshold = (FLOAT_DMEM)getDouble("threshold");

  const char *cond = getStr("condition");
  if (!strncasecmp(cond, "eq", 2)) {
    if (!strncasecmp(cond, "eq_s", 4)) {
      condition = COND_EQ_S;
    } else {
      condition = COND_EQ;
    }
  } else if (!strncasecmp(cond, "gteq", 4)) {
    if (!strncasecmp(cond, "gteq_s", 6)) {
      condition = COND_GTEQ_S;
    } else {
      condition = COND_GTEQ;
    }
  } else if (!strncasecmp(cond, "leeq", 4)) {
    if (!strncasecmp(cond, "leeq_s", 6)) {
      condition = COND_LEEQ_S;
    } else {
      condition = COND_LEEQ;
    }
  } else if (!strncasecmp(cond, "gt", 2)) {
    if (!strncasecmp(cond, "gt_s", 4)) {
      condition = COND_GT_S;
    } else {
      condition = COND_GT;
    }
  } else if (!strncasecmp(cond, "le", 2)) {
    if (!strncasecmp(cond, "le_s", 4)) {
      condition = COND_LE_S;
    } else {
      condition = COND_LE;
    }
  }
}


/*
int cSimpleMessageSender::myConfigureInstance()
{
  return  cDataSink::myConfigureInstance();
  
}
*/


int cSimpleMessageSender::myFinaliseInstance()
{
  // FIRST call cDataSink myFinaliseInstance, this will finalise our dataWriter...
  int ret = cDataSink::myFinaliseInstance();

  /* if that was SUCCESSFUL (i.e. ret == 1), then do some stuff like
     loading models or opening output files: */

  // We find the index of dataElementName here. 
  // The lookup of this index has computational overhead, so we do not
  // want to do it for every tick in the myTick() function.
  
  if (dataElementName != NULL && dataElementIndex != -1) {
    const FrameMetaInfo * fmeta = reader_->getFrameMetaInfo();
    dataElementIndex = fmeta->findField(dataElementName);
  }
  return ret;
}

int cSimpleMessageSender::processComponentMessage(cComponentMessage *msg)
{
  if (enableDebugReceiver && msg != NULL) {
    SMILE_IMSG(2, "Got component message from '%s'", msg->sender);
    printMessage(msg);
    return 1;
  }
  return 0;
}

void cSimpleMessageSender::printMessage(cComponentMessage *msg)
{
  if (msg->sender != NULL) { 
    SMILE_PRINT("  Sender: %s", msg->sender);
  }
  SMILE_PRINT("  MsgType: %s", msg->msgtype);
  SMILE_PRINT("  MsgName: %s", msg->msgname);
  SMILE_PRINT("  MsgId: %i", msg->msgid);
  SMILE_PRINT("  ReaderTime: %f", msg->readerTime);
  SMILE_PRINT("  SmileTime: %f", msg->smileTime);
  if (msg->userTime1 != 0.0) {
    SMILE_PRINT("  UserTime1: %f", msg->userTime1);
  }
  if (msg->userTime2 != 0.0) {
    SMILE_PRINT("  UserTime2: %f", msg->userTime2);
  }
  if (msg->userflag1 != 0) {
    SMILE_PRINT("  UserFlag1: %i", msg->userflag1);
  }
  if (msg->userflag2 != 0) {
    SMILE_PRINT("  UserFlag2: %i", msg->userflag2);
  }
  if (msg->userflag3 != 0) {
    SMILE_PRINT("  UserFlag3: %i", msg->userflag3);
  }
  for (int i = 0; i < CMSG_nUserData; ++i) { 
    SMILE_PRINT("  Float[%i]: %f", i, msg->floatData[i]);
  }
  for (int i = 0; i < CMSG_nUserData; ++i) { 
    SMILE_PRINT("  Int[%i]: %ld", i, msg->intData[i]);
  }
  SMILE_PRINT("  MsgText: %s", msg->msgtext);
  SMILE_PRINT("  CustDataSize: %ld, CustData2Size: %ld", msg->custDataSize, msg->custData2Size);
  if (showCustDataAsText && msg->custData != NULL) {
    SMILE_PRINT("  CustData : '%s'", msg->custData);
  }
  if (showCustData2AsText && msg->custData2 != NULL) {
    SMILE_PRINT("  CustData2: '%s'", msg->custData2);
  }
  SMILE_PRINT("--- end of message ---\n");
}

void cSimpleMessageSender::sendMessage(cComponentMessage *msg)
{
  if (enableDebugSender) {
    SMILE_IMSG(2, "Printing message that will be sent to '%s':", messageRecp);
    printMessage(msg);
  }
  sendComponentMessage(messageRecp, msg);
}

void cSimpleMessageSender::sendPeriodicMessage(FLOAT_DMEM *v, int Nv, long vi, double tm)
{
  cComponentMessage msg;
  memset(&msg, 0, sizeof(msg));
  strncpy(msg.msgtype, messageType, CMSG_typenameLen);
  strncpy(msg.msgname, messageName, CMSG_typenameLen);
  if (v != NULL) {
    for (int i = 0; i < Nv && i < CMSG_nUserData; i++) {
      msg.floatData[i] = v[i];
    }
  }
  msg.intData[0] = 0;
  msg.readerTime = tm;
  msg.userTime1 = (double)vi;
  sendMessage(&msg);
}

void cSimpleMessageSender::sendEventMessage(FLOAT_DMEM *v, int Nv, const char * text, FLOAT_DMEM ref, long vi, double tm)
{
  cComponentMessage msg;
  memset(&msg, 0, sizeof(msg));
  strncpy(msg.msgtype, messageType, CMSG_typenameLen);
  strncpy(msg.msgname, messageName, CMSG_typenameLen);
  strncpy(msg.msgtext, text, CMSG_textLen);
  if (v != NULL) {
    for (int i = 0; i < Nv && i < CMSG_nUserData; i++) {
      msg.floatData[i] = v[i];
    }
  }
  msg.floatData[1] = ref;
  msg.intData[0] = (int)condition;
  msg.readerTime = tm;
  msg.userTime1 = (double)vi;
  sendMessage(&msg);
}

void cSimpleMessageSender::eventMessage(FLOAT_DMEM v, long vi, double tm)
{
  int Nv = 1;
  if (condition == COND_GTEQ && v >= threshold) {
    sendEventMessage(&v, Nv, "greater equal", threshold, vi, tm);
  } else if (condition == COND_GT && v > threshold) {
    sendEventMessage(&v, Nv, "greater", threshold, vi, tm);
  } else if (condition == COND_EQ && v == threshold) {
    sendEventMessage(&v, Nv, "equal", threshold, vi, tm);
  } else if (condition == COND_LEEQ && v <= threshold) {
    sendEventMessage(&v, Nv, "lesser equal", threshold, vi, tm);
  } else if (condition == COND_LE && v < threshold) {
    sendEventMessage(&v, Nv, "lesser", threshold, vi, tm);
  } else if (condition == COND_GTEQ_S) {
    if (v >= threshold) {
      if (!condFlag) {
        sendEventMessage(&v, Nv, "begin greater equal", threshold, vi, tm);
        condFlag = true;
      }
    } else {
      condFlag = false;
    }
  } else if (condition == COND_GT_S) {
    if (v > threshold) {
      if (!condFlag) {
        sendEventMessage(&v, Nv, "begin greater", threshold, vi, tm);
        condFlag = true;
      }
    } else {
      condFlag = false;
    }
  } else if (condition == COND_EQ_S) {
    if (v == threshold) {
      if (!condFlag) {
        sendEventMessage(&v, Nv, "begin equal", threshold, vi, tm);
        condFlag = true;
      }
    } else {
      condFlag = false;
    }
  } else if (condition == COND_LEEQ_S) {
    if (v <= threshold) {
      if (!condFlag) {
        sendEventMessage(&v, Nv, "begin lesser equal", threshold, vi, tm);
        condFlag = true;
      }
    } else {
      condFlag = false;
    }
  } else if (condition == COND_LE_S) {
    if (v < threshold) {
      if (!condFlag) {
        sendEventMessage(&v, Nv, "begin lesser", threshold, vi, tm);
        condFlag = true;
      }
    } else {
      condFlag = false;
    }
  } 
}

int cSimpleMessageSender::myTick(long long t)
{
  cVector *vec = reader_->getNextFrame();
  if (vec == NULL) return 0;  // no data available

  long vi = vec->tmeta->vIdx;   // the frame's virtual index (frame number from start of system)
  double tm = vec->tmeta->time; // the frame's time (data time from start of system)
  
  if (dataElementIndex < vec->N && dataElementIndex >= 0) {
    if (sendPeriodically) {
      sendPeriodicMessage(&(vec->dataF[dataElementIndex]), 1, vi, tm);
    } else {
      eventMessage(vec->dataF[dataElementIndex], vi, tm);
    }
  } else if (dataElementIndex == -1) {
    if (sendPeriodically) {
      sendPeriodicMessage(vec->dataF, vec->N, vi, tm);
    }
  }
  return 1;
}


cSimpleMessageSender::~cSimpleMessageSender()
{
}

