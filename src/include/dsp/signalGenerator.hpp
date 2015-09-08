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

Signal source. Generates various noise types and pre-defined signals.

*/


#ifndef __CSIGNALGENERATOR_HPP
#define __CSIGNALGENERATOR_HPP

#include <core/smileCommon.hpp>
#include <core/dataSource.hpp>

#define BUILD_COMPONENT_SignalGenerator
#define COMPONENT_DESCRIPTION_CSIGNALGENERATOR "This component provides a signal source. This source generates various noise types and pre-defined signals and value patterns. See the configuration documentation for a list of currently implemented types."
#define COMPONENT_NAME_CSIGNALGENERATOR "cSignalGenerator"

#define NOISE_WHITE   0   // white gaussian noise 'white'
#define SIGNAL_SINE   1   // sinusodial signal (single frequency) 'sine'
#define SIGNAL_CONST  2   // constant value 'const'
#define SIGNAL_RECT   3   // rectangular periodic signal 'rect'
//...

#undef class
class DLLEXPORT cSignalGenerator : public cDataSource {
  private:
    long nValues;
    int randSeed;
    FLOAT_DMEM stddev, mean;
    FLOAT_DMEM constant;
    double signalPeriod, phase;
    double myt ; // current time
    double samplePeriod;
    double lastP;
    double scale;
    double val;

    int * nElements;
    char ** fieldNames;
    int nFields;
    
    int noiseType;
    long lengthFrames;
    long curT;
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    virtual int configureWriter(sDmLevelConfig &c);
    virtual int setupNewNames(long nEl);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cSignalGenerator(const char *_name);

    virtual ~cSignalGenerator();
};




#endif // __CSIGNALGENERATOR_HPP
