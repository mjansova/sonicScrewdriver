
// System includes

#include <stdlib.h>
#include <iomanip>
#include <cmath>
#include <iostream>
#include <time.h>
using namespace std;

// Root includes

#include <TFile.h>
#include <TMarker.h>
#include <TTree.h>
#include <TBranch.h>
#include <TLorentzVector.h>

// Sonic screwdriver

#include "../interface/SonicScrewdriver.h" 
using namespace theDoctor;

// Structure used to read the tree

typedef struct
{
    Float_t mMuf;
    Float_t invariantMass;
    Float_t MET;
    Float_t leptonPt;
    Float_t leptonFlavor;

}
microEvent;

// Global pointer used for the region/channel selectors
microEvent* myEventPointer;

// #########################################################################""
// #########################################################################""

// Regions
bool preSelection_allmMuf();
bool preSelection();

// Channels
bool combinedChannel();
bool eChannel();
bool muChannel();

int main (int argc, char *argv[])
{

  cout << endl;
  cout << "   ┌──────────────────────────────┐  " << endl;
  cout << "   │   Starting plot generation   │  " << endl;
  cout << "   └──────────────────────────────┘  " << endl; 
  cout << endl;

  // ####################
  // ##   Init tools   ##
  // ####################
  
     // Create a sonic Screwdriver
      SonicScrewdriver myScrewdriver;

     // Create a container for the event
     microEvent myEvent;
     myEventPointer = &myEvent;

  // ##########################
  // ##   Create Variables   ##
  // ##########################

      myScrewdriver.AddVariable("invariantMass",   "Invariant mass",         "GeV",    40,60,160     ,&(myEvent.invariantMass)              );
      myScrewdriver.AddVariable("MET",             "Missing E_{T}",          "GeV",    40,0,400      ,&(myEvent.MET)            ,"logY=true");
      myScrewdriver.AddVariable("leptonPt",        "p_{T}(lepton)",          "GeV",    30,0,150      ,&(myEvent.leptonPt)       ,"");
      myScrewdriver.AddVariable("mMuf",            "True muf mass",          "GeV",    11,114,136    ,&(myEvent.mMuf)           ,"");

  // #########################################################
  // ##   Create ProcessClasses (and associated datasets)   ##
  // #########################################################

	  // Backgrounds

      myScrewdriver.AddProcessClass("foo",     "Foo",     "background", COLORPLOT_ORANGE);
           myScrewdriver.AddDataset("foo","foo",50000,0.495);
  
      myScrewdriver.AddProcessClass("bar",     "Bar",     "background", COLORPLOT_AZURE);
           myScrewdriver.AddDataset("bar","bar",50000,0.495);

	  // Signal(s)

      myScrewdriver.AddProcessClass("muf",     "Muf",     "signal",     COLORPLOT_GREEN);
           myScrewdriver.AddDataset("muf","muf",50000 / 11,0.01);

	  // Data

      myScrewdriver.AddProcessClass("data" ,   "Data",    "data",       COLORPLOT_BLACK);
           myScrewdriver.AddDataset("data","data",-1,50000);
  
  // ##########################
  // ##    Create Regions    ##
  // ##########################

     myScrewdriver.AddRegion("preSelection","Pre-selection, m(muf) = 125 GeV",&preSelection);
     myScrewdriver.AddRegion("preSelection_allmMuf","Pre-selection",&preSelection_allmMuf);

  // ##########################
  // ##   Create Channels    ##
  // ##########################
   
     myScrewdriver.AddChannel("combinedChannel","e/#mu-channel",&combinedChannel);
     //myScrewdriver.AddChannel("eChannel",       "e-channel",    &eChannel       );
     //myScrewdriver.AddChannel("muChannel",      "#mu-channel",  &muChannel      );

  // ########################################
  // ##       Create histograms and        ##
  // ##  schedule type of plots to produce ##
  // ########################################
  
     // Create histograms
     myScrewdriver.Create1DHistos();
     
     myScrewdriver.Add2DHisto("invariantMass","leptonPt");
     myScrewdriver.Add2DHisto("mMuf","invariantMass");

     // Set options

     myScrewdriver.SetGlobalBoolOption  ("1DSuperimposed",    "includeSignal",                    true   );
     
     myScrewdriver.SetGlobalStringOption("1DStack",           "includeSignal",                    "stack");
     myScrewdriver.SetGlobalFloatOption ("1DStack",           "factorSignal",                     2.0    );

     myScrewdriver.SetGlobalStringOption("DataMCComparison",  "includeSignal",                    "stack");
     myScrewdriver.SetGlobalFloatOption ("DataMCComparison",  "factorSignal",                     1.0    );
     
     myScrewdriver.SetGlobalFloatOption ("FigureOfMerit",     "backgroundSystematicUncertainty",  0.15   );

     // Schedule plots
     
     myScrewdriver.SchedulePlots("1DSuperimposed");
     myScrewdriver.SchedulePlots("1DStack");
     myScrewdriver.SchedulePlots("1DDataMCComparison");
     myScrewdriver.SchedulePlots("1DFigureOfMerit","var=invariantMass,cutType=keepHighValues");
     myScrewdriver.SchedulePlots("2D");
     //myScrewdriver.SchedulePlots("1DFrom2DProjection","varX=invariantMass,varY=leptonPt,projectionType=mean,tagY=meanLeptonPt,labelY=Mean Lepton Pt");
      myScrewdriver.SchedulePlots("1DFrom2DProjection",string("varX=mMuf,varY=invariantMass")
                                                    +",projectionType=mean"
                                                    +",tagY=meanMass,labelY=Mean invariant mass");
     myScrewdriver.SchedulePlots("1DFrom2DProjection",string("varX=mMuf,varY=invariantMass")
                                                    +",projectionType=cutOptimalFigureOfMeritForVarXBeingSignalParameter,cutType=keepHighValues"
                                                    +",tagY=bestCut,labelY=Best cut");

     // Config plots

     myScrewdriver.SetGlobalStringOption("Plot", "infoTopRight", "CMS Internal");
     myScrewdriver.SetGlobalStringOption("Plot", "infoTopLeft",  "#sqrt{s} = 8 TeV, L = 20 fb^{-1}");
     
     myScrewdriver.SetGlobalBoolOption("Plot", "exportPdf", false);
     myScrewdriver.SetGlobalBoolOption("Plot", "exportEps", false);
     myScrewdriver.SetGlobalBoolOption("Plot", "exportPng", false);

  // ########################################
  // ##        Run over the events         ##
  // ########################################
  
     vector<string> datasetsList;
     myScrewdriver.GetDatasetList(&datasetsList);

     cout << "   > Running on dataset : " << endl;

     for (unsigned int d = 0 ; d < datasetsList.size() ; d++)
     {
         // Get current dataset
         string currentDataset = datasetsList[d];
         string currentProcessClass = myScrewdriver.GetProcessClass(currentDataset); 
    
         cout << "                    " << currentDataset << endl; 

         // Open dataset tree
         TTree* theTree;
         TFile* f = new TFile((string("trees/")+currentDataset+".root").c_str());
         f->GetObject("theTree",theTree);
         theTree->SetBranchAddress("theBranch",&myEvent);
      
         // Loop over the events
         for (int i = 0 ; i < theTree->GetEntries() ; i++)
         {

            // Get the i-th entry
            theTree->GetEntry(i);
         
            // Reweight event to luminosity
            float weight = myScrewdriver.GetDatasetLumiWeight(currentDataset);
              
            // Fill all the variables
            myScrewdriver.AutoFillProcessClass(currentProcessClass,weight);

         }
     }

     // ###################################
     // ##   Make plots and write them   ##
     // ###################################

     cout << "   > Making and writing plots ... " << endl;

     system("mkdir -p ./plots/");
     myScrewdriver.MakePlots();
     myScrewdriver.WritePlots("./plots/");

     cout << endl;
     cout << "   ┌──────────────────────────────┐ " << endl;
     cout << "   │   Plot generation completed  │ " << endl;
     cout << "   └──────────────────────────────┘ " << endl; 
     cout << endl;

     return (0);

}

// ##########################################
// ##   Regions and channels definitions   ##
// ##########################################

bool preSelection()
{
    if ((myEventPointer->mMuf == -1) 
     || (myEventPointer->mMuf == 125)) return true;
    else return false;
}

bool preSelection_allmMuf()
{
    return true;
}

bool combinedChannel()
{
    return true;
}

bool eChannel()
{
   if (myEventPointer->leptonFlavor == 0) return true;
   else                                   return false;
}

bool muChannel()
{
   if (myEventPointer->leptonFlavor == 1) return true;
   else                                   return false;
}


