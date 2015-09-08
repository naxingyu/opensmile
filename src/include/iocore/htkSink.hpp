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


#ifndef __CHTKSINK_HPP
#define __CHTKSINK_HPP

#include <core/smileCommon.hpp>
#include <core/smileComponent.hpp>
#include <core/dataSink.hpp>

#define COMPONENT_DESCRIPTION_CHTKSINK "This component writes dataMemory data to a binary HTK parameter file."
#define COMPONENT_NAME_CHTKSINK "cHtkSink"


#undef class
class DLLEXPORT cHtkSink : public cDataSink {
  private:
    FILE * filehandle;
    int vax;
    const char *filename;
    int lag;
    int append;
    uint16_t parmKind;
    uint32_t vecSize;
    uint32_t nVec;
    double period;
    sHTKheader header;
    bool disabledSink_;

    void prepareHeader( sHTKheader *h )
    {
      smileHtk_prepareHeader(h);
    }

    int writeHeader();
    
    int readHeader() 
    {
      return smileHtk_readHeader(filehandle,&header);
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
    
    cHtkSink(const char *_name);

    virtual ~cHtkSink();
};




#endif // __CHTKSINK_HPP
