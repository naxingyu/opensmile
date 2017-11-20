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


#ifndef __KWSJ_RESULT_H
#define __KWSJ_RESULT_H


// STL includes for the queue
#include <queue>

// A message received from the turn detector.
struct TurnMsg {
    TurnMsg() : time(0), vIdx(0), isForcedTurnEnd(0) {}
    double time;     // smile time or whatever
    long vIdx;        // vector index
    int isForcedTurnEnd;  // forced turn end = at end of input
    long midx;
};

// A queue of turn detector messages.
typedef std::queue<TurnMsg> TurnMsgQueue;


/* keyword spotter result message */
#define MAXNUMKW 100
// old message:
typedef struct kresult {
  int numOfKw;
  int turnDuration;
  double kwStartTime[MAXNUMKW]; // time in seconds
  const char* keyword[MAXNUMKW];
  float kwConf[MAXNUMKW];
} Kresult;


// new message:
#define JULIUSRESULT_FLAG_KEYWORD   1

class juliusResult {
  public:
    int numW;
    char ** word; // word array
    unsigned int * wordFlag;
    char ** phstr; // phoneme string for each word (phonemes are space separated)
    float * conf;
    float * start;
    float * end;
    long userFlag;
    double amScore, lmScore, score;
    double duration;

    juliusResult() : numW(0), word(NULL), phstr(NULL), conf(NULL), start(NULL), end(NULL), wordFlag(NULL), amScore(0.0), lmScore(0.0), score(0.0), duration(0.0), userFlag(0) {}

    juliusResult( int N ) : numW(N), word(NULL), phstr(NULL), conf(NULL), start(NULL), end(NULL), wordFlag(NULL), amScore(0.0), lmScore(0.0), score(0.0), duration(0.0), userFlag(0) {
      if (numW > 0) {
        word = (char**) calloc(1, sizeof(char*) * numW * 2);
        phstr = word + numW;
        conf = (float*) calloc(1, sizeof(float) * numW * 3);
        start = conf + numW;
        end = start + numW;
        wordFlag = (unsigned int*)calloc(1,sizeof(unsigned int) * numW);
      }
    }

    ~juliusResult() {
      int i;
      if (word != NULL) {
        for (i=0; i<numW*2; i++) { if (word[i] != NULL) free(word[i]); }
        free(word);
      }
      if (conf != NULL) { free(conf); }
      if (wordFlag != NULL) { free(wordFlag); }
    }
};

#endif // __KWSJ_RESULT_H
