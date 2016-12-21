#ifndef SonicScrewdriver_h
#define SonicScrewdriver_h

#include "interface/Common.h"

#include "interface/OptionsScrewdriver.h"
#include "interface/HistoScrewdriver.h"
#include "interface/PlotScrewdriver.h"

#include "interface/Region.h"
#include "interface/Channel.h"

#include "TXMLEngine.h"

namespace theDoctor
{


    class SonicScrewdriver
    {

     public:

      SonicScrewdriver(bool add_default_yield = true);
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

      void AddVariable(string tag, string plotLabel, string unit, int nBins, float min, float max, float*  autoFillPointer = 0, string options = "");
      void AddVariable(string tag, string plotLabel, string unit, int nBins, float min, float max, double* autoFillPointer = 0, string options = "");
      void AddVariable(string tag, string plotLabel, string unit, int nBins, int   min, int   max, int*    autoFillPointer = 0, string options = "");
      void AddVariable(string tag, string plotLabel, string unit, int nBins, float min, float max, float*   (*autoFillFunction)() = 0, string options = "");
      void AddVariable(string tag, string plotLabel, string unit, int nBins, int   min, int   max, int*     (*autoFillFunction)() = 0, string options = "");

      void AddVariable(string tag, string plotLabel, string unit, int nBins,    float* binning,    float*  autoFillPointer = 0, string options = "");
      void AddVariable(string tag, string plotLabel, string unit, int nBins,    float* binning,    double* autoFillPointer = 0, string options = "");
      void AddVariable(string tag, string plotLabel, string unit, int nBins,    float* binning,    int*    autoFillPointer = 0, string options = "");
      void AddVariable(string tag, string plotLabel, string unit, int nBins,    float* binning,    float*   (*autoFillFunction)() = 0, string options = "");
      void AddVariable(string tag, string plotLabel, string unit, int nBins,    float* binning,    int*     (*autoFillFunction)() = 0, string options = "");

      void GetVariablesTagList(vector<string> *output);
      void GetVariablesLabelList(vector<string> *output);
      int GetIndexOfVariable(string tag);
      void SetSizeOfVarArray(string tag, uint32_t arrSize);
      void fillYieldsVector();
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
      //    The list is returned in the "output" vector.
      //

      void   GetProcessClassTagList(vector<string> *output);
      void   GetProcessClassLabelList(vector<string> *output);

      //
      // *** Get the type of a specific process class
      //

      string GetProcessClassType(string processClass);

      // ########################
      // #  Dataset management  #
      // ########################

      void AddDataset(string tag, string processClass, int trueNumberOfEvents = -1, float xsecOrLumi = -1, string options = "");

      void     GetDatasetList(vector<string> *output);
      string   GetProcessClass(string dataset);
      float    GetDatasetLumiWeight(string dataset);
      float    GetDatasetCrossSection(string dataset);

      void     SetTrueNumberOfEvents(string dataset, int n);

      float GetLumi();
      void SetLumi(float inputLumi);

      // #######################
      // # Channels management #
      // #######################

      //
      // *** Add a channel to the knownledge of this Screwdriver.
      //
      //     The selector is a function returning true or false
      //     if the event currently being filled is or isnt
      //     in this channel
      //

      void  AddChannel(string tag, string plotLabel, bool (*selector)(), string options = "");

      // ######################
      // # Regions management #
      // ######################

      //
      // *** Add a region to the knownledge of this Screwdriver.
      //
      //     The selector is a function returning true or false
      //     if the event currently being filled is or isnt
      //     in this region
      //

      void  AddRegion(string tag, string plotLabel,                         bool (*selector)(),     string options = "");
      void  AddRegion(string tag, string plotLabel, string parentRegionTag, bool (*selector)(),     string options = "");
      void  AddRegion(string tag, string plotLabel,                         vector<Cut> setOfCuts,  string options = "");
      void  AddRegion(string tag, string plotLabel, string parentRegionTag, vector<Cut> setOfCuts,  string options = "");

      //
      // *** Get the list of names of regions
      //
      //    The list is returned in the "output" vector.
      //

      void   GetRegionTagList(vector<string> *output);
      void   GetRegionLabelList(vector<string> *output);

      // #########################
      // #   Figure management   #
      // #########################

      void AddFigurePerProcess(string tag, string label, string options = "");
      void AddFigure          (string tag, string label, string options = "");

      void SetFigure(string tag, string process, string region, string channel, Figure figureValue);
      void SetFigure(string tag, string region, string channel, Figure figureValue);

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

      void AutoFillProcessClass(string processClass, float weight = 1, bool zeroNegative = false, vector<float> weightV = DEFAULT_VECTOR, bool syst = false);

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

      void ApplyScaleFactor(string var,    string processClass, string region, string channel, Figure scaleFactor);

      void ApplyScaleFactor(string processClass, string region, string channel, Figure scaleFactor);

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
      //     If autoFill is set to true, the histogram will be
      //     filled during the "automatic" fill.
      //

      void Add3DHisto(string varX, string varY, string varZ);

      // ##########################
      // #   Options management   #
      // ##########################

      void SetGlobalFloatOption(string category, string field, float value)   { thePlotScrewdriver.SetGlobalFloatOption (category,field,value); }
      void SetGlobalStringOption(string category, string field, string value) { thePlotScrewdriver.SetGlobalStringOption(category,field,value); }
      void SetGlobalBoolOption(string category, string field, bool value)     { thePlotScrewdriver.SetGlobalBoolOption  (category,field,value); }

      void LoadJsonConfig(string fileName)                                    { thePlotScrewdriver.LoadJsonConfig(fileName);                    }

      OptionsScrewdriver GetGlobalOptions() { return thePlotScrewdriver.GetGlobalOptions(); }

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

      Figure GetYieldAndError(string processClass,string region,string channel);

      vector<Histo1DEntries>* Get1DHistosEntries();
      vector<Histo2DEntries>* Get2DHistosEntries();
      vector<Histo3DEntries>* Get3DHistosEntries();
      void ExportHistosEntries(string outputFile);
      void ImportHistosEntries(string inputFile); //Function used by babyScrewDriver
      void ImportHistosFromFile(string inputFile); // Function used to read output file using the same directory structure

      TH1D* Get1DHistoClone(string var, string processClass, string region, string channel);
      TH1D* Get1DCompositeHistoClone(string var, string type, string region, string channel, string otherParameters);
      TH2D* Get2DHistoClone(string varX, string varY, string processClass, string region, string channel);
      TH2D* Get2DCompositeHistoClone(string varX, string varY, string type, string region, string channel, string otherParameters);
      vector<TH1D*> Get1DHistoCloneFromFile(string dir, string type, string var, vector<string> processClass, string region, string channel);
      vector<TH1D*> Get1DHistoCanFileFromFile(TFile** file,TCanvas** can,string dir, string type, string var, vector<string> processClass, string region, string channel);


       //
       // *** Write XML file to dump the configuration
       //
       void WriteXMLConfig(string outputfilename = "config.xml");
       void LoadXMLConfig(string inputfilename = "config.xml");

     private:

      Dataset* GetDataset(string tag);

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

      // FIXME We need a special class to handle these guys.

          // *** Container for the figures process-dependent
          vector< pair<Name, Map3DFigure> > theFiguresPerProcessMap;

          // *** Container for the figures
          vector< pair<Name, Map2DFigure> > theFiguresMap;

      // *** Manager for the histograms
      HistoScrewdriver theHistoScrewdriver;

      // *** Manager for the plots
      PlotScrewdriver thePlotScrewdriver;

      float theLumi;

    };

}

#endif

