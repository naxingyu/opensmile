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


#include <stdio.h>
#include <stdlib.h>

/*
  sample code to make a 1-nn model from mfcc feature files (bianry format.. ??)
*/

int main(int argc, char**argv) 
{
  if (argc < 3) {
    printf("Usage: a.out <input mfcc file(s) ...> <output model file>\n");
    return -1;
  }
  int i; long Nv=0;
  float N,nVec;
  float *means = NULL;

  for (i=1; i<argc-1; i++) {
 
  FILE *f= fopen(argv[i],"rb");
  if (f==NULL) { printf("could not open '%s', skipping.\n"); continue; }

  
  fread( &N, sizeof(float), 1, f );
  fread( &nVec, sizeof(float), 1, f );
  int i,j;
  if (means == NULL) means=calloc(1,sizeof(float)*(int)N);
  float *vec=calloc(1,sizeof(float)*(int)N);
  for (i=0; i<(int)nVec; i++) {
    fread( vec, sizeof(float), (int)N, f );
    for(j=0; j<(int)N; j++) {
      means[j] += vec[j];
    }
    Nv++;
  }
  fclose(f);
  free(vec);

  }

  int j;
  for(j=0; j<(int)N; j++) {
    means[j] /= (float)Nv;
  }

  FILE *f = fopen(argv[2],"wb");
  N--;
  fwrite( &N, sizeof(float), 1, f);
  nVec = (float)1.0;
  fwrite( &nVec, sizeof(float), 1, f);
  fwrite( (means+1), sizeof(float), (int)(N-1), f);
  fclose(f);

  free(means);
}

