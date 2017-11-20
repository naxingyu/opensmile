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

vector Processor :  (abstract class only)
       specialised dataProcessor, which takes one frame as input an produces one frame as output
       however, each array field is processed individually as a vector
       for each field the output dimension can be set in derived components

*/


#include <core/vectorProcessor.hpp>

#define MODULE "cVectorProcessor"


SMILECOMPONENT_STATICS(cVectorProcessor)

SMILECOMPONENT_REGCOMP(cVectorProcessor)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CVECTORPROCESSOR;
  sdescription = COMPONENT_DESCRIPTION_CVECTORPROCESSOR;

  // we inherit cDataProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")

  SMILECOMPONENT_IFNOTREGAGAIN( {}
    ct->setField("processArrayFields","1 = process each array field as one vector individually (and produce one output for each input array field). Only array fields (i.e. fields with more than one element) are processed if this is set. / 0 = process complete input frame as one vector, ignoring field/element structure",1);
    ct->setField("includeSingleElementFields", "1 = if in processArrayFields (1) mode, then also include single element fields.", 0);
    ct->setField("preserveFieldNames", "1 = when processArrrayFields is disabled (0), then still preserve the input field partitioning and names in the output, but just process the whole vector instead of fields. This was the default in old versions, but now it can be controlled. This is what you would usually want, except if you want/need to combine inputs to a single large array field.", 1);
  )

  SMILECOMPONENT_MAKEINFO(cVectorProcessor);
}

SMILECOMPONENT_CREATE_ABSTRACT(cVectorProcessor)

//-----

cVectorProcessor::cVectorProcessor(const char *_name) :
  cDataProcessor(_name),
  Nfo(0), No(0),
  Nfi(0), Ni(0),
  fNi(NULL),
  fNo(NULL),
  vecO(NULL),
  confBs(NULL),
  fconf(NULL),
  fconfInv(NULL),
  Nfconf(0),
  processArrayFields(1),
  preserveFieldNames(1),
  includeSingleElementFields(0)
{

}

void cVectorProcessor::fetchConfig()
{
  cDataProcessor::fetchConfig();

  processArrayFields = getInt("processArrayFields");
  SMILE_IDBG(2,"processArrayFields = %i",processArrayFields);
  preserveFieldNames = getInt("preserveFieldNames");
  includeSingleElementFields = getInt("includeSingleElementFields");
}

/*
int cVectorProcessor::myConfigureInstance()
{
  int ret=1;
  ret *= cDataProcessor::myConfigureInstance();
  return ret;
}
*/

/*
int cVectorProcessor::configureWriter(const sDmLevelConfig *c)
{

  // you must return 1, in order to indicate configure success (0 indicated failure)
  return 1;
}

*/

int cVectorProcessor::addFconf(long bs, int field) // return value is index of assigned configuration
{
  int i;
  if (bs<=0) return -1;
  for (i=0; i<Nfi; i++) {
    if ((confBs[i] == bs)||(confBs[i]==0)) {
      confBs[i] = bs;
      fconfInv[i] = field;
      fconf[field] = i;
      if (i>=Nfconf) Nfconf = i+1;
      return i;
    }
  }
  return -1;
}


// this method should be overridden by a derived class in order to configure the output fields
/*
int cVectorProcessor::setupNamesForField(int i, const char*name, long nEl)
{
  return cDataProcessor::setupNamesForField(i,name,nEl);
//  return nEl;
}
*/

