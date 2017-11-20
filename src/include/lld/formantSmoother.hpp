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

Input: candidates produced by a pitchBase descendant

*/


#ifndef __CFORMANTSMOOTHER_HPP
#define __CFORMANTSMOOTHER_HPP

#include <core/smileCommon.hpp>
#include <core/vectorProcessor.hpp>

#define BUILD_COMPONENT_FormantSmoother
#define COMPONENT_DESCRIPTION_CFORMANTSMOOTHER "This component performs temporal formant smoothing. Input: candidates produced by a formant** component AND(!) - appended - an F0final or voicing field (which is 0 for unvoiced frames and non-zero for voiced frames). Output: Smoothed formant frequency contours."
#define COMPONENT_NAME_CFORMANTSMOOTHER "cFormantSmoother"

#define FPOSTSMOOTHING_NONE   0
#define FPOSTSMOOTHING_SIMPLE 1
#define FPOSTSMOOTHING_MEDIAN 2

#undef class
class DLLEXPORT cFormantSmoother : public cVectorProcessor {
  private:
    int firstFrame;
    int no0f0;
    int medianFilter0;
    int postSmoothing, postSmoothingMethod;
    int saveEnvs;

    long F0fieldIdx, formantFreqFieldIdx, formantBandwidthFieldIdx, formantFrameIntensField;
    long nFormantsIn;

    int nFormants, bandwidths, formants, intensity;
    //int onsFlag, onsFlagO;
    //int octaveCorrection;

    FLOAT_DMEM *median0WorkspaceF0cand;
    FLOAT_DMEM *lastFinal;

    FLOAT_DMEM *fbin;
    FLOAT_DMEM *fbinLastVoiced;
    // NOTE: difficulty when analysing data from multiple pdas : 
    // the scores and voicing probs. may be scaled differently, although they should all be in the range 0-1
    // thus, a direct comparion of these may not be feasible
    // We thus start comparison for each field (pda output) individually and then (TODO) merge information from multiple pdas

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    FLOAT_DMEM *voicingCutoff;

    virtual void fetchConfig();
	  virtual int setupNewNames(long nEl);
    
    //virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cFormantSmoother(const char *_name);
    
    virtual ~cFormantSmoother();
};




#endif // __CFORMANTSMOOTHER_HPP
