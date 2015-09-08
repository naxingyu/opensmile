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

ARFF file output (for WEKA)

*/


#ifndef __ARFF_SINK_HPP
#define __ARFF_SINK_HPP

#include <core/smileCommon.hpp>
#include <core/dataSink.hpp>

#define COMPONENT_DESCRIPTION_CARFFSINK "This component writes dataMemory data to an ARFF file (WEKA). Depending on your config an instance name field, a frame index, and a frame time field can be added as well as multiple class/target attributes. See the config type documentation for more details."
#define COMPONENT_NAME_CARFFSINK "cArffSink"

#undef class
class DLLEXPORT cArffSink : public cDataSink {
  private:
    FILE * filehandle;
    double frameTimeAdd;
    const char *filename;
    int lag;
    int append, timestamp, number, prname, frameLength;
    int instanceNameFromMetadata, useTargetsFromMetadata;
    const char *relation;
    const char *instanceBase, *instanceName;
    bool disabledSink_;

    int printDefaultClassDummyAttribute;    
    int nClasses; long nInst;
    long inr;
    char **classname;
    char **classtype;
    
    char **targetall;
    char ***targetinst;
    
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
    
    cArffSink(const char *_name);

    virtual ~cArffSink();
};




#endif // __ARFF_SINK_HPP
