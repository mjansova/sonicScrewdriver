ObjSuf        = o
SrcSuf        = cc
ExeSuf        = 
DllSuf        = so
OutPutOpt     = -o
HeadSuf       = h

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs) 
ROOTGLIBS     = $(shell root-config --libs)

# Linux with egcs
DEFINES       = -DNO_ORCA_CLASSES
CXX           = g++
CXXFLAGS	  = -O -Wall -fPIC $(DEFINES)
LD		      = g++ 
LDFLAGS 	  = -g -O -Wall -fPIC
SOFLAGS		  = -shared

CXXFLAGS	+= $(ROOTCFLAGS) -I./
LIBS		= $(ROOTLIBS)  -lEG 
GLIBS		= $(ROOTGLIBS)
#------------------------------------------------------------------------------
SOURCES		= $(wildcard src/*.cc)
HEADERS		= $(wildcard interface/*.h)
OBJECTS		= $(SOURCES:.$(SrcSuf)=.$(ObjSuf))
SOBJECTS	= $(SOURCES:.$(SrcSuf)=.$(DllSuf))


all:  libSonicScrewdriver.so

clean:
	@echo "Cleaning..."
	@rm -f $(OBJECTS)

.SUFFIXES: .$(SrcSuf) .C .o .so

libSonicScrewdriver.so: $(OBJECTS) 
	@echo "Building libSonicScrewdriver..."
	$(LD) -L${ROOTSYS}/lib $+ -o $@ $(LIBS) $(SOFLAGS) $(LDFLAGS)

