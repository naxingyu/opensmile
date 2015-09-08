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
