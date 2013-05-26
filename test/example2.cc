
#include <iomanip>
#include <iostream>
#include <time.h>

#include <cmath>
#include <iomanip>
#include <iostream>
#include <time.h>

#include <TFile.h>
#include <TMarker.h>
#include <TTree.h>
#include <TBranch.h>
#include <TLorentzVector.h>

using namespace std;

#include "../interface/SonicScrewdriver.h" 

typedef struct
{

    Float_t invariantMass;
    Float_t pT;

    Float_t channelType;

}
EventStruct;


// #########################################################################""
// #########################################################################""

EventStruct* myEventPointer;

bool baselineSelector();
bool WSelector();
bool ZSelector();
bool noChannelSelector();

int main (int argc, char *argv[])
{

  cout << endl;
  cout << "   ┌───────────────────┐ " << endl;
  cout << "   │   Starting test   │  " << endl;
  cout << "   └───────────────────┘" << endl; 
  cout << endl;

  // ####################
  // ##   Init tools   ##
  // ####################
  
	  // Create a sonic Screwdriver
 	 SonicScrewdriver mySonic;

  	// Create a container for the event
	  EventStruct myEvent;
	  myEventPointer = &myEvent;

  // ##########################
  // ##   Create Variables   ##
  // ##########################

 	 mySonic.AddVariable("invariantMass",   "Invariant mass",         "GeV",    50,60,110,&(myEvent.invariantMass)              );
 	 mySonic.AddVariable("pT",              "p_{T}",                  "GeV",    20,0,60  ,&(myEvent.pT)           	,"logY=true");

  // #########################################################
  // ##   Create ProcessClasses (and associated datasets)   ##
  // #########################################################

 	 mySonic.AddProcessClass("W",    "W+jets",     "background",COLORPLOT_ORANGE);
    	   mySonic.AddDataset("W","W",33000,0.5);
  
 	 mySonic.AddProcessClass("Z",    "Z+jets",     "background",COLORPLOT_AZURE);
     	  mySonic.AddDataset("Z","Z",33000,0.5);
            
  	 mySonic.AddProcessClass("data" ,"data","data",COLORPLOT_BLACK);
         mySonic.AddDataset("data1","data",1, 33000);
  
  // ##########################
  // ##    Create Regions    ##
  // ##########################

     mySonic.AddRegion("baseline","Baseline",&baselineSelector);
   	 	mySonic.ScheduleVariablesForRegion("baseline","invariantMass,pT");

	 mySonic.AddRegion("Wzone","W zone",&WSelector);
   	 	mySonic.ScheduleVariablesForRegion("Wzone","invariantMass,pT");
     
	 mySonic.AddRegion("Zzone","Z zone",&ZSelector);
   	 	mySonic.ScheduleVariablesForRegion("Zzone","invariantMass,pT");
   
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
	 mySonic.SchedulePlots("DataMCComparison");
  	 mySonic.SchedulePlots("1DSuperpRenorm");

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

     int nData = 0;
     int nW = 0;

  for (int i = 0 ; i < theTree->GetEntries() ; i++)
  {
	  // Get the i-th entry
      theTree->GetEntry(i);
 
	 // Find which dataset the event is from
     string currentDataset;
          if (myEvent.channelType == 0) { currentDataset = string("W"); 	nW++;    }
     else if (myEvent.channelType == 1) { currentDataset = string("Z");              }
     else if (myEvent.channelType == 2) { currentDataset = string("data1"); nData++; }        
  
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
  cout << "   ┌───────────────────┐  " << endl;
  cout << "   │   Test completed  │  " << endl;
  cout << "   └───────────────────┘  " << endl; 
  cout << endl;

  return (0);

}

bool baselineSelector()
{
	return true;
}

bool WSelector()
{
    if (myEventPointer->invariantMass > 86) return false;
	else return baselineSelector();
}

bool ZSelector()
{
    if (myEventPointer->invariantMass < 86) return false;
	else return baselineSelector();
}

bool noChannelSelector()
{
	return true;
}

