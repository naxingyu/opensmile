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

reads in windows and outputs vectors (frames)

*/


#ifndef __CWINTOVECPROCESSOR_HPP
#define __CWINTOVECPROCESSOR_HPP

#include <core/smileCommon.hpp>
#include <core/dataProcessor.hpp>

#define COMPONENT_DESCRIPTION_CWINTOVECPROCESSOR "reads input windows, outputs frame(s)"
#define COMPONENT_NAME_CWINTOVECPROCESSOR "cWinToVecProcessor"

#define FRAME_MSG_QUE_SIZE 10
#define MATBUF_ALLOC_STEP  200

#define FRAMEMODE_FIXED 0 
#define FRAMEMODE_FULL  1
#define FRAMEMODE_VAR   2
#define FRAMEMODE_LIST  3
#define FRAMEMODE_META  4


#undef class
class DLLEXPORT cWinToVecProcessor : public cDataProcessor {
  private:
    int   frameMode;
    int   fsfGiven;   // flag that indicates whether frameSizeFrame, etc. was specified directly (to override frameSize in seconds)
    int   fstfGiven;   // flag that indicates whether frameStepFrame, etc. was specified directly (to override frameStep in seconds)
    int   dtype;     // data type (DMEM_FLOAT, DMEM_INT)
    int   noPostEOIprocessing;
    int   nIntervals;  // number of intervals for frameMode = list
    double *ivSec; //interleaved array : start/end in seconds (frameList, frameMode = list)
    long *ivFrames; //interleaved array : start/end in frames (vIdx) (frameList, frameMode = list)
    long frameIdx; // next frame index, (frameMode== list)

    long Mult;
    long matBufN; // number of valid elements in matbuf
    long matBufNalloc; // backup of matbuf->nT
    char * lastText, *lastCustom;
    double inputPeriod;
    
    cMatrix *matBuf;
    cMatrix *tmpRow;
    cVector *tmpVec;
    FLOAT_DMEM *tmpFrameF;
    INT_DMEM *tmpFrameI;

    //mapping of field indicies to config indicies: (size of these array is maximum possible size: Nfi)
    int Nfconf;
    int *fconf, *fconfInv;
    long *confBs;  // blocksize for configurations

    int addFconf(long bs, int field); // return value is index of assigned configuration
    
    // message memory:
    int nQ; // number of frames quequed
    double Qstart[FRAME_MSG_QUE_SIZE]; // start time array
    double Qend[FRAME_MSG_QUE_SIZE];   // end time array
    int Qflag[FRAME_MSG_QUE_SIZE];     // final frame flag (turn end..)
    int QID[FRAME_MSG_QUE_SIZE];     // final frame flag (turn end..)
    int getNextFrameData(double *start, double *end, int *flag, int *ID);
    int peakNextFrameData(double *start, double *end, int *flag, int *ID);
    int clearNextFrameData();
    int queNextFrameData(double start, double end, int flag, int ID);
    void addVecToBuf(cVector *ve);

  protected:
    long Ni, Nfi;
    long No; //, Nfo;
    int   allow_last_frame_incomplete_;
    int   wholeMatrixMode; // this is 0 by default, but child classes can overwrite it, if they implement the doProcessMatrix method instead of the doProcess method
    int   processFieldsInMatrixMode; // <- this variable is set from the config by the child class, the config option must also be populated by the child class; by default the value is 0

    double frameSize, frameStep, frameCenter;
    long  frameSizeFrames, frameStepFrames, frameCenterFrames, pre;

    SMILECOMPONENT_STATIC_DECL_PR
    double getInputPeriod(){return inputPeriod;}
    long getNi() { return Ni; } // number of elements
    long getNfi() { return Nfi; } // number of fields
    long getNf() { return Ni; } // return whatever will be processed (Ni: winToVec, Nfi, vecProc.)

    // converts a string time (framemode=list index list) to a float time value
    double stringToTimeval(char *x, int *isSec);

    // (input) field configuration, may be used in setupNamesForField
    int getFconf(int field) { return fconf[field]; } // caller must check for return value -1 (= no conf available for this field)
    void multiConfFree( void * x );
    void *multiConfAlloc() {
      return calloc(1,sizeof(void*)*getNf());
    }
    
    virtual void fetchConfig();
    //virtual int myFinaliseInstance();
    //virtual int myConfigureInstance();
    virtual int dataProcessorCustomFinalise();
    virtual int myTick(long long t);

    // this must return the multiplier, i.e. the vector size returned for each input element (e.g. number of functionals, etc.)
    virtual int getMultiplier();
    
    // get the number of outputs given a number of inputs, 
    // this is used for wholeMatrixMode, where the quantitative relation between inputs and outputs may be non-linear
    // this replaces getMultiplier, which is used in the standard row-wise processing mode
    virtual long getNoutputs(long nIn);

    virtual int configureWriter(sDmLevelConfig &c);
    virtual int setupNamesForElement(int idxi, const char*name, long nEl);
    //virtual int setupNamesForField(int idxi, const char*name, long nEl);
    virtual int doProcessMatrix(int i, cMatrix *in, FLOAT_DMEM *out, long nOut);
    virtual int doProcessMatrix(int i, cMatrix *in, INT_DMEM *out, long nOut);
    virtual int doProcess(int i, cMatrix *row, FLOAT_DMEM*x);
    virtual int doProcess(int i, cMatrix *row, INT_DMEM*x);
    virtual int doFlush(int i, FLOAT_DMEM*x);
    virtual int doFlush(int i, INT_DMEM*x);

    virtual int processComponentMessage( cComponentMessage *_msg );

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cWinToVecProcessor(const char *_name);

    virtual ~cWinToVecProcessor();
};




#endif // __CWINTOVECPROCESSOR_HPP
