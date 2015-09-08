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

BLSTM RNN processor

*/


#ifndef __CRNNPROCESSOR_HPP
#define __CRNNPROCESSOR_HPP

#include <core/smileCommon.hpp>

#ifdef BUILD_RNN
#include <core/dataProcessor.hpp>
#include <rnn/rnn.hpp>

#define COMPONENT_DESCRIPTION_CRNNPROCESSOR "BLSTM RNN processor."
#define COMPONENT_NAME_CRNNPROCESSOR "cRnnProcessor"

#undef class
class DLLEXPORT cRnnProcessor : public cDataProcessor {
  private:
    const char *netfile;
    char *classlabels_;
    const char **classlabelArr_;
    long nClasses;
    cNnRnn *rnn;
    cRnnNetFile net;
    FLOAT_NN *in;
    FLOAT_DMEM *out;
    int printConnections;
    cVector *frameO;
    int jsonNet;
    int net_created_;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    virtual int setupNewNames(long nEl);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cRnnProcessor(const char *_name);

    virtual ~cRnnProcessor();
};


#endif // BUILD_RNN

#endif // __CRNNPROCESSOR_HPP
