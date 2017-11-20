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

arffSink
ARFF file output (for WEKA)

*/


#include <iocore/arffSink.hpp>

#define MODULE "cArffSink"

/*Library:
sComponentInfo * registerMe(cConfigManager *_confman) {
  cDataSink::registerComponent(_confman);
}
*/

SMILECOMPONENT_STATICS(cArffSink)

//sComponentInfo * cArffSink::registerComponent(cConfigManager *_confman)
SMILECOMPONENT_REGCOMP(cArffSink)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CARFFSINK;
  sdescription = COMPONENT_DESCRIPTION_CARFFSINK;

  // we inherit cDataSink configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSink")

  SMILECOMPONENT_IFNOTREGAGAIN_BEGIN
    ct->setField("filename","The filename of the ARFF file to write to. An empty filename disables this sink component.","smileoutput.arff");
    ct->setField("lag","output data <lag> frames behind",0,0,0);
    ct->setField("append","1 = append to an existing file, or create a new file; 0 = overwrite an existing file, or create a new file",0);
    ct->setField("relation","The name of the ARFF relation (@relation tag)","smile");
    ct->setField("instanceBase","If this string is not empty and not equal to '-', cArffSink prints an instance name attribute <instanceBase_Nr>, where Nr is the number (continuous index) of current instance","");
    ct->setField("instanceName","If this string is not empty and not equal to '-', cArffSink prints an instance name attribute <instanceName> for all instances. instanceName overwrites instanceBase.","");
    ct->setField("instanceNameFromMetadata","1/0 = yes/no. Use instanceName from metadata, if available. If this is not available, printing of an instance name depends on 'instanceName' and 'instanceBase' attributes.",0);
    ct->setField("useTargetsFromMetadata","1/0 = yes/no. Use string of target values/classes from vector metadata instead of building it from the 'target' array. If no metadata is present the target string will be built from the 'target' array",0);
    ct->setField("number","'Synonym' for the 'frameIndex' option: 1 = print an instance number (= frameIndex) attribute (continuous index) (1/0 = yes/no)",1);
    ct->setField("timestamp","'Synonym' for the 'frameTime' option: 1 = print a timestamp (=frameTime) attribute (1/0 = yes/no)",1);
    ct->setField("frameIndex","1 = print an instance number (= frameIndex) attribute (continuous index) (1/0 = yes/no) (same as 'number' option)",1);
    ct->setField("frameTime","1 = print a timestamp (=frameTime) attribute (1/0 = yes/no) (same as 'timestamp' option)",1);
    ct->setField("frameTimeAdd","Add this value to the framTime values (unit: seconds)",0);
    ct->setField("frameLength","1 = print a frame length attribute (1/0 = yes/no).",0);

    ConfigType * classType = new ConfigType("arffClass");
    classType->setField("name", "The name of the target attribute", "class");
    classType->setField("type", "The type of the target attribute: 'numeric', 'string', or nominal (= list of classes, enclosed in {} )", "numeric");
    ct->setField("class","This is an array defining the optional class target attributes (grund-truths that you want to have included in your arff file along with your features). It is an array for multiple targets/classes. See also the 'target' array.", classType, ARRAY_TYPE);
    ct->setField("printDefaultClassDummyAttribute", "1/0 = yes/no, print default class dummy attribute if no class attributes are given in the class[] array. Default is 1 for backward compatibility. Set to 0 to remove default class attribute.", 1);

    ConfigType * targetType = new ConfigType("arffTarget");
    targetType->setField("instance", "An array containing a target for each instance ", "",ARRAY_TYPE);
    targetType->setField("all", "Assign this one target to all processed instances. You can use this option if you pass only one instance to cArffSink when openSMILE is run. (This option is used by a lot batch feature extraction scripts)", "");
    
    ct->setField("target","The ground truth targets (classes) for each target (class) attribute",targetType,ARRAY_TYPE);
 
    // overwrite default for this option inherited from the parent class
    ct->setField("errorOnNoOutput",(const char*)NULL,1);

    // TODO: custom fields, import from file, merge arff streams...
    
  SMILECOMPONENT_IFNOTREGAGAIN_END

  SMILECOMPONENT_MAKEINFO(cArffSink);
}

