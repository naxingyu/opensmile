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

LibSVM file output

*/


#ifndef __CLIBSVMSINK_HPP
#define __CLIBSVMSINK_HPP

#include <core/smileCommon.hpp>
#include <core/dataSink.hpp>

#define BUILD_COMPONENT_LibsvmSink
#define COMPONENT_DESCRIPTION_CLIBSVMSINK "This component writes data to a text file in LibSVM feature file format. For the 'on-the-fly' classification component see 'cLibsvmliveSink'."
#define COMPONENT_NAME_CLIBSVMSINK "cLibsvmSink"


#undef class
class DLLEXPORT cLibsvmSink : public cDataSink {
  private:
    FILE * filehandle;
    const char *filename;
    int lag;
    int append, timestamp;
    const char *instanceBase, *instanceName;
    
    int targetNumAll;
    int nClasses;
    long nInst;
    long inr;
    char **classname;
    int *target;

    int getClassIndex(const char *_name)
    {
      int i;
      if (_name == NULL) return -1;
      if (classname == NULL) return -1;
      for (i=0; i<nClasses; i++) {
        if ((classname[i]!=NULL)&&(!strcmp(_name,classname[i]))) return i;
      }
      return -1;
    }

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);


  public:
    //static sComponentInfo * registerComponent(cConfigManager *_confman);
    //static cSmileComponent * create(const char *_instname);
    SMILECOMPONENT_STATIC_DECL
    
    cLibsvmSink(const char *_name);

    virtual ~cLibsvmSink();
};




#endif // __CLIBSVMSINK_HPP
