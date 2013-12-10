
#ifndef Common_h
#define Common_h

// C/C++ headerrs
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <map>


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
#include <TVirtualPad.h>
#include <TPaveText.h>
#include <TStyle.h>
#include <TError.h>
#include <THStack.h>
#include <TPaletteAxis.h>

using namespace std;

#define DEBUG_MSG cout << "DEBUG (" << __FILE__ << ", l." << __LINE__ << ") "
#define WARNING_MSG cout << "WARNING (" << __FILE__ << ", l." << __LINE__ << ") "

#define COLORPLOT_GREEN		kSpring-1
#define COLORPLOT_GREEN2	kGreen+1
#define COLORPLOT_TEAL		kTeal
#define COLORPLOT_CYAN		kCyan

#define COLORPLOT_ORANGE 	kOrange-2
#define COLORPLOT_ORANGE2 	kORange+7
#define COLORPLOT_RED 		kRed 
#define COLORPLOT_PINK		kPink+10
#define COLORPLOT_MAGENTA   kMagenta

#define COLORPLOT_AZURE 	kAzure-2
#define COLORPLOT_BLUE 		kBlue
#define COLORPLOT_VIOLET 	kViolet+1

#define COLORPLOT_BLACK		kBlack
#define COLORPLOT_GRAY		kGray

string floatToString(float input);

#endif