SMILECOMPONENT_CREATE(cArffSink)

//-----

cArffSink::cArffSink(const char *_name) :
  cDataSink(_name),
  prname(0),
  filehandle(NULL),
  filename(NULL),
  nInst(0),
  nClasses(0),
  inr(0),
  classname(NULL), classtype(NULL),
  targetall(NULL), targetinst(NULL),
  disabledSink_(false)
{
}

void cArffSink::fetchConfig()
{
  cDataSink::fetchConfig();
  
  filename = getStr("filename");
  if (filename == NULL || *filename == 0 || (*filename == '?' && *(filename+1) == 0)) {
    SMILE_IMSG(2, "No filename given, disabling this sink component.");
    disabledSink_ = true;
    errorOnNoOutput_ = 0;
  }

  lag = getInt("lag");
  SMILE_IDBG(3,"lag = %i",lag);

  append = getInt("append");
  if (append) { SMILE_IDBG(3,"append to file is enabled"); }

  printDefaultClassDummyAttribute = getInt("printDefaultClassDummyAttribute");
  
  if (isSet("frameTime")) timestamp = getInt("frameTime");
  else timestamp = getInt("timestamp");
  if (timestamp) { SMILE_IDBG(3,"printing timestamp attribute enabled"); }

  if (isSet("frameIndex")) number = getInt("frameIndex");
  else number = getInt("number");
  if (number) { SMILE_IDBG(3,"printing instance number (=frame number) attribute enabled"); }

  frameLength = getInt("frameLength");

  relation = getStr("relation");
  SMILE_IDBG(3,"ARFF relation = '%s'",relation);

  instanceBase = getStr("instanceBase");
  SMILE_IDBG(3,"instanceBase = '%s'",instanceBase);

  instanceName = getStr("instanceName");
  SMILE_IDBG(3,"instanceName = '%s'",instanceName);
  
  int i;
  nClasses = getArraySize("class");
  //printf("nclasses: %i\n", nClasses);
  classname = (char**)calloc(1,sizeof(char*)*nClasses);
  classtype = (char**)calloc(1,sizeof(char*)*nClasses);
  for (i=0; i<nClasses; i++) {
    const char *tmp = getStr_f(myvprint("class[%i].name",i));
    if (tmp!=NULL) classname[i] = strdup(tmp);
    tmp = getStr_f(myvprint("class[%i].type",i));
    if (tmp!=NULL) classtype[i] = strdup(tmp);
  }
/*
    ConfigType * classType = new ConfigType("arffClass");
    classType->setField("name", "name of target", "class");
    classType->setField("type", "numeric, or nominal (= list of classes)", "numeric");
    ct->setField("class","definition of class target attributes (array for multiple targets/classes)", classType, ARRAY_TYPE);

    ConfigType * targetType = new ConfigType("arffTarget");
    targetType->setField("instance", "array containing targets for each instance", 0,ARRAY_TYPE);
    targetType->setField("all", "one common target for all processed instances", 0);
*/

  if (getArraySize("target") != nClasses) {
    SMILE_ERR(1,"number of targets (%i) is != number of class attributes (%i)!",getArraySize("target"),nClasses);
  } else {
    targetall = (char**)calloc(1,sizeof(char*)*nClasses);
    targetinst = (char***)calloc(1,sizeof(char**)*nClasses);
    nInst = -2;
    for (i=0; i<nClasses; i++) {
      char *tmp = myvprint("target[%i].instance",i);
      const char *t = getStr_f(myvprint("target[%i].all",i));
      if (t!=NULL) targetall[i] = strdup(t);
      long ni = getArraySize(tmp);
      if (nInst==-2) nInst = ni; // -1 if no array
      else {
        if (nInst != ni) COMP_ERR("number of instances in target[].instance array is not constant among all targets! %i <> %i",nInst,ni);
      }
      int j;
      if (nInst > 0) {
        targetinst[i] = (char**)calloc(1,sizeof(char*)*nInst);
        for (j=0; j<nInst; j++) {
          t = getStr_f(myvprint("%s[%i]",tmp,j));
          if (t!=NULL) targetinst[i][j] = strdup(t);
        }
      }
      free(tmp);
    }
  }

  instanceNameFromMetadata = getInt("instanceNameFromMetadata");
  SMILE_IDBG(2,"instanceNameFromMetadata = %i",instanceNameFromMetadata);
  useTargetsFromMetadata = getInt("useTargetsFromMetadata");
  SMILE_IDBG(2,"useTargetsFromMetadata = %i",useTargetsFromMetadata);
  frameTimeAdd= getDouble("frameTimeAdd");
//    ct->setField("target","targets (classes) for each target (class) attribute",targetType,ARRAY_TYPE);
}

