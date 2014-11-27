ObjSuf        = o
SrcSuf        = cc
LogSuf        = log
ExeSuf        = run

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs) 

DEFINES       = 

CXX           = g++
CXXFLAGS      = -std=c++0x
CXXFLAGS      += -O -Wall -fPIC $(DEFINES) -Wno-unused-result -Wshadow
CXXFLAGS      += $(ROOTCFLAGS) -I./
CXXFLAGS      += -I../

LD            = g++ 
LDFLAGS       = -g -O -Wall -fPIC -Wl,--no-undefined 
LDFLAGS       += $(ROOTLIBS)  -lEG -lTMVA
LDFLAGS       += -l:`pwd`/../libSonicScrewdriver.so

SOFLAGS       = -shared
LIBS          = 

# Uncomment this line if you want to use a script to parse & colorize gcc output
# (You can also export this variable from your bashrc)
#GCCPARSER     = 2>&1 | python ~/littleScripts/colorGcc.py

#------------------------------------------------------------------------------
SOURCES       = $(wildcard *.$(SrcSuf))
OBJECTS       = $(SOURCES:.$(SrcSuf)=.$(ObjSuf))
EXECUTABLES   = $(SOURCES:.$(SrcSuf)=.$(ExeSuf))
LOGS          = $(SOURCES:.$(SrcSuf)=.$(LogSuf))
#------------------------------------------------------------------------------

all: $(EXECUTABLES)

test: trees plots

# Target to produce the example trees
trees: makeTreeForExample.$(ExeSuf)
	@./makeTreeForExample.$(ExeSuf) | tee makeTreeForExample.$(LogSuf)

# Target to produce the example plots
plots: plotGenerationExample.$(ExeSuf)
	@./plotGenerationExample.$(ExeSuf) | tee plotGenerationExample.$(LogSuf)

# Target to browse the plots
browse:
	PLOTS_TO_BROWSE=`ls plots/*.root` root -l .browse

clean:
	@echo "Cleaning..."
	@rm -f $(OBJECTS) $(EXECUTABLES) $(LOGS) *.tab

%.$(ExeSuf): %.$(SrcSuf)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS) $(LIBS) $(GCCPARSER)

.PHONY : trees plots test browse clean









