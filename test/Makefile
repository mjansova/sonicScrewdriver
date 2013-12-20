ObjSuf        = o
SrcSuf        = cc

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs) 

# Linux with egcs
DEFINES       = 
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC $(DEFINES) -Wno-unused-result
LD            = g++ 

CXXFLAGS      += $(ROOTCFLAGS) -I../
LDFLAGS       =
LDLIBS  	  = $(ROOTLIBS) -lEG -L../ -lSonicScrewdriver

#------------------------------------------------------------------------------
SOURCES     = $(wildcard *.cc)
OBJECTS     = $(SOURCES:.$(SrcSuf)=.$(ObjSuf))
EXEC        = $(SOURCES:.$(SrcSuf)=)

all: $(EXEC)

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $<

%: %.o
	$(LD) $(LDFLAGS) -o $@ $< $(LDLIBS) 

.PHONY : trees plots test

trees: makeTreeForExample
	@./makeTreeForExample

plots: plotGenerationExample
	@./plotGenerationExample

test: trees plots

clean:
	@echo "Cleaning..."
	@rm -f $(EXEC) $(OBJECTS)

.SUFFIXES: .$(SrcSuf) .C .o .so

