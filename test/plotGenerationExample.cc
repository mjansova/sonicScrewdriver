// ################
// #   Includes   #
// ################

#include "common.h"

// Sonic screwdriver
#include "../interface/SonicScrewdriver.h" 
using namespace theDoctor;

// ####################
// #   Event format   #
// ####################

typedef struct
{
    float invariantMass;
    float MET;
    float leptonPt;
    float leptonFlavor;
    int   mMuf;
}
microEvent;

// Global pointer used for the region/channel selectors
microEvent* myEventPointer;

// ###############
// #   Regions   #
// ###############

// Regions
vector<Cut> preSelection
{
    Cut("invariantMass", '>', 90)
};

vector<Cut> signalRegion
{
    Cut("MET",     '>', 50),
    Cut("leptonPt",'>', 50)
};

// ################
// #   Channels   #
// ################

// Channels
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

// ####################
// #   Main program   #
// ####################

int main (int argc, char *argv[])
{

  printBoxedMessage("Starting plot generation");

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

      myScrewdriver.AddVariable("invariantMass", "Invariant mass",   "GeV",    40,60,160,   &(myEvent.invariantMass),  "noUnderflowInFirstBin,noOverflowInLastBin");
      myScrewdriver.AddVariable("MET",           "Missing E_{T}",    "GeV",    40,0,400,    &(myEvent.MET),            "logY"     );
      myScrewdriver.AddVariable("leptonPt",      "p_{T}(lepton)",    "GeV",    30,0,150,    &(myEvent.leptonPt),       "noUnderflowInFirstBin");
      myScrewdriver.AddVariable("mMuf",          "True muf mass",    "GeV",    11,114,136,  &(myEvent.mMuf),           "noUnderflowInFirstBin");
      
      float customBinning[17] = {0.0,5.0,10.0,15.0,20.0,25.0,30.0,35.0,40.0,50.0,60.0,70.0,80.0,90.0,100.0,125.0,150.0};
      myScrewdriver.AddVariable("leptonPt_customBinning",      "p_{T}(lepton)",    "GeV",    16,customBinning,    &(myEvent.leptonPt),       "noUnderflowInFirstBin");

  // #########################################################
  // ##   Create ProcessClasses (and associated datasets)   ##
  // #########################################################

	  // Backgrounds

      myScrewdriver.AddProcessClass("foo",     "Foo",     "background", COLORPLOT_RED);
           myScrewdriver.AddDataset("foo","foo",50000,0.495);
  
      myScrewdriver.AddProcessClass("bar",     "Bar",     "background", COLORPLOT_ORANGE);
           myScrewdriver.AddDataset("bar","bar",50000,0.495);

	  // Signal(s)

      myScrewdriver.AddProcessClass("muf",     "Muf",               "signal",  COLORPLOT_BLUE, "no1DPlots");
           myScrewdriver.AddDataset("muf","muf",50000 / 11,0.01);
      myScrewdriver.AddProcessClass("muf_125", "Muf (m=125 GeV)",    "signal",  COLORPLOT_BLUE);

	  // Data

      myScrewdriver.AddProcessClass("data" ,   "Data",    "data",       COLORPLOT_BLACK);
           myScrewdriver.AddDataset("data","data",-1,50000);
  
  // ##########################
  // ##    Create Regions    ##
  // ##########################

     myScrewdriver.AddRegion("preSelection","Pre-selection",               preSelection);
     myScrewdriver.AddRegion("signalRegion","Signal region","preSelection",signalRegion,"showCuts");

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
     myScrewdriver.SetGlobalBoolOption  ("DataMCComparison",  "showBackgroundUncertainty",        true   );
     myScrewdriver.SetGlobalBoolOption  ("DataMCRatio",       "includeSignal",                    true   );
     
     myScrewdriver.SetGlobalFloatOption ("FigureOfMerit",     "backgroundSystematicUncertainty",  0.15   );

     // Schedule plots
     
     myScrewdriver.SchedulePlots("1DSuperimposed");
     myScrewdriver.SchedulePlots("1DStack");
     myScrewdriver.SchedulePlots("1DDataMCComparison");
     myScrewdriver.SchedulePlots("1DFigureOfMerit","var=leptonPt,cutType=keepHighValues");
     myScrewdriver.SchedulePlots("2D");

     // Config plots

     myScrewdriver.SetGlobalStringOption("Plot", "infoTopRight", "Work in progress");
     myScrewdriver.SetGlobalStringOption("Plot", "infoTopLeft",  "#sqrt{s} = 8 TeV, L = 20 fb^{-1}");
     
     myScrewdriver.SetGlobalBoolOption("Plot", "exportPdf", true);
     myScrewdriver.SetGlobalBoolOption("Plot", "exportEps", false);
     myScrewdriver.SetGlobalBoolOption("Plot", "exportPng", false);

  // ########################################
  // ##        Run over the events         ##
  // ########################################
  
     vector<string> datasetsList;
     myScrewdriver.GetDatasetList(&datasetsList);

     cout << "   > Reading datasets " << endl;

     for (unsigned int d = 0 ; d < datasetsList.size() ; d++)
     {
         // Get current dataset
         string currentDataset = datasetsList[d];
         string currentProcessClass = myScrewdriver.GetProcessClass(currentDataset); 
    
         // Open dataset tree
         TTree* theTree;
         TFile* f = new TFile(("trees/"+currentDataset+".root").c_str());
         f->GetObject("theTree",theTree);
         theTree->SetBranchAddress("theBranch",&myEvent);
      
         // Loop over the events
         int nEntries = theTree->GetEntries();
         for (int i = 0 ; i < nEntries ; i++)
         {
          
            if (i % (nEntries / 50) == 0) printProgressBar(i,nEntries,currentDataset);

            // Get the i-th entry
            theTree->GetEntry(i);
            
            // Reweight event to luminosity
            float weight = myScrewdriver.GetDatasetLumiWeight(currentDataset);
              
            // Fill all the variables
            myScrewdriver.AutoFillProcessClass(currentProcessClass,weight);

            // Also fill muf_125
            if ((currentProcessClass == "muf") && (myEvent.mMuf == 125))
                myScrewdriver.AutoFillProcessClass("muf_125",weight);

         }
         printProgressBar(nEntries,nEntries,currentDataset);
         cout << endl;
     }

     // ###################################
     // ##   Make plots and write them   ##
     // ###################################

     cout << "   > Making and writing plots ... " << endl;

     myScrewdriver.MakePlots();
     myScrewdriver.WritePlots("./plots/");

     printBoxedMessage("Plot generation completed");

     return (0);

}

