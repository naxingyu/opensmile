/*F******************************************************************************
 *
 * openSMILE - open Speech and Music Interpretation by Large-space Extraction
 *       the open-source Munich Audio Feature Extraction Toolkit
 * Copyright (C) 2008-2009  Florian Eyben, Martin Woellmer, Bjoern Schuller
 *
 *
 * Institute for Human-Machine Communication
 * Technische Universitaet Muenchen (TUM)
 * D-80333 Munich, Germany
 *
 *
 * If you use openSMILE or any code from openSMILE in your research work,
 * you are kindly asked to acknowledge the use of openSMILE in your publications.
 * See the file CITING.txt for details.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ******************************************************************************E*/


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