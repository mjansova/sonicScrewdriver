
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

    Float_t invariantMass;
    Float_t MET;
    Float_t leptonsFlavor;

}
microEvent;

// Global pointer used for the region/channel selectors
microEvent* myEventPointer;

// #########################################################################""
// #########################################################################""

// Regions
bool baselineSelector();

// Channels
bool combinedChannelSelector();
bool eChannelSelector();
bool muChannelSelector();

int main (int argc, char *argv[])
{

  cout << endl;
  cout << "   ┌──────────────────────────────┐ " << endl;
  cout << "   │   Starting plot generation   │  " << endl;
  cout << "   └──────────────────────────────┘" << endl; 
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

      myScrewdriver.AddVariable("invariantMass",   "Invariant mass",         "GeV",    40,60,160 ,&(myEvent.invariantMass)              );
      myScrewdriver.AddVariable("MET",             "Missing E_{T}",          "GeV",    40,0,400  ,&(myEvent.MET)            ,"logY=true");

  // #########################################################
  // ##   Create ProcessClasses (and associated datasets)   ##
  // #########################################################

	  // Backgrounds

      myScrewdriver.AddProcessClass("foo",     "Foo",     "background", COLORPLOT_ORANGE);
           myScrewdriver.AddDataset("foo","foo",12500,0.495);
  
      myScrewdriver.AddProcessClass("bar",     "Bar",     "background", COLORPLOT_AZURE);
           myScrewdriver.AddDataset("bar","bar",12500,0.495);

	  // Signal(s)

      myScrewdriver.AddProcessClass("muf",     "Muf",     "signal",     COLORPLOT_GREEN);
           myScrewdriver.AddDataset("muf","muf",12500,0.01);

	  // Data

      myScrewdriver.AddProcessClass("data" ,   "Data",    "data",       COLORPLOT_BLACK);
           myScrewdriver.AddDataset("data","data",1,12500);
  
  // ##########################
  // ##    Create Regions    ##
  // ##########################

     myScrewdriver.AddRegion("baseline","Pre-selection",&baselineSelector);

  // ##########################
  // ##   Create Channels    ##
  // ##########################
   
     myScrewdriver.AddChannel("combinedChannel","e/#mu-channel",&combinedChannelSelector);
     myScrewdriver.AddChannel("eChannel",       "e-channel",    &eChannelSelector       );
     myScrewdriver.AddChannel("muChannel",      "#mu-channel",  &muChannelSelector      );

  // ########################################
  // ##       Create histograms and        ##
  // ##  schedule type of plots to produce ##
  // ########################################
  
     // Create histograms
     myScrewdriver.Create1DHistos();
     
     myScrewdriver.Add2DHisto("invariantMass","MET");

     // Set options

     myScrewdriver.SetGlobalBoolOption  ("1DSuperpRenorm",    "includeSignal",                    true   );
     
     myScrewdriver.SetGlobalStringOption("1DStack",           "includeSignal",                    "stack");
     myScrewdriver.SetGlobalFloatOption ("1DStack",           "factorSignal",                     2.0    );

     myScrewdriver.SetGlobalStringOption("DataMCComparison",  "includeSignal",                    "stack");
     myScrewdriver.SetGlobalFloatOption ("DataMCComparison",  "factorSignal",                     1.0    );
     
     myScrewdriver.SetGlobalFloatOption ("FigureOfMerit",     "backgroundSystematicUncertainty",  0.15   );

     // Schedule plots
     
     myScrewdriver.SchedulePlots("1DSuperpRenorm");
     myScrewdriver.SchedulePlots("1DStack");
     myScrewdriver.SchedulePlots("1DDataMCComparison");
     myScrewdriver.SchedulePlots("1DFigureOfMerit","var=invariantMass,cutType=keepHighValues");
     myScrewdriver.SchedulePlots("2D");

     // Config plots

     myScrewdriver.SetGlobalStringOption("Plot", "infoTopRight", "CMS Internal");
     myScrewdriver.SetGlobalStringOption("Plot", "infoTopLeft",  "#sqrt{s} = 8 TeV, L = 20 fb^{-1}");

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

bool baselineSelector()
{
    return true;
}

bool combinedChannelSelector()
{
    return true;
}

bool eChannelSelector()
{
   if (myEventPointer->leptonsFlavor == 0) return true;
   else                                    return false;
}

bool muChannelSelector()
{
   if (myEventPointer->leptonsFlavor == 1) return true;
   else                                    return false;
}