int cVectorProcessor::dataProcessorCustomFinalise()
{
  if (namesAreSet_) return 1;

  int i;

  Nfi = reader_->getLevelNf();
  if (!processArrayFields) {
    if (!preserveFieldNames) {
      Nfi=1; // this has to be one... setupNamesForField must be called only once with nEl=number of elements!
             // if we need to call setupNames... for each field to preserve the input names, but then call processVector only once (where??)
             // then we need a separate option for preserveFieldNames.. etc.
    }
    Ni = reader_->getLevelN(); // number of elements
  }

  fNi = (long*)calloc(1,sizeof(long)*Nfi); // input
  fNo = (long*)calloc(1,sizeof(long)*Nfi); // output  (Nfo <= Nfi, always!)

  fconf = (int *)malloc(sizeof(int)*Nfi);
  fconfInv = (int *)malloc(sizeof(int)*Nfi);
  confBs = (long *)calloc(1,sizeof(long)*Nfi);

  if (!processArrayFields) {

    int __N=0;
    //const char *tmp = reader->getFieldName(0,&__N);
    fconf[0] = -1;
    addFconf(Ni,0);

    if (!namesAreSet_) fNo[0] = setupNewNames(Ni);

    if (!namesAreSet_) {
      fNo[0] = 0; 
      //SMILE_IMSG(1,"XXXX array Nfi=%i Ni=%i\n",Nfi,Ni);
      i=0;
      //TODO: still a problem here: output names are only 1st field + appended name, instead we want the names to be input name + appended bit... (example: cVectorOperation with processArrayFields=0)
      // NOTE: for vectorTransform an co. we need to preserve the individual names (other components might not... or want to have setupNamesForField only called once? Check!!
      int myN = 1;
      if (preserveFieldNames) {
        myN = Nfi;
      }
      //for (i=0; i<Nfi; i++) {  // OK, changed Nfi to 1 on this line.. hope this breaks nothing else.. ( TODO::: this is NOT OK... why do we run this for each input field?? (names...?) )
      for (i=0; i<myN; i++) {  // OK, changed Nfi to 1 on this line.. hope this breaks nothing else.. ( TODO::: this is NOT OK... why do we run this for each input field?? (names...?) )
        int arrNameOffset=0;
        const char *tmp = reader_->getFieldName(i,&__N,&arrNameOffset);
        int myN2 = Ni;
        if (preserveFieldNames) {
          myN2 = __N;
        }
        long nOut = setupNamesForField(i,tmp,myN2);
        if (nOut==__N) {
          // set arrNameOffset
          writer_->setArrNameOffset(arrNameOffset);
        }
        fNo[0] += nOut; //fNo[0] += __N;
      }
    }
    else { if (fNo[0]==0) fNo[0] = Ni; }
    No = fNo[0];
//old:    Nfo=Nfi;  // ?????
//new:
    Nfo=1;

    fNi[0] = Ni;
    Nfi=1; // ????

    configureField(0,Ni,No);

  } else {
    for (i=0; i<Nfi; i++) { fconf[i] = -1; }
    //
    for (i=0; i<Nfi; i++) {
      int __N=0;
      int arrNameOffset=0;
      const char *tmp = reader_->getFieldName(i,&__N,&arrNameOffset);
      if (tmp == NULL) { SMILE_ERR(1,"reader->getFieldName(%i) failed (return value = NULL)!",i); return 0; }
      fNi[i] = __N; Ni += __N;
      if ((__N > 1) || (includeSingleElementFields)) {  // Changed to >= 1 (was > 1) ,single fields should be ok..!
        addFconf(__N,i);
        if (!namesAreSet_) {
          fNo[Nfo] = setupNamesForField(i,tmp,__N);
          if (fNo[Nfo]==__N) {
            // set arrNameOffset
            writer_->setArrNameOffset(arrNameOffset);
          }
        }
        else fNo[Nfo] = __N; // fNo[i] = __N;
        No += fNo[Nfo];
        configureField(i,__N,fNo[Nfo]);
        Nfo++;
      } else {
        SMILE_IWRN(1,"ignoring single (non-array) field '%s'!",tmp);
      }
    }
  }

  namesAreSet_ = 1;
  return 1;
}

/*
int cVectorProcessor::myFinaliseInstance()
{
  int ret=1;
  ret *= reader->finaliseInstance();
  if (!ret) return ret;

 

  for (i=0; i<Nfconf; i++) {
    //      configureField(fconfInv[i],fNi[fconfInv[i]]);
    configureField(fconfInv[i],confBs[i]);
  }

  
  return cDataProcessor::myFinaliseInstance();
}

*/

// a derived class should override this method, in order to implement the actual processing
int cVectorProcessor::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  //memcpy(dst,src, MIN(Nsrc,Ndst));
  SMILE_ERR(1,"component '%s' (type '%s') does not support data type DMEM_FLOAT (yet) !",getInstName(),getTypeName());
  return 1;
}

// a derived class should override this method, in order to implement the actual processing
int cVectorProcessor::processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  //memcpy(dst,src, MIN(Nsrc,Ndst));
  SMILE_ERR(1,"component '%s' (type '%s') does not support data type DMEM_INT (yet) !",getInstName(),getTypeName());
  return 1;
}

// a derived class should override this method, in order to implement the actual processing
int cVectorProcessor::flushVectorFloat(FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  //memcpy(dst,src, MIN(Nsrc,Ndst));
  //SMILE_ERR(1,"component '%s' (type '%s') does not support data type DMEM_FLOAT (yet) !",getInstName(),getTypeName());
  return 0;
}

// a derived class should override this method, in order to implement the actual processing
int cVectorProcessor::flushVectorInt(INT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  //memcpy(dst,src, MIN(Nsrc,Ndst));
  //SMILE_ERR(1,"component '%s' (type '%s') does not support data type DMEM_INT (yet) !",getInstName(),getTypeName());
  return 0;
}

