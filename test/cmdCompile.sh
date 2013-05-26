#!/bin/bash

g++ -I../ `root-config --cflags` -c makeTreeForTest.cc
g++ -o makeTreeForTest makeTreeForTest.o -L/usr/local/lib/root `root-config --libs`

g++ -I../ `root-config --cflags` -c example2.cc
g++ -o example2 example2.o -L$PWD/../ -lSonicScrewdriver `root-config --libs`


