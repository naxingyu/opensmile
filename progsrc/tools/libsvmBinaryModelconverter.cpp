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

#include <classifiers/libsvm/svm.h>

int main(int argc, char**argv) 
{
  if (argc < 3) {
    printf("\nUSAGE: %s <ASCII model file> <binary model file>\n",argv[0]);
    printf(" Convert an ASCII LibSVM model file into a binary LibSVM model file.\n\n");
    printf("USAGE: %s - <binary model file> <ASCII model file>\n",argv[0]);
    printf(" Convert a binary LibSVM model file into an ASCII LibSVM model file.\n\n");
    return -1;
  }

  if (argv[1][0] == '-') {  // bin -> ASCII
 
    if (argc < 4) {

      printf("USAGE: %s - <binary model file> <ASCII model file>\n",argv[0]);
      printf(" Convert a binary LibSVM model file into an ASCII LibSVM model file.\n\n");
      return -1;
    
    }

    printf("Loading binary model '%s'... ",argv[2]);

    svm_model * m = svm_load_binary_model(argv[2]);

    if (m==NULL) {
      printf("\nERROR: failed loading model '%s'!\n",argv[2]);
      return -2;
    }

    printf ("OK\n");
    printf("Saving ASCII model '%s'... ",argv[3]);
 
    int r = svm_save_model(argv[3],m);
    if (!r) printf ("OK\n");
    else { 
      printf("ERROR: failed saving ASCII model '%s' (code=%i)\n",argv[3],r); 
      return -3;
    }

    svm_destroy_model(m);
 
  } else { // ASCII -> bin

    printf("Loading ASCII model '%s'... ",argv[1]);

    svm_model * m = svm_load_model(argv[1]);

    if (m==NULL) {
      printf("\nERROR: failed loading model '%s'!\n",argv[1]);
      return -2;
    }

    printf ("OK\n");
    printf("Saving binary model '%s'... ",argv[2]);
 
    int r = svm_save_binary_model(argv[2],m);
    if (!r) printf ("OK\n");
    else { 
      printf("ERROR: failed saving binary model '%s' (code=%i)\n",argv[2],r); 
      return -3;
    }

    svm_destroy_model(m);

  }

  return 0;
}

