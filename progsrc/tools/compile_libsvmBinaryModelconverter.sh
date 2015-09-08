#!/bin/sh

echo g++ -g -DNOSMILE -I../../src/include -c ../../src/classifiers/libsvm/svm.cpp -o svm.o
g++ -g -DNOSMILE -c -I../../src/include ../../src/classifiers/libsvm/svm.cpp -o svm.o
echo g++ -g -DNOSMILE -I../../src/include libsvmBinaryModelconverter.cpp svm.o -o modelconverter
g++ -g -DNOSMILE -I../../src/include libsvmBinaryModelconverter.cpp svm.o -o modelconverter