int cArffSink::myFinaliseInstance()
{
  int ap=0;
  if (disabledSink_) {
    filehandle = NULL;
    return 1;
  }
  int ret = cDataSink::myFinaliseInstance();
  if (ret==0) return 0;

  if (append) {
    // check if file exists:
    filehandle = fopen(filename, "r");
    if (filehandle != NULL) {
      fclose(filehandle);
      filehandle = fopen(filename, "a");
      ap=1;
    } else {
      filehandle = fopen(filename, "w");
    }
  } else {
    filehandle = fopen(filename, "w");
  }
  if (filehandle == NULL) {
    COMP_ERR("Error opening file '%s' for writing (component instance '%s', type '%s')",filename, getInstName(), getTypeName());
  }

  if (instanceNameFromMetadata) {
    prname = 1;
  } else {
    if ((instanceBase!=NULL)&&(strlen(instanceBase)>0)&&
        (instanceBase[0] != '-' || strlen(instanceBase) > 1)
       ) prname = 2;
    if ((instanceName!=NULL)&&(strlen(instanceName)>0)&&
        (instanceName[0] != '-' || strlen(instanceName) > 1)
       ) prname = 1;
  }
  long _N = reader_->getLevelN();

  if (!ap) {
    // write arff header ....
    fprintf(filehandle, "@relation %s%s%s",relation,NEWLINE,NEWLINE);
    if (prname) {
      fprintf(filehandle, "@attribute name string%s",NEWLINE);
    }
    if (number) {
      fprintf(filehandle, "@attribute frameIndex numeric%s",NEWLINE);
    }
    if (timestamp) {
      fprintf(filehandle, "@attribute frameTime numeric%s",NEWLINE);
    }
    if (frameLength) {
      fprintf(filehandle, "@attribute frameLength numeric%s",NEWLINE);
    }

    long i;
    if (_N > 10000) {
      SMILE_IMSG(2,"writing ARFF header (%i features), this may take a while (it is a lot of data and heavy weight string formatting) ...",_N);
    } else {
      SMILE_IMSG(2,"writing ARFF header (%i features)...",_N);
    }
    for(i=0; i<_N; i++) {
      char *tmp = reader_->getElementName(i);
      fprintf(filehandle, "@attribute %s numeric%s",tmp,NEWLINE);
      free(tmp);
      if ((i>0)&&(i%20000==0)) {
        SMILE_IMSG(2,"Status: %i feature names written.",i);
      }
    }
    if (_N > 6000) {
      SMILE_IMSG(2,"finished writing ARFF header.");
    }

    // TODO: classes..... as config file options...
    if (nClasses > 0) {
      for (i=0; i<nClasses; i++) {
        if (classtype[i] == NULL) fprintf(filehandle, "@attribute %s numeric%s",classname[i],NEWLINE);
        else fprintf(filehandle, "@attribute %s %s%s",classname[i],classtype[i],NEWLINE);
      }
    } else {
      // default dummy class attribute...
      if (printDefaultClassDummyAttribute) {
        fprintf(filehandle, "@attribute class {0,1,2,3}%s",NEWLINE);
      }
    }
    fprintf(filehandle, "%s@data%s%s",NEWLINE,NEWLINE,NEWLINE);
    fflush(filehandle);
  }
  return ret;
}


