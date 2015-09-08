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

reads in frames , outputs windows
*/


#ifndef __CVECTOWINPROCESSOR_HPP
#define __CVECTOWINPROCESSOR_HPP

#include <core/smileCommon.hpp>
#include <core/dataProcessor.hpp>
#include <dspcore/windower.hpp>

#define COMPONENT_DESCRIPTION_CVECTOWINPROCESSOR "Base class: reads in frames , outputs windows"
#define COMPONENT_NAME_CVECTOWINPROCESSOR "cVecToWinProcessor"

struct sVecToWinProcessorOla
{
  double * norm;
  FLOAT_DMEM * buffer;
  long bufferPtr;
  long bufferReadPtr;
  long buffersize;
  double overlap;
  long framelen;
};


#undef class
class DLLEXPORT cVecToWinProcessor : public cDataProcessor {
  private:
    
    int   dtype;     // data type (DMEM_FLOAT, DMEM_INT)
    int   noPostEOIprocessing;
    int   processArrayFields;
    int   normaliseAdd;
    int   useWinAasWinB;
    FLOAT_DMEM gain;

    long Nfi;
    long No;

    cWindower * ptrWinA;
    cWindower * ptrWinB;

    long inputPeriodS;
    double inputPeriod;
    double samplePeriod;
    
    int hasOverlap;
    struct sVecToWinProcessorOla * ola;

    cMatrix *mat;

  protected:

    SMILECOMPONENT_STATIC_DECL_PR
    double getInputPeriod(){ return inputPeriod; }
    double getSamplePeriod(){ return samplePeriod; }

    long getNfi() { return Nfi; } // number of fields
    

    virtual void fetchConfig();
    //virtual int myFinaliseInstance();
    //virtual int myConfigureInstance();
    virtual int dataProcessorCustomFinalise();
    virtual int myTick(long long t);

    virtual int configureWriter(sDmLevelConfig &c);
    //virtual int setupNamesForElement(int idxi, const char*name, long nEl);
    //virtual int setupNamesForField(int idxi, const char*name, long nEl);

    void initOla(long n, double _samplePeriod, double _inputPeriod, int idx);
    void computeOlaNorm(long n, int idx);
    double * getWindowfunction(cWindower *_ptrWin, long n, int *didAlloc = NULL);
    //void setOlaBuffer(long i, long j, FLOAT_DMEM val);
    void setOlaBufferNext(long idx, FLOAT_DMEM val);
    int flushOlaBuffer(cMatrix *mat);
    
    virtual int doProcess(int i, cMatrix *row, FLOAT_DMEM*x);
    virtual int doProcess(int i, cMatrix *row, INT_DMEM*x);
    virtual int doFlush(int i, FLOAT_DMEM*x);
    virtual int doFlush(int i, INT_DMEM*x);
    

    //virtual int processComponentMessage( cComponentMessage *_msg );

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cVecToWinProcessor(const char *_name);

    virtual ~cVecToWinProcessor();
};




#endif // __CVECTOWINPROCESSOR_HPP
