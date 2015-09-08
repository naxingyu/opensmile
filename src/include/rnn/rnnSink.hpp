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

example dataSink:
reads data from data memory and outputs it to console/logfile (via smileLogger)
this component is also useful for debugging

*/


#ifndef __CRNNSINK_HPP
#define __CRNNSINK_HPP

#include <core/smileCommon.hpp>

#ifdef BUILD_RNN
#include <core/dataSink.hpp>
#include <rnn/rnn.hpp>

#define COMPONENT_DESCRIPTION_CRNNSINK "This is an example of a cDataSink descendant. It reads data from the data memory and prints it to the console. This component is intended as a template for developers."
#define COMPONENT_NAME_CRNNSINK "cRnnSink"

#undef class




class DLLEXPORT cRnnSink : public cDataSink {
  private:
    FILE *outfile;
    FILE *outfileC;
    const char *netfile;
    const char *actoutput;
    const char *classoutput;
    char *classlabels;
    const char **classlabelArr;
    long nClasses;
    int ctcDecode;
    cNnRnn *rnn;
    cRnnNetFile net;
    FLOAT_NN *in;
    FLOAT_DMEM *out;
    int lasti;
    int printConnections;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    //cRnnWeightVector *createWeightVectorFromLine(char *line);
    //int loadNet(const char *filename);
    
//    int findPeepWeights(unsigned long id);
    //int findWeights(unsigned long idFrom, unsigned long idTo);
    //cNnLSTMlayer *createLstmLayer(int i, int idx, int dir=LAYER_DIR_FWD);
    // create a network from a successfully loaded net config file (loadNet function)
    //int createNet();
    
    virtual void fetchConfig();
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cRnnSink(const char *_name);

    virtual ~cRnnSink();
};


#endif // BUILD_RNN

#endif // __CRNNSINK_HPP