int cArffSink::myTick(long long t)
{
  if (filehandle == NULL) return 0;

  SMILE_DBG(4,"tick # %i, reading value vector (lag=%i):",t,lag);
  cVector *vec= reader_->getFrameRel(lag);  //new cVector(nValues+1);
  if (vec == NULL) return 0;
  //else reader->nextFrame();

  long vi = vec->tmeta->vIdx;
  double tm = vec->tmeta->time;
  double len = vec->tmeta->lengthSec;

  if (vec->tmeta->metadata.iData[1] == 1234) {
    instanceName = vec->tmeta->metadata.text;
    if (prname==1) {
      fprintf(filehandle,"%s,",instanceName);
    }
  } else {
    if (prname==1) {
      fprintf(filehandle,"'%s',",instanceName);
    } else if (prname==2) {
      fprintf(filehandle,"'%s_%i',",instanceBase,vi);
    }
  }

  if (number) fprintf(filehandle,"%i,",vi);
  if (timestamp) fprintf(filehandle,"%f,",tm+frameTimeAdd);
  if (frameLength) fprintf(filehandle,"%f,",len);
  
  // now print the vector:
  int i;
  fprintf(filehandle,"%e",vec->dataF[0]);
  for (i=1; i<vec->N; i++) {
    fprintf(filehandle,",%e",vec->dataF[i]);
    //printf("  (a=%i vi=%i, tm=%fs) %s.%s = %f\n",reader->getCurR(),vi,tm,reader->getLevelName(),vec->name(i),vec->dataF[i]);
  }

  // classes: 
  if ((vec->tmeta->metadata.iData[1] == 1234)&&(vec->tmeta->metadata.custom != NULL)&&(vec->tmeta->metadata.customLength > 0)&&(useTargetsFromMetadata)) {
    // TODO: check the order of the fields....
    fprintf(filehandle, ",%s", (const char *)(vec->tmeta->metadata.custom));
  } else {
    if (nClasses > 0) {
      if (nInst>0) {
        if (inr >= nInst) {
          SMILE_WRN(3,"more instances writte to ARFF file, then there are targets available for (%i)!",nInst);
          if (targetall != NULL) {
            for (i=0; i<nClasses; i++) {
              if (targetall[i] != NULL)
                fprintf(filehandle,",%s",targetall[i]);
              else
                fprintf(filehandle,",NULL");
            }
          } else {
            for (i=0; i<nClasses; i++) {
              fprintf(filehandle,",NULL");
            }
          }
          //inr++;
        } else {
          for (i=0; i<nClasses; i++) {
            fprintf(filehandle,",%s",targetinst[i][inr]);
          }
          inr++;
        }
      } else {
        if (targetall != NULL) {
          for (i=0; i<nClasses; i++) {
            if (targetall[i] != NULL)
              fprintf(filehandle,",%s",targetall[i]);
            else
              fprintf(filehandle,",NULL");
          }
        } else {
          for (i=0; i<nClasses; i++) {
            fprintf(filehandle,",NULL");
          }
        }
      }
    } else {
      // dummy class attribute, always 0
      if (printDefaultClassDummyAttribute) {
        fprintf(filehandle,",0");
      }
    }
  }

  fprintf(filehandle,"%s",NEWLINE);

  int err = fflush(filehandle);
  if (err == EOF) {
    SMILE_IERR(1,"error writing to file '%s' (code: %i)",filename,errno);
    COMP_ERR("aborting");
    return 0;
  }

  nWritten_++;

  // tick success
  return 1;
}


cArffSink::~cArffSink()
{
  if (filehandle != NULL) {
    fclose(filehandle);
    filehandle = NULL;
  }
  int i;
  if (classname!=NULL) {
    for (i=0; i<nClasses; i++) if (classname[i] != NULL) free(classname[i]);
    free(classname);
  }
  if (classtype!=NULL) {
    for (i=0; i<nClasses; i++) if (classtype[i] != NULL) free(classtype[i]);
    free(classtype);
  }
  if (targetall!=NULL) {
    for (i=0; i<nClasses; i++) if (targetall[i] != NULL) free(targetall[i]);
    free(targetall);
  }
  if (targetinst!=NULL) {
    int j;
    for (i=0; i<nClasses; i++) {
      if (targetinst[i] != NULL) {
        for (j=0; j<nInst; j++) {
          if (targetinst[i][j] != NULL) free(targetinst[i][j]);
        }
        free(targetinst[i]);
      }
    }
    free(targetinst);
  }
}

