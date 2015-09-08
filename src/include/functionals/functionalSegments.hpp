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

number of segments based on delta thresholding

*/


#ifndef __CFUNCTIONALSEGMENTS_HPP
#define __CFUNCTIONALSEGMENTS_HPP

#include <core/smileCommon.hpp>
#include <core/dataMemory.hpp>
#include <functionals/functionalComponent.hpp>

#define COMPONENT_DESCRIPTION_CFUNCTIONALSEGMENTS "  number of segments based on simple delta thresholding"
#define COMPONENT_NAME_CFUNCTIONALSEGMENTS "cFunctionalSegments"


#define SEG_DELTA  1
#define SEG_ABSTH  2
#define SEG_RELTH  3
#define SEG_ABSTH_NOAVG  4
#define SEG_RELTH_NOAVG  5
#define SEG_MRELTH  6
#define SEG_MRELTH_NOAVG  7
#define SEG_DELTA2  8

#define SEG_CHX  101
#define SEG_NONX  102
#define SEG_EQX  103
#define SEG_LTX  104
#define SEG_GTX  105
#define SEG_LEQX  106
#define SEG_GEQX  107

struct sSegData {
  FLOAT_DMEM mean;
  FLOAT_DMEM range;
  FLOAT_DMEM max,min;
  long * segLens;

  long nSegments;
  long meanSegLen;
  long maxSegLen;
  long minSegLen;
  long seglenStddev;
};

#undef class
class DLLEXPORT cFunctionalSegments : public cFunctionalComponent {
  private:
    int dbgPrint;
    int useOldBuggyChX;   // use old buggy version of ChX, EqX and NonX
    int autoSegMinLng;
    long manualRavgLng;
    int segmentationAlgorithm;
    long maxNumSeg;
    long maxNumSeg0;
    int growDynSegBuffer;
    long segMinLng, pauseMinLng;
    FLOAT_DMEM rangeRelThreshold;
    FLOAT_DMEM X;
    int XisRel;
    int nThresholds;
    FLOAT_DMEM *thresholds, *thresholdsTemp;
    //int overlapFlag;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    virtual void fetchConfig();

    long addNewSegment(long i, long lastSeg, sSegData *result);
    int process_SegDelta(FLOAT_DMEM *in, FLOAT_DMEM *out, long Nin, long Nout, sSegData *result);
    int process_SegDelta2(FLOAT_DMEM *in, FLOAT_DMEM *out, long Nin, long Nout, sSegData *result);
    int process_SegThresh(FLOAT_DMEM *in, FLOAT_DMEM *out, long Nin, long Nout, sSegData *result);
    int process_SegThreshNoavg(FLOAT_DMEM *in, FLOAT_DMEM *out, long Nin, long Nout, sSegData *result);
    int process_SegChX_oldBuggy(FLOAT_DMEM *in, FLOAT_DMEM *out, long Nin, long Nout, sSegData *result);
    int process_SegNonX_oldBuggy(FLOAT_DMEM *in, FLOAT_DMEM *out, long Nin, long Nout, sSegData *result);
    int process_SegEqX_oldBuggy(FLOAT_DMEM *in, FLOAT_DMEM *out, long Nin, long Nout, sSegData *result);
    int process_SegChX(FLOAT_DMEM *in, FLOAT_DMEM *out, long Nin, long Nout, sSegData *result);
    int process_SegNonX(FLOAT_DMEM *in, FLOAT_DMEM *out, long Nin, long Nout, sSegData *result);
    int process_SegEqX(FLOAT_DMEM *in, FLOAT_DMEM *out, long Nin, long Nout, sSegData *result);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cFunctionalSegments(const char *_name);


    // inputs: sorted and unsorted array of values, out: pointer to space in output array, You may not return MORE than Nout elements, please return as return value the number of actually computed elements (usually Nout)
    virtual long process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM min, FLOAT_DMEM max, FLOAT_DMEM mean, FLOAT_DMEM *out, long Nin, long Nout);
    //virtual long process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout);

    virtual long getNoutputValues() { return nEnab; }
    virtual int getRequireSorted() { return 0; }

    virtual ~cFunctionalSegments();
};




#endif // __CFUNCTIONALSEGMENTS_HPP
