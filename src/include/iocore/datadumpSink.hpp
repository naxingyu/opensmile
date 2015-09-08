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


#ifndef __CDATADUMPSINK_HPP
#define __CDATADUMPSINK_HPP

#include <core/smileCommon.hpp>
#include <core/dataSink.hpp>

#define COMPONENT_DESCRIPTION_CDATADUMPSINK "This component writes dataMemory data to a raw binary file (e.g. for matlab import). The binary file consits of 32-bit float values representing the data values, concattenated frame by frame along the time axis. The first two float values in the file resemble the file header, an thus indicate the dimension of the matrix (1: size of frames, 2: number of frames in file). The total file size in bytes is thus <size of frames>x<number of frames>x4 + 2."
#define COMPONENT_NAME_CDATADUMPSINK "cDatadumpSink"

#undef class
class DLLEXPORT cDatadumpSink : public cDataSink {
  private:
    FILE * filehandle;
    const char *filename;
    int lag;
    int append;
    long nVec,vecSize;
    
    void writeHeader();
    
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
    
    cDatadumpSink(const char *_name);

    virtual ~cDatadumpSink();
};




#endif // __CDATADUMPSINK_HPP
