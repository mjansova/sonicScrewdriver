ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs) 

DEFINES       = 

CXX           = g++
CXXFLAGS      = -std=c++0x
CXXFLAGS      += -O3 -Wall -fPIC $(DEFINES) -Wno-unused-result -Wshadow  -pg -fopenmp -g 
CXXFLAGS      += $(ROOTCFLAGS) -I./

LD            = g++ 
LDFLAGS       = -pg -O3 -Wall -fPIC -Wl,-undefined,error -fopenmp -g
LDFLAGS       += $(ROOTLIBS)

SOFLAGS       = -shared
LIBS          = 

# Uncomment this line if you want to use a script to parse & colorize gcc output
# (You can also export this variable from your bashrc)
#GCCPARSER     = 2>&1 | python ~/littleScripts/colorGcc.py

#------------------------------------------------------------------------------
SOURCES       = $(wildcard src/*.cc)
OBJECTS       = $(SOURCES:.cc=.o)
#------------------------------------------------------------------------------

lib:  libSonicScrewdriver.so

libSonicScrewdriver.so: $(OBJECTS) 
	@echo "Building libSonicScrewdriver..."
	$(LD) -L${ROOTSYS}/lib $+ -o $@ $(LIBS) $(SOFLAGS) $(LDFLAGS)

test:
	@make -C test test

browseTest:
	@make -C test browse

clean:
	@echo "Cleaning..."
	@rm -f $(OBJECTS) *.so


%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LIBS) $(GCCPARSER)

 
.PHONY : test


