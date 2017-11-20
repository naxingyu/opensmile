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
 *            Martin Woellmer, Bjoern Schuller
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

/* openSMILE plugin loader */

#include <smileCommon.hpp>
#include <smileLogger.hpp>
#include <componentManager.hpp>


//++ include all your component headers here: ----------------
#include <exampleGuipluginSink.hpp>
#include <simpleVisualiserGUI.hpp>
#include <classifierResultGUI.hpp>

//++ ---------------------------------------------------------

// dll export for msvc++
#ifdef _MSC_VER 
#define DLLEXPORT_P __declspec(dllexport)
#else
#define DLLEXPORT_P
#endif


#define MODULE "pluginLoader"

static DLLLOCAL const registerFunction complist[] = {
//++ add your component register functions here: -------------
  cExampleGuipluginSink::registerComponent,
  cSimpleVisualiserGUI::registerComponent,
  cClassifierResultGUI::registerComponent,

//++ ---------------------------------------------------------
  NULL  // last element must always be NULL to terminate the list !!
};

sComponentInfo *master=NULL;

//Library:
extern "C" DLLEXPORT_P sComponentInfo * registerPluginComponent(cConfigManager *_confman, cComponentManager *_compman) {
  registerFunction f;
  sComponentInfo *cur = NULL, *last = NULL;
  int i=0;
  f = complist[i];
  while (f!=NULL) {
    cur = (f)(_confman, _compman);
    cur->next = NULL;
    if (i==0) master = cur;
    else { last->next = cur; }
    last = cur;
    f=complist[++i];
  }
  return master;
}

/*extern "C" DLLEXPORT_P sComponentInfo * registerPluginComponent(cConfigManager *_confman, cComponentManager *_compman) {
  return (sComponentInfo *)complist;
}*/


// this function frees all memory allocated within the scope of the dll ...
extern "C" DLLEXPORT_P void unRegisterPluginComponent() {
  while (master != NULL) {
    sComponentInfo *tmp = master;
    if (master->next != NULL) master = master->next;
    delete tmp;
  }
}
