#!/bin/bash

g++ -I../ `root-config --cflags` -c makeTreeForExample.cc
g++ -o makeTreeForExample makeTreeForExample.o -L/usr/local/lib/root `root-config --libs`

g++ -I../ `root-config --cflags` -c plotGenerationExample.cc
g++ -o plotGenerationExample plotGenerationExample.o -L$PWD/../ -lSonicScrewdriver `root-config --libs`


