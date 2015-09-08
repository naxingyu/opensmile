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

vector selector based on threshold of value <idx>

*/


#ifndef __CVALBASEDSELECTOR_HPP
#define __CVALBASEDSELECTOR_HPP

#include <core/smileCommon.hpp>
#include <core/dataProcessor.hpp>

#define COMPONENT_DESCRIPTION_CVALBASEDSELECTOR "This component copies only those frames from the input to the output that match a certain threshold criterion, i.e. where a specified value N exceeds a certain threshold."
#define COMPONENT_NAME_CVALBASEDSELECTOR "cValbasedSelector"

#undef class
class DLLEXPORT cValbasedSelector : public cDataProcessor {
  private:
    long idx;
    int removeIdx, invert, allowEqual, zerovec;
    int adaptiveThreshold_;
    int debugAdaptiveThreshold_;
    FLOAT_DMEM outputVal;
    FLOAT_DMEM threshold;
    cVector *myVec;
    long elI;
    FLOAT_DMEM *valBuf_;
    FLOAT_DMEM valBufSum_;
    FLOAT_DMEM valBufN_;
    int valBufSize_, valBufIdx_;
    int valBufRefreshCnt_;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);
    virtual int setupNamesForField(int i, const char*name, long nEl);

    // virtual int dataProcessorCustomFinalise();
    // virtual int setupNewNames(long nEl);
    //virtual int setupNamesForField(int i, const char*name, long nEl);
    //virtual int configureWriter(sDmLevelConfig &c);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cValbasedSelector(const char *_name);

    virtual ~cValbasedSelector();
};




#endif // __CVALBASEDSELECTOR_HPP
