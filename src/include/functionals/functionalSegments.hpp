/*F***************************************************************************
 * 
 * openSMILE - the Munich open source Multimedia Interpretation by 
 * Large-scale Extraction toolkit
 * 
 * This file is part of openSMILE.
 * 
 * openSMILE is copyright (c) by audEERING GmbH. All rights reserved.
 * 
 * See file "COPYING" for details on usage rights and licensing terms.
 * By using, copying, editing, compiling, modifying, reading, etc. this
 * file, you agree to the licensing terms in the file COPYING.
 * If you do not agree to the licensing terms,
 * you must immediately destroy all copies of this file.
 * 
 * THIS SOFTWARE COMES "AS IS", WITH NO WARRANTIES. THIS MEANS NO EXPRESS,
 * IMPLIED OR STATUTORY WARRANTY, INCLUDING WITHOUT LIMITATION, WARRANTIES OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ANY WARRANTY AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE SOFTWARE OR ANY WARRANTY OF TITLE
 * OR NON-INFRINGEMENT. THERE IS NO WARRANTY THAT THIS SOFTWARE WILL FULFILL
 * ANY OF YOUR PARTICULAR PURPOSES OR NEEDS. ALSO, YOU MUST PASS THIS
 * DISCLAIMER ON WHENEVER YOU DISTRIBUTE THE SOFTWARE OR DERIVATIVE WORKS.
 * NEITHER TUM NOR ANY CONTRIBUTOR TO THE SOFTWARE WILL BE LIABLE FOR ANY
 * DAMAGES RELATED TO THE SOFTWARE OR THIS LICENSE AGREEMENT, INCLUDING
 * DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL OR INCIDENTAL DAMAGES, TO THE
 * MAXIMUM EXTENT THE LAW PERMITS, NO MATTER WHAT LEGAL THEORY IT IS BASED ON.
 * ALSO, YOU MUST PASS THIS LIMITATION OF LIABILITY ON WHENEVER YOU DISTRIBUTE
 * THE SOFTWARE OR DERIVATIVE WORKS.
 * 
 * Main authors: Florian Eyben, Felix Weninger, 
 * 	      Martin Woellmer, Bjoern Schuller
 * 
 * Copyright (c) 2008-2013, 
 *   Institute for Human-Machine Communication,
 *   Technische Universitaet Muenchen, Germany
 * 
 * Copyright (c) 2013-2015, 
 *   audEERING UG (haftungsbeschraenkt),
 *   Gilching, Germany
 * 
 * Copyright (c) 2016,	 
 *   audEERING GmbH,
 *   Gilching Germany
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
