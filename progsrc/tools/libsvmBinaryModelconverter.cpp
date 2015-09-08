/*F***************************************************************************
 * openSMILE - the Munich open source Multimedia Interpretation by Large-scale
 * Extraction toolkit
 * 
 * (c) 2008-2011, Florian Eyben, Martin Woellmer, Bjoern Schuller @ TUM-MMK
 * (c) 2012-2013, Florian Eyben, Felix Weninger, Bjoern Schuller @ TUM-MMK (c)
 * 2013-2014 audEERING UG, haftungsbeschränkt. All rights reserved.
 * 
 * Any form of commercial use and redistribution is prohibited, unless another
 * agreement between you and audEERING exists. See the file LICENSE.txt in the
 * top level source directory for details on your usage rights, copying, and
 * licensing conditions.
 * 
 * See the file CREDITS in the top level directory for information on authors
 * and contributors. 
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

