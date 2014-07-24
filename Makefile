ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs) 

DEFINES       = 

CXX           = g++
CXXFLAGS      = -std=c++0x
CXXFLAGS      += -O -Wall -fPIC $(DEFINES) -Wno-unused-result -Wshadow
CXXFLAGS      += $(ROOTCFLAGS) -I./

LD            = g++ 
LDFLAGS       = -g -O -Wall -fPIC -Wl,--no-undefined 
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

all:  libSonicScrewdriver.so

libSonicScrewdriver.so: $(OBJECTS) 
	@echo "Building libSonicScrewdriver..."
	$(LD) -L${ROOTSYS}/lib $+ -o $@ $(LIBS) $(SOFLAGS) $(LDFLAGS)

test:
	@make -C test test

clean:
	@echo "Cleaning..."
	@rm -f $(OBJECTS)


%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LIBS) $(GCCPARSER)

 
.PHONY : test


