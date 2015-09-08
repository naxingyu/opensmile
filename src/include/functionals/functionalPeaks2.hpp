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

functionals: number of peaks and various measures associated with peaks

*/


#ifndef __CFUNCTIONALPEAKS2_HPP
#define __CFUNCTIONALPEAKS2_HPP

#include <core/smileCommon.hpp>
#include <core/dataMemory.hpp>
#include <functionals/functionalComponent.hpp>

#define COMPONENT_DESCRIPTION_CFUNCTIONALPEAKS2 "  number of peaks and various measures associated with peaks, such as mean of peaks, mean distance between peaks, etc. Peak finding is based on : x(t-1) < x(t) > x(t+1) plus an advanced post filtering of low relative amplitude peaks. See source code for brief description of peak picking algorithm. This component provideas a new and improved algorithm for peak detection, as compared to cFunctionalPeaks component."
#define COMPONENT_NAME_CFUNCTIONALPEAKS2 "cFunctionalPeaks2"


#undef class
    struct peakMinMaxListEl {
      int type;
      FLOAT_DMEM y;
      long x;
      struct peakMinMaxListEl * next, *prev;
    };

class DLLEXPORT cFunctionalPeaks2 : public cFunctionalComponent {
  private:
    int enabSlope;
    int noClearPeakList;

	  FLOAT_DMEM relThresh;
	  FLOAT_DMEM absThresh;
	  int useAbsThresh;
	  int dynRelThresh;
	  int isBelowThresh(FLOAT_DMEM diff, FLOAT_DMEM base);

	  struct peakMinMaxListEl * mmlistFirst;
	  struct peakMinMaxListEl * mmlistLast;
	  void addMinMax(int type, FLOAT_DMEM y, long x);
	  void dbgPrintMinMaxList(struct peakMinMaxListEl * listEl);
	  void removeFromMinMaxList( struct peakMinMaxListEl * listEl );
	  void clearList();

    FILE *dbg;
    int consoleDbg;
    const char *posDbgOutp;
    int posDbgAppend;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    virtual void fetchConfig();

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cFunctionalPeaks2(const char *_name);
    // inputs: sorted and unsorted array of values, out: pointer to space in output array, You may not return MORE than Nout elements, please return as return value the number of actually computed elements (usually Nout)
    virtual long process(FLOAT_DMEM *in, FLOAT_DMEM *inSorted, FLOAT_DMEM min, FLOAT_DMEM max, FLOAT_DMEM mean, FLOAT_DMEM *out, long Nin, long Nout);
    //virtual long process(INT_DMEM *in, INT_DMEM *inSorted, INT_DMEM *out, long Nin, long Nout);

    virtual long getNoutputValues() { return nEnab; }
    virtual int getRequireSorted() { return 0; }

    virtual ~cFunctionalPeaks2();
};




#endif // __CFUNCTIONALPEAKS2_HPP
