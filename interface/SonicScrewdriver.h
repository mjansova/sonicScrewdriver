#ifndef SonicScrewdriver_h
#define SonicScrewdriver_h

#include "interface/Common.h"

#include "interface/OptionsScrewdriver.h"
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

      void Create1DHistos();

      // *** Apply scale factor on an histogram
      
      void ApplyScaleFactor(string var,	string processClass, string region, string channel, Figure scaleFactor);

      void ApplyScaleFactor(string var,	string processClass, Figure scaleFactor);

            // ########################
            // #  Histo2DEntries management  #
            // ########################
     
      // 
      // *** Add a 2-dimension histogram to the HistoScrewdriver
      //     for the specified set of variables/processClass.
      //
      // 
      
      void Add2DHisto(string varX, string varY);

            // ########################
            // #  Histo3DEntries management  #
            // ########################
     
      // 
      // *** Add a 3-dimension histogram to the HistoScrewdriver
      //     for the specified set of variables/processClass.
      //
      //	 If autoFill is set to true, the histogram will be
      //	 filled during the "automatic" fill.
      // 
      
      void Add3DHisto(string varX, string varY, string varZ);

      // ##########################
      // #   Options management   #
      // ##########################

      void SetGlobalFloatOption(string category, string field, float value)   { thePlotScrewdriver.SetGlobalFloatOption (category,field,value); }
      void SetGlobalStringOption(string category, string field, string value) { thePlotScrewdriver.SetGlobalStringOption(category,field,value); }
      void SetGlobalBoolOption(string category, string field, bool value)     { thePlotScrewdriver.SetGlobalBoolOption  (category,field,value); }
      void SetGlobalIntOption(string category, string field, int value)       { thePlotScrewdriver.SetGlobalIntOption   (category,field,value); }

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
      void WritePlots(string outputFolder);

      Figure GetYieldAndError(string var, string processClass,string region,string channel);

      vector<Histo1DEntries>* Get1DHistosEntries();
      vector<Histo2DEntries>* Get2DHistosEntries();
      vector<Histo3DEntries>* Get3DHistosEntries();

      TH1F* get1DHistoClone(string var, string processClass, string region, string channel);
      TH1F* get1DCompositeHistoClone(string var, string type, string region, string channel, string otherParameters);
      TH2F* get2DHistoClone(string varX, string varY, string processClass, string region, string channel);
      TH2F* get2DCompositeHistoClone(string varX, string varY, string type, string region, string channel, string otherParameters);

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

      float theLumi;

    };

}

#endif

