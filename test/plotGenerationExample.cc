

// System includes

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

    Float_t processType;
    Float_t leptonsFlavor;

}
microEvent;

// Global pointer used for the region/channel selectors
microEvent* myEventPointer;

// #########################################################################""
// #########################################################################""

bool baselineSelector();
bool noChannelSelector();

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
      SonicScrewdriver mySonic;

     // Create a container for the event
     microEvent myEvent;
     myEventPointer = &myEvent;

  // ##########################
  // ##   Create Variables   ##
  // ##########################

      mySonic.AddVariable("invariantMass",   "Invariant mass",         "GeV",    40,60,160 ,&(myEvent.invariantMass)              );
      mySonic.AddVariable("MET",             "Missing E_{T}",          "GeV",    40,0,400  ,&(myEvent.MET)            ,"logY=true");

  // #########################################################
  // ##   Create ProcessClasses (and associated datasets)   ##
  // #########################################################

	  // Backgrounds

      mySonic.AddProcessClass("foo",     "Foo",     "background", COLORPLOT_ORANGE);
           mySonic.AddDataset("foo1","foo",25000,0.475);
  
      mySonic.AddProcessClass("bar",     "Bar",     "background", COLORPLOT_AZURE);
           mySonic.AddDataset("bar1","bar",25000,0.475);

	  // Signal(s)

      mySonic.AddProcessClass("muf",     "Muf",     "signal",     COLORPLOT_GREEN);
           mySonic.AddDataset("muf1","muf",25000,0.05);

	  // Data

      mySonic.AddProcessClass("data" ,   "Data",    "data",       COLORPLOT_BLACK);
           mySonic.AddDataset("data1","data",1,25000);
  
  // ##########################
  // ##    Create Regions    ##
  // ##########################

     mySonic.AddRegion("baseline","Baseline",&baselineSelector);

  // ##########################
  // ##   Create Channels    ##
  // ##########################
   
     mySonic.AddChannel("noChannel","",&noChannelSelector);

  // ########################################
  // ##       Create histograms and        ##
  // ##  schedule type of plots to produce ##
  // ########################################
  
     // Create histograms
     mySonic.Create1DHistos();

     // Set options

     mySonic.SetGlobalBoolOption  ("1DSuperpRenorm",    "includeSignal",                    true   );
     
     mySonic.SetGlobalStringOption("1DStack",           "includeSignal",                    "stack");
     mySonic.SetGlobalFloatOption ("1DStack",           "factorSignal",                     2.0    );

     mySonic.SetGlobalStringOption("DataMCComparison",  "includeSignal",                    "stack");
     mySonic.SetGlobalFloatOption ("DataMCComparison",  "factorSignal",                     1.0    );
     
     mySonic.SetGlobalFloatOption ("FigureOfMerit",     "backgroundSystematicUncertainty",  0.15   );

     // Schedule plots
     
     mySonic.SchedulePlots("1DSuperpRenorm");
     mySonic.SchedulePlots("1DStack");
     mySonic.SchedulePlots("1DDataMCComparison");
     
     mySonic.SchedulePlots("1DFigureOfMerit","var=invariantMass,cutType=keepHighValues");

  // ########################################
  // ##        Run over the events         ##
  // ########################################

     // Open the tree
     //
     TFile* f = new TFile("tree.root");
     TTree* theTree;
     f->GetObject("theTree",theTree);
     theTree->SetBranchAddress("theBranch",&myEvent);
 
     // Loop over the events

     for (int i = 0 ; i < theTree->GetEntries() ; i++)
     {
        // Get the i-th entry
        theTree->GetEntry(i);
     
        // Find which dataset the event is from
        string currentDataset;
             if (myEvent.processType == 0) { currentDataset = string("foo1");  }
        else if (myEvent.processType == 1) { currentDataset = string("bar1");  }
        else if (myEvent.processType == 2) { currentDataset = string("muf1");  }
        else if (myEvent.processType == 3) { currentDataset = string("data1");  }        
     
        // Reweight event to luminosity
        float weight = mySonic.GetDatasetLumiWeight(currentDataset);
          
        // Get the processClass associated to the current dataset
        string currentProcessClass = mySonic.GetProcessClass(currentDataset);
     
        // Fill all the variables with autoFill-mode activated
        mySonic.AutoFillProcessClass(currentProcessClass,weight);

     }

  // ###################################
  // ##   Make plots and write them   ##
  // ###################################
  
  mySonic.MakePlots();
  mySonic.WritePlots("plots/","CMS Internal #sqrt{s} = 8 TeV, L = 20 fb^{-1}");

  cout << endl;
  cout << "   ┌──────────────────────────────┐ " << endl;
  cout << "   │   Plot generation completed  │  " << endl;
  cout << "   └──────────────────────────────┘" << endl; 
  cout << endl;

  return (0);

}

bool baselineSelector()
{
    return true;
}

bool noChannelSelector()
{
    return true;
}

