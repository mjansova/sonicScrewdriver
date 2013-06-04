#ifndef SonicScrewdriver_h
#define SonicScrewdriver_h

#include "interface/Common.h"

#include "interface/HistoScrewdriver.h"
#include "interface/PlotScrewdriver.h"

#include "interface/Region.h"
#include "interface/Channel.h"

namespace theDoctor 
{


    class SonicScrewdriver 
    {
      
     public:
     
      SonicScrewdriver();
      ~SonicScrewdriver();

      // ########################
      // # Variables management #
      // ########################

      //
      // *** Add a variable to the knownledge of this Screwdriver.
      //     
      //     The nBins/min/max are the default value used for all the plots
      //     which will be using this variable.
      //
      //     The autoFillPointer is a pointer to the value used when
      //     "automatically" filling histograms/tables.
      //

      void AddVariable(string tag, string plotLabel, string unit, int nBins, float min, float max, float* autoFillPointer = 0, string options = "");

      int getIndexOfVariable(string tag);
          
      // #############################
      // #  ProcessClass management  #
      // #############################

      //
      // *** Add a process class to the knownledge of this Screwdriver.
      //     
      //     The xsec is the cross-section used for the relevant plots
      //     that needs cross-section reweighting.
      //

      void AddProcessClass(string tag, string plotLabel, string type, Color_t color, string options = "");
     
      //
      // *** Get the list of names of process classes
      //     
      //	The list is returned in the "output" vector.
      //
     
      void   GetProcessClassList(vector<string> *output);

      //
      // *** Get the type of a specific process class
      //
     
      string GetProcessClassType(string processClass);

      // ########################
      // #  Dataset management  #
      // ########################
      
      void AddDataset(string tag, string processClass, int trueNumberOfEvents, float xsecOrLumi = 1.0, string options = "");
      
      void   GetDatasetList(vector<string> *output);
      string GetProcessClass(string dataset);
      float  GetDatasetLumiWeight(string dataset);
      void   SetTrueNumberOfEvents(string dataset, int n);
      
      float GetLumi();
      void SetLumi(float inputLumi);

      // #######################
      // # Channels management #
      // #######################

      //
      // *** Add a channel to the knownledge of this Screwdriver.
      //     
      //	 The selector is a function returning true or false
      //     if the event currently being filled is or isnt
      //	 in this channel
      //

      void  AddChannel(string tag, string plotLabel, bool (*selector)(), string options = "");

      // ######################
      // # Regions management #
      // ######################

      //
      // *** Add a region to the knownledge of this Screwdriver.
      //     
      //	 The selector is a function returning true or false
      //     if the event currently being filled is or isnt
      //	 in this region
      //

      void  AddRegion(string tag, string plotLabel, bool (*selector)(), string options = "");

      // ########################
      // #   Histo management   #
      // ########################
     
      //
      // *** "Automatically" fill every 1D/2D histo associated to the
      //     specified process class, and for which autoFill has been set to
      //     true.
      //
      //     The value used to fill is obtained using the autoFillPointer
      //     of variables.
      // 

      void AutoFillProcessClass(string processClass, float weight = 1.0);

            // ########################
            // #  Histo1D management  #
            // ########################

      // 
      // *** Add a 1-dimension histogram to the HistoScrewdriver
      //     for the specified set of variables/processClass.
      //
      //     (If no variable/processClass is specified, every 
      //     variable/processClass known is used)
      //
      //     When adding a single var-processClass histo, one can override
      //     the default nBins/min/max for the variable.
      //	
      //	 If autoFill is set to true, the histogram will be
      //	 filled during the "automatic" fill.
      //

      void Create1DHistos();

      //
      // *** Manual fill of the var-processClass specified histogram.
      //

      void Fill(string var,	string processClass, float value, float weight = 1.0);
      
      // *** Apply scale factor on an histogram
      
      void ApplyScaleFactor(string var,	string processClass, string region, string channel, Figure scaleFactor);

      void ApplyScaleFactor(string var,	string processClass, Figure scaleFactor);

            // ########################
            // #  Histo2D management  #
            // ########################
     
      // 
      // *** Add a 2-dimension histogram to the HistoScrewdriver
      //     for the specified set of variables/processClass.
      //
      //	 If autoFill is set to true, the histogram will be
      //	 filled during the "automatic" fill.
      // 
      
      void Add2DHisto(string varX, string varY, 
                      bool autoFill = true, 
                      int nBinsX = -1, float minX = -1.0, float maxX = -1.0,
                      int nBinsY = -1, float minY = -1.0, float maxY = -1.0);

      //
      // *** Manual fill of the varX-varY-processClass specified histogram.
      //

      void Fill(string varX, string varY, string processClass, float valueX, float valueY, float weight = 1.0);
 
            // ########################
            // #  Histo3D management  #
            // ########################
     
      // 
      // *** Add a 3-dimension histogram to the HistoScrewdriver
      //     for the specified set of variables/processClass.
      //
      //	 If autoFill is set to true, the histogram will be
      //	 filled during the "automatic" fill.
      // 
      
      void Add3DHisto(string varX, string varY, string varZ, 
                      bool autoFill = true, 
                      int nBinsX = -1, float minX = -1.0, float maxX = -1.0,
                      int nBinsY = -1, float minY = -1.0, float maxY = -1.0,
                      int nBinsZ = -1, float minZ = -1.0, float maxZ = -1.0);

      //
      // *** Manual fill of the varX-varY-varZ-processClass specified histogram.
      //

      void Fill(string varX, string varY, string varZ, string processClass, float valueX, float valueY, float valueZ, float weight = 1.0);
         
      // ########################
      // #   Plots management   #
      // ########################


      // Schedule the plot type specified to be generated when doing MakePlots
      void SchedulePlots(string plotType, string options = "");


      //
      // *** (Temporary ?) Function that create a bunch of histos
      //
      void MakePlots(string options = "");

      //
      // *** Ask the plotScrewdriver to write the plots
      //
      void WritePlots(string outputFolder, string infoText = "", string options = "");

      Figure GetYieldAndError(string var, string processClass,string region,string channel);

      vector<Histo2D>* Get2DHistoList();

     private:

      // *** Container for the variables
      vector<Variable> theVariables;

      // *** Container for the processClasses
      vector<ProcessClass> theProcessClasses;
      
      // *** Container for the datasets
      vector<Dataset> theDatasets;

      // *** Container for the channels
      vector<Channel> theChannels;
      
      // *** Container for the regions
      vector<Region> theRegions;
      



      // *** Manager for the histograms
      HistoScrewdriver theHistoScrewdriver;
      
      // *** Manager for the plots
      PlotScrewdriver thePlotScrewdriver;

      // TODO	  
      //vector<TableScrewdriver> theTableScrewdrivers;

      float theLumi;

    };

}

#endif
