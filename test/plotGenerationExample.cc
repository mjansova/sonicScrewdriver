// ################
// #   Includes   #
// ################

#include "common.h"

// Sonic screwdriver
#include "../interface/SonicScrewdriver.h" 
#include "../interface/tables/TableDataMC.h" 
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

microEvent myEvent;

// ######################################################
// #   Additional variables to be computed on the fly   #
// ######################################################

float leptonPtOverMET()
{
    return myEvent.leptonPt / myEvent.MET;
}

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
   if (myEvent.leptonFlavor == 0) return true;
   else                           return false;
}

bool muChannel()
{
   if (myEvent.leptonFlavor == 1) return true;
   else                           return false;
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
      SonicScrewdriver s;
      s.LoadJsonConfig("./testStyleConfig.json");

  // ##########################
  // ##   Create Variables   ##
  // ##########################

      s.AddVariable("invariantMass",    "Invariant mass",   "GeV",    40,60,160,   &(myEvent.invariantMass),  "noUnderflowInFirstBin,noOverflowInLastBin");
      s.AddVariable("MET",              "Missing E_{T}",    "GeV",    40,0,400,    &(myEvent.MET),            "logY"     );
      s.AddVariable("leptonPt",         "p_{T}(lepton)",    "GeV",    30,0,150,    &(myEvent.leptonPt),       "noUnderflowInFirstBin");
      s.AddVariable("mMuf",             "True muf mass",    "GeV",    11,114,136,  &(myEvent.mMuf),           "noUnderflowInFirstBin");
      
      // Variable with value defined by a function
      s.AddVariable("leptonPtOverMET",  "Lepton pT / E^#text{miss}_{T}",    "",    40,0,10,  &(leptonPtOverMET),  "noUnderflowInFirstBin");
     
      // Variable with a custom binning
      float customBinning[17] = {0.0,5.0,10.0,15.0,20.0,25.0,30.0,35.0,40.0,50.0,60.0,70.0,80.0,90.0,100.0,125.0,150.0};
      s.AddVariable("leptonPt_customBinning",      "p_{T}(lepton)",    "GeV",    16,customBinning,    &(myEvent.leptonPt),       "noUnderflowInFirstBin");

  // #########################################################
  // ##   Create ProcessClasses (and associated datasets)   ##
  // #########################################################

	  // Backgrounds

      s.AddProcessClass("foo",     "Foo",     "background", COLORPLOT_RED);
           s.AddDataset("foo","foo",50000,0.495);
  
      s.AddProcessClass("bar",     "Bar",     "background", COLORPLOT_ORANGE);
           s.AddDataset("bar","bar",50000,0.495);

	  // Signal(s)

      s.AddProcessClass("muf",     "Muf",               "signal",  COLORPLOT_BLUE, "no1DPlots");
           s.AddDataset("muf","muf",50000 / 11,0.01);
      s.AddProcessClass("muf_125", "Muf (m=125 GeV)",    "signal",  COLORPLOT_BLUE);

	  // Data

      s.AddProcessClass("data" ,   "Data",    "data",       COLORPLOT_BLACK);
           s.AddDataset("data","data",-1,-1);

      s.SetLumi(50000);
  
  // ##########################
  // ##    Create Regions    ##
  // ##########################

     vector<Cut> preSelection { Cut("invariantMass", '>', 90) };
        s.AddRegion("preSelection","Pre-selection",               preSelection);
    
     // Signal region is a daughter of "preSelection", with additional cuts
     vector<Cut> signalRegion { Cut("MET", '>', 50), Cut("leptonPt",'>', 50) };
        s.AddRegion("signalRegion","Signal region","preSelection",signalRegion);
     
  // ##########################
  // ##   Create Channels    ##
  // ##########################
   
     s.AddChannel("combinedChannel","e/#mu-channel",&combinedChannel);
     //s.AddChannel("eChannel",       "e-channel",    &eChannel       );
     //s.AddChannel("muChannel",      "#mu-channel",  &muChannel      );

  // ########################################
  // ##       Create histograms and        ##
  // ##  schedule type of plots to produce ##
  // ########################################
  
     // Create histograms
     s.Create1DHistos();
     
     s.Add2DHisto("invariantMass","leptonPt");
     s.Add2DHisto("mMuf","invariantMass");

     // Schedule plots
     
     s.SchedulePlots("1DSuperimposed");
     s.SchedulePlots("1DStack");
     s.SchedulePlots("1DDataMCComparison");
     s.SchedulePlots("1DFigureOfMerit","var=leptonPt,cutType=keepHighValues");
     s.SchedulePlots("2D");

  // ########################################
  // ##        Run over the events         ##
  // ########################################
  
     vector<string> datasetsList;
     s.GetDatasetList(&datasetsList);

     cout << "   > Reading datasets " << endl;

     for (unsigned int d = 0 ; d < datasetsList.size() ; d++)
     {
         // Get current dataset
         string currentDataset = datasetsList[d];
         string currentProcessClass = s.GetProcessClass(currentDataset); 
    
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
            float weight = s.GetDatasetLumiWeight(currentDataset);
              
            // Fill all the variables
            s.AutoFillProcessClass(currentProcessClass,weight);

            // Also fill muf_125
            if ((currentProcessClass == "muf") && (myEvent.mMuf == 125))
                s.AutoFillProcessClass("muf_125",weight);

         }
         printProgressBar(nEntries,nEntries,currentDataset);
         cout << endl;
     }

     // ###################################
     // ##   Make plots and write them   ##
     // ###################################

     cout << "   > Making and writing plots ... " << endl;

     s.MakePlots();
     s.WritePlots("./plots/");

     vector<string> regionsForTable  = { "preSelection", "signalRegion" };
     TableDataMC(&s,regionsForTable,"combinedChannel","includeSignal").Print("yieldTable.tab",2);



     printBoxedMessage("Plot generation completed");

     return (0);

}

