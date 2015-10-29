#ifndef Common_h
#define Common_h

// C/C++ headerrs
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <map>
#include <set>


// ROOT headers
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TAxis.h>
#include <TLine.h>
#include <TArrow.h>
#include <TVirtualPad.h>
#include <TPaveText.h>
#include <TStyle.h>
#include <TError.h>
#include <THStack.h>
#include <TColor.h>
#include <TPaletteAxis.h>
#include <TMath.h>

using namespace std;

#define DEBUG_MSG cerr << "DEBUG (" << __FILE__ << ", l." << __LINE__ << ") "
#define WARNING_MSG cerr << "WARNING (" << __FILE__ << ", l." << __LINE__ << ") "

#define SONICSCREWDRIVER_RANGE_RESCALER_NOLOG    1.3
#define SONICSCREWDRIVER_RANGE_RESCALER_WITHLOG  9.0

#define COLORPLOT_RED          kRed-7
#define COLORPLOT_CYAN         kCyan-3
#define COLORPLOT_ORANGE       kOrange-2
#define COLORPLOT_MAGENTA      kMagenta-5
#define COLORPLOT_BLUE         kAzure-2
#define COLORPLOT_GREEN        kSpring-1
#define COLORPLOT_GREEN2       kGreen+1
#define COLORPLOT_BLACK        kBlack
#define COLORPLOT_GRAY         kGray

string floatToString(float input);

void replaceSubstring(string& str, const string oldStr, const string newStr);

#endif
