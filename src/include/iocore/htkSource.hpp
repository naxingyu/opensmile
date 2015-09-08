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


/*  openSMILE component: cHtkSource
-----------------------------------

HTK Source:

Reads data from an HTK parameter file.

-----------------------------------

11/2009 - Written by Florian Eyben
*/


#ifndef __CHTKSOURCE_HPP
#define __CHTKSOURCE_HPP

#include <core/smileCommon.hpp>
#include <core/dataSource.hpp>
//#include <htkSink.hpp>   // include this for the HTK header type and endianness functions

#define COMPONENT_DESCRIPTION_CHTKSOURCE "This component reads data from binary HTK parameter files."
#define COMPONENT_NAME_CHTKSOURCE "cHtkSource"

class cHtkSource : public cDataSource {
  private:
    sHTKheader head;
    int vax;
    const char * featureName;
    int N; // <-- sampleSize
    float *tmpvec;

    FILE *filehandle;
    const char *filename;
    int eof;
 
    void prepareHeader( sHTKheader *h )
    {
      smileHtk_prepareHeader(h);
    }

    int readHeader() 
    {
      return smileHtk_readHeader(filehandle,&head);
    }

  protected:
    SMILECOMPONENT_STATIC_DECL_PR


    virtual void fetchConfig();
    virtual int configureWriter(sDmLevelConfig &c);
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    virtual int setupNewNames(long nEl=0);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cHtkSource(const char *_name);

    virtual ~cHtkSource();
};




#endif // __CHTKSOURCE_HPP
