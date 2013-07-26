

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

      mySonic.AddVariable("invariantMass",   "Invariant mass",         "GeV",    20,80,150 ,&(myEvent.invariantMass)              );
      mySonic.AddVariable("MET",             "Missing E_{T}",          "GeV",    20,0,200  ,&(myEvent.MET)            ,"logY=true");

  // #########################################################
  // ##   Create ProcessClasses (and associated datasets)   ##
  // #########################################################

      mySonic.AddProcessClass("foo",     "Foo",     "background", COLORPLOT_ORANGE);
           mySonic.AddDataset("foo","foo",2500,0.25);
  
      mySonic.AddProcessClass("bar",     "Bar",     "background", COLORPLOT_AZURE);
           mySonic.AddDataset("bar","bar",2500,0.5);

      mySonic.AddProcessClass("baz",     "Baz",     "signal",     COLORPLOT_GREEN);
           mySonic.AddDataset("baz","baz",2500,0.5);

      mySonic.AddProcessClass("data" ,   "Data",    "data",       COLORPLOT_BLACK);
           mySonic.AddDataset("data","data",1,25000);
  
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

     // Schedule plots
     mySonic.SchedulePlots("1DSuperpRenorm");
     mySonic.SchedulePlots("DataMCComparison");

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
             if (myEvent.processType == 0) { currentDataset = string("foo");  }
        else if (myEvent.processType == 1) { currentDataset = string("bar");  }
        else if (myEvent.processType == 2) { currentDataset = string("baz");  }
        else if (myEvent.processType == 3) { currentDataset = string("data");  }        
     
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
  mySonic.WritePlots("plots/");

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

