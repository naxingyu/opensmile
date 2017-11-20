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

example dataSink:
reads data from data memory and outputs it to console/logfile (via smileLogger)
this component is also useful for debugging

*/


#ifndef __CSVMSINK_HPP
#define __CSVMSINK_HPP

#include <core/smileCommon.hpp>

#ifdef BUILD_SVMSMO

#include <core/dataSink.hpp>
#define BUILD_COMPONENT_SvmSink
#define COMPONENT_DESCRIPTION_CSVMSINK "This is an example of a cDataSink descendant. It reads data from the data memory and prints it to the console. This component is intended as a template for developers."
#define COMPONENT_NAME_CSVMSINK "cSvmSink"

#undef class

#define SMILESVM_KERNEL_UNKNOWN 0  // unknown or unsupported kernel type

// supported kernel types:
#define SMILESVM_KERNEL_LINEAR        1
#define SMILESVM_KERNEL_POLYNOMIAL    2
#define SMILESVM_KERNEL_RBF           3
#define SVMSINK_WINNING_CLASS_PROB	  0
#define SVMSINK_WINNING_CLASS_VOTE	  1


struct smileBinarySvmModel
{
  int nSV;
  int has_logit;
  char * c1; int ic1;
  char * c2; int ic2;
  FLOAT_DMEM * SV;
  FLOAT_DMEM bias;
  double logit_coeff1;
  double logit_intercept;
};


class smileSvmModel
{

private:
  int showStatsDebug;
  int lineNr;
  int kernelType;
  int nClasses;
  int nPairs;
  char ** classnames;
  int vectorDim;
  char ** attributeNames;
  struct smileBinarySvmModel * binSvm;
  int * ftSelMap;
  int nFtSel;
  int ignoreLogitModel;

  const char * model;

  cSmileComponent * parent;
  const char * getInstName() { return parent->getInstName(); }

  int prepareModelStruct();
  int parseKernelType(char *line);
  int parseNClasses(char * line);
  char ** parseClassnames(char * line, int nClasses);
  int parseInt(char * line);
  FLOAT_DMEM parseDouble(char * line);
  FLOAT_DMEM parseBias(char * line);
  int parseLogisticModel(struct smileBinarySvmModel *_s, char *inp);
  int parseClassPair(struct smileBinarySvmModel *_s, char *inp);
  int parseSVs(FLOAT_DMEM **_SV, int *_nSV, char * line, int idx);
  int parseEnd(char * line);

  // get the distance from the hyperplane of model <index> for the vector in <v>
  double evalBinSvm(FLOAT_DMEM *v, int index);
  double evalLogit(int index, double d);
  FLOAT_DMEM * getPairwiseProbabilities(double **ps);

public:
  smileSvmModel(cSmileComponent *_parent, const char * _model) : parent(_parent), model(_model),
    classnames(NULL), attributeNames(NULL), binSvm(NULL), ftSelMap(NULL), nFtSel(0), showStatsDebug(0),
    ignoreLogitModel(0)
    {}

  void setIgnoreLogitModel() {
    ignoreLogitModel = 1;
  }

  // load the model from file
  int load();

  int getNclasses() { return nClasses; }

  // compute the output for a given vector v (length N, must match vectorDim variable (vector size in model))
  int evaluate(FLOAT_DMEM *v , long N, const char ** winningClass = NULL, FLOAT_DMEM ** probs=NULL, FLOAT_DMEM *conf=NULL, int winningClassMethod=SVMSINK_WINNING_CLASS_VOTE, int usePairwiseCoupling=1);

  const char * className(int i) {
    if (i<nClasses && i >= 0) 
      return classnames[i];
    else
      return NULL;
  }

  int getClassIndex(const char * c);
  void setStatsDebug() { showStatsDebug = 1; }

  int buildFtSelMap(int n, const char * name);
  int checkFtSelMap();

  ~smileSvmModel();
};


class DLLEXPORT cSvmSink : public cDataSink {
  private:
    const char * model;
    const char * resultRecp;
    const char * resultMessageName;
    const char * resultFile;
    const char * winningClassMethodName;
    int sendResult, printResult, saveResult;
    int printParseableResult;
    int ignoreLogitModel;
    int winningClassMethod, usePairwiseCoupling;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    smileSvmModel * modelObj;

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cSvmSink(const char *_name);

    virtual ~cSvmSink();
};


#endif // BUILD_SVMSMO

#endif // __CSVMSINK_HPP
