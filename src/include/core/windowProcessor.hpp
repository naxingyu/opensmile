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

filter :  (abstract class only)
       linear N-th order filter for single value data streams
       this class processed every element of a frame independently
       derived classes only need to implement the filter algorithm

*/


#ifndef __CWINDOWPROCESSOR_HPP
#define __CWINDOWPROCESSOR_HPP

#include <core/smileCommon.hpp>
#include <core/dataProcessor.hpp>

#define COMPONENT_DESCRIPTION_CWINDOWPROCESSOR "filter dataProcessor, filters each element in a dataMemory level independently"
#define COMPONENT_NAME_CWINDOWPROCESSOR "cWindowProcessor"

#undef class
class DLLEXPORT cWindowProcessor : public cDataProcessor {
  private:
    //int blocksize;         // block size for filter (speed up purpose only)
    long Ni;
    int isFirstFrame;
    int pre, post, winsize;
    int noPostEOIprocessing;
    
    cMatrix * matnew;
    cMatrix * rowout;
    cMatrix * rowsout;
    cMatrix * row;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    int multiplier;

    void setWindow(int _pre, int _post);
    int firstFrame() { return isFirstFrame; }

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    virtual int configureWriter(sDmLevelConfig &c);


   // buffer must include all (# order) past samples
    virtual int processBuffer(cMatrix *_in, cMatrix *_out,  int _pre, int _post );
    virtual int processBuffer(cMatrix *_in, cMatrix *_out,  int _pre, int _post, int rowGlobal );
    virtual int dataProcessorCustomFinalise();

/*
    virtual int setupNamesForField(int i, const char*name, long nEl);
    virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);
*/
    
  public:
    SMILECOMPONENT_STATIC_DECL
    
    cWindowProcessor(const char *_name, int _pre=0, int _post=0);

    virtual ~cWindowProcessor();
};




#endif // __CWINDOWPROCESSOR_HPP