int cVectorProcessor::myTick(long long t)
{
  SMILE_IDBG(4,"tick # %i, running vector processor",t);

  if (!(writer_->checkWrite(1))) return 0;
  // printf("'%s' checkwrite ok\n",getInstName());

  // get next frame from dataMemory
  cVector *vec = reader_->getNextFrame();
  int i;
  int iO = 0;
  int toSet = 1;
  int ret = 1;
  int res;

  int type;
  if (vec==NULL) {
    const sDmLevelConfig * c = writer_->getLevelConfig();
    if (c != NULL) {
      type = c->type;
    } else {
      return 0;
    }
  } else { 
    type = vec->type;
  }
  if (vecO == NULL)
    vecO = new cVector(No, type);
  if (customVecProcess(vec, vecO)) {
    if (vec != NULL) vecO->tmetaReplace(vec->tmeta);
    // save the output to dataMemory
    writer_->setNextFrame(vecO);
    return 1;
  }

  if (type == DMEM_FLOAT) {
    FLOAT_DMEM *dFi = NULL;
    if (vec != NULL) {
      dFi = vec->dataF;
    }
    FLOAT_DMEM *dFo = vecO->dataF;
    for (i=0; i<Nfi; i++) {
      if ((fNi[i] == 1 && includeSingleElementFields == 0 && processArrayFields == 1) || (fNi[i] < 1)) {
        continue;
      }
      if (vec != NULL) {
        if ((dFo == NULL)||(fNo[iO]<=0)) {
          SMILE_IERR(1,"output field size for field %i is 0 in call to processVectorFloat!\n  Please check if setupNewNames or setupNamesForField returns a number > 0 !!",iO);
          COMP_ERR("aborting here, since this is a serious bug in this component ...");
        }
        res = processVectorFloat(dFi, dFo, fNi[i], fNo[iO], i);
      } else {
        if ((dFo == NULL)||(fNo[iO]<=0)) {
          SMILE_IERR(1,"output field size for field %i is 0 in call to processVectorFloat!\n  Please check if setupNewNames or setupNamesForField returns a number > 0 !!",iO);
          COMP_ERR("aborting here, since this is a serious bug in this component ...");
        }
        res = flushVectorFloat(dFo, fNi[i], fNo[iO], i);
      }
      if (res == 0) ret=0;
      else if (res < 0) toSet=0;
      dFi += fNi[i];
      dFo += fNo[iO];
      iO++;
    }
  } else if (type == DMEM_INT) {
    INT_DMEM *iFi = NULL;
    if (vec != NULL) {
      iFi = vec->dataI;
    }
    INT_DMEM *iFo = vecO->dataI;
    for (i=0; i<Nfi; i++) {
      if ((fNi[i] == 1 && includeSingleElementFields == 0 && processArrayFields == 1) || (fNi[i] < 1)) {
        continue;
      }
      if (vec != NULL) {
        if ((iFo == NULL)||(fNo[iO]<=0)) {
          SMILE_IERR(1,"output field size for field %i is 0 in call to processVectorFloat!\n  Please check if setupNewNames or setupNamesForField returns a number > 0 !!",iO);
          COMP_ERR("aborting here, since this is a serious bug in this component ...");
        }
        res = processVectorInt(iFi, iFo, fNi[i], fNo[iO], i);
      } else {
        if ((iFo == NULL)||(fNo[iO]<=0)) {
          SMILE_IERR(1,"output field size for field %i is 0 in call to processVectorFloat!\n  Please check if setupNewNames or setupNamesForField returns a number > 0 !!",iO);
          COMP_ERR("aborting here, since this is a serious bug in this component ...");
        }
        res = flushVectorInt(iFo, fNi[i], fNo[iO], i);
      }
      if (res == 0) ret=0;
      else if (res < 0) toSet=0;

      iFi += fNi[i];
      iFo += fNo[iO];
      iO++;
    }
  } else {
    SMILE_ERR(1,"unknown data type: vec->type = %i! (instance: '%s')",vec->type,getInstName());
  }

  if (!ret) {
    toSet=0;
  }
  if (toSet) {    
    if (vec != NULL) vecO->tmetaReplace(vec->tmeta);
    // save to dataMemory
    writer_->setNextFrame(vecO);
  }
  return ret;
}


void cVectorProcessor::multiConfFree( void *x )
{
  int i;
  void **y = (void **)x;
  if (y!=NULL) {
    for (i=0; i<getNf(); i++) {
      if (y[i] != NULL) free(y[i]);
    }
    free(y);
  }
}

cVectorProcessor::~cVectorProcessor()
{
  if (fNi!=NULL)  free(fNi);
  if (fNo!=NULL)  free(fNo);
  if (fconf != NULL) free(fconf);
  if (fconfInv != NULL) free(fconfInv);
  if (confBs != NULL)  free(confBs);
  if (vecO!=NULL) delete vecO;
}

