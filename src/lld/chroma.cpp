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


/*  openSMILE component: cChroma

Component to compute chroma features from a semi-tone spectrum. 

This component is based on original code from Moritz Dausinger (wave2chroma). 
The openSMILE chroma component was implemented by Christoph Kozielski.

*/


#include <lld/chroma.hpp>

#define MODULE "cChroma"

/*Library:
sComponentInfo * registerMe(cConfigManager *_confman) {
  cDataProcessor::registerComponent(_confman);
}
*/

SMILECOMPONENT_STATICS(cChroma)

SMILECOMPONENT_REGCOMP(cChroma)
{
  if (_confman == NULL) return NULL;
  int rA = 0;

  sconfman = _confman;
  scname = COMPONENT_NAME_CCHROMA;
  sdescription = COMPONENT_DESCRIPTION_CCHROMA;

  // we inherit cVectorProcessor configType and extend it:
  ConfigType *ct = new ConfigType( *(sconfman->getTypeObj("cVectorProcessor")) , scname );
  if (ct == NULL) {
    SMILE_WRN(4,"cVectorProcessor config Type not found!");
    rA=1;
  }
  if (rA==0) {
    ct->setField("nameAppend",NULL,"chroma");
    ct->setField("copyInputName",NULL,0);
    ct->setField("octaveSize","The size of an octave, i.e. the number of output bins, or the interval to which the input bins are mapped via warping.",12);
    ct->setField("silThresh","Chroma 'floor', i.e. silence threshold. If at least on element of the (unnormalised) chroma vector falls below the threshold, the vector will not be normalised, and a 0 vector will be output instead.",0.001); // chroma toolbox uses 0.001 ?
    ConfigInstance *Tdflt = new ConfigInstance( scname, ct, 1 );
    sconfman->registerType(Tdflt);
  } 

  SMILECOMPONENT_MAKEINFO(cChroma);
}

SMILECOMPONENT_CREATE(cChroma)

//-----

cChroma::cChroma(const char *_name) :
  cVectorProcessor(_name), silThresh(0.0)
{

}

void cChroma::fetchConfig()
{
  cVectorProcessor::fetchConfig();
  
  silThresh = (FLOAT_DMEM)getDouble("silThresh");

  octaveSize = getInt("octaveSize");
  SMILE_IDBG(2,"octaveSize = %i",octaveSize);
}


int cChroma::setupNamesForField(int i, const char*name, long nEl)
{
  return cVectorProcessor::setupNamesForField(i,name,octaveSize);
}



// a derived class should override this method, in order to implement the actual processing
int cChroma::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
// input: power of tonespec...!

  int i,j;
  double sum = 0.0;
  int silFlag = 0;
  
  nOctaves = Nsrc / octaveSize;
  if (Nsrc % octaveSize == 0) {
    for (i = 0;i < octaveSize; i++) {
      dst[i] = 0.0;
      for (j = 0;j < nOctaves; j++) {
        dst[i] += src[j * octaveSize + i];
      }
      //sum += (dst[i]*dst[i]); // <- old pre 1.0.0 code
      if (dst[i] < silThresh) silFlag = 1;
      sum += (double)dst[i];
    }
    if ((sum != 0.0)&&(!silFlag)) {
      //double power = sqrt(sum);  // <- old pre 1.0.0 code
      for (i = 0;i < octaveSize; i++)
        dst[i] = dst[i] / (FLOAT_DMEM)sum;
        //dst[i] = dst[i] / (FLOAT_DMEM)power;  // <- old pre 1.0.0 code
    } else {
      for (i = 0;i < octaveSize; i++) dst[i] = 0.0;
    }
  } else {
    SMILE_IERR(1,"cannot compute Chroma features from an input frame with a size (%i) no an integer multiple of 'octaveSize' (%i)! Check your tonespec configuration!",Nsrc,octaveSize);
  }

  return 1;
}

cChroma::~cChroma()
{
}
