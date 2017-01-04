#ifndef HistoScrewdriver_h
#define HistoScrewdriver_h

#include "interface/Common.h"
#include "interface/Variable.h"
#include "interface/ProcessClass.h"
#include "interface/Dataset.h"
#include "interface/histos/Histo1DEntries.h"
#include "interface/histos/Histo2DEntries.h"
#include "interface/histos/Histo3DEntries.h"

static vector<float> DEFAULT_VECTOR;

namespace theDoctor
{

    class HistoScrewdriver
    {

     public:

      HistoScrewdriver(vector<Variable>*, vector<ProcessClass>*, vector<Region>*, vector<Channel>*);
      ~HistoScrewdriver();

      void Create1DHistosEntries();
      void AutoFillProcessClass(vector< vector<Figure> >* yieldsMtx, string processClass, float weight, vector<float> weightV, bool syst, bool checkYields);
      Figure GetYieldAndError(string processClass, string region, string channel);

      vector<Histo1DEntries>* Get1DHistosEntries();
      vector<Histo2DEntries>* Get2DHistosEntries();
      vector<Histo3DEntries>* Get3DHistosEntries();

      vector<Histo1D>* Get1DHistosForPlots() { return &the1DHistosForPlots; }
      vector<Histo2D>* Get2DHistosForPlots() { return &the2DHistosForPlots; }
      vector<Histo3D>* Get3DHistosForPlots() { return &the3DHistosForPlots; }

      void AutoFill1DProcessClass(vector< vector<Figure> >* yieldsMtx, string processClass, float weight = 1.0, vector<float> weightV = DEFAULT_VECTOR, bool syst = false, bool checkYields = false);

      void ApplyScaleFactor(string var, string processClass, string region, string channel, Figure scaleFactor);

      Histo1DEntries* get1DHistoEntriesPointer(string var, string processClass, string region, string channel);
      Histo1D* get1DHistoForPlotPointer(string type, string var, string region, string channel, string options);
      TH1D* get1DHistoClone(string var, string processClass, string region, string channel);
      TH1D* get1DHistoEntriesClone(string var, string processClass, string region, string channel);
      TH1D* get1DCompositeHistoClone(string var, string type, string region, string channel, string otherParameters);
      vector<TH1D*> get1DHistoCloneFromFile(string dir, string type, string var, vector<string> processClass, string region, string channel);
      vector<TH1D*> get1DHistoCanFileFromFile(TFile** file, TCanvas** can, string dir, string type, string var, vector<string> processClass, string region, string channel);

      // 2D histos

      void Add2DHistoEntries(string varX, string varY);
      void AutoFill2DProcessClass(string processClass, float weight = 1.0);

      Histo2DEntries* get2DHistoEntriesPointer(string varX, string varY, string processClass, string region, string channel);
      Histo2D* get2DHistoForPlotPointer(string type, string varX, string varY, string region, string channel, string options);
      TH2D* get2DHistoClone(string varX, string varY, string processClass, string region, string channel);
      TH2D* get2DHistoEntriesClone(string varX, string varY, string processClass, string region, string channel);
      TH2D* get2DCompositeHistoClone(string varX, string varY, string type, string region, string channel, string otherParameters);

      // 3D histos
      void Add3DHistoEntries(string varX, string varY, string varZ);
      void AutoFill3DProcessClass(string processClass, float weight = 1.0);

      Histo3DEntries* get3DHistoEntriesPointer(string varX, string varY, string varZ, string processClass, string region, string channel);
      Histo3D* get3DHistoForPlotPointer(string type, string varX, string varY, string varZ, string region, string channel, string options);
      TH3D* get3DHistoClone(string varX, string varY, string varZ, string processClass, string region, string channel);
      TH3D* get3DHistoEntriesClone(string varX, string varY, string varZ, string processClass, string region, string channel);

      void Add1DHistoForPlots(Histo1D theHisto) { the1DHistosForPlots.push_back(theHisto); }
      void Add2DHistoForPlots(Histo2D theHisto) { the2DHistosForPlots.push_back(theHisto); }
      void Add3DHistoForPlots(Histo3D theHisto) { the3DHistosForPlots.push_back(theHisto); }

    private:

      void UpdateRegionsAndChannels();

      int getIndexOfVariable(string tag);
      int getIndexOfProcessClass(string tag);
      int getIndexOfRegion(string tag);
      int getIndexOfChannel(string tag);
      int getIndexOfHisto1DEntries(string tagVar,  string tagProcessClass, string tagRegion, string tagChannel);
      int getIndexOfHisto2DEntries(string tagVarX, string tagVarY, string tagProcessClass, string tagRegion, string tagChannel);
      int getIndexOfHisto3DEntries(string tagVarX, string tagVarY, string tagVarZ, string tagProcessClass, string tagRegion, string tagChannel);

      int getIndexOfHisto1DForPlot(string type, string tagVar, string tagRegion, string tagChannel, string otherParameters = "");
      int getIndexOfHisto2DForPlot(string type, string tagVarX, string tagVarY, string tagRegion, string tagChannel, string otherParameters = "");
      int getIndexOfHisto3DForPlot(string type, string tagVarX, string tagVarY, string tagVarZ, string tagRegion, string tagChannel, string otherParameters = "");

      vector<Variable>*      theVariables;
      vector<ProcessClass>*  theProcessClasses;
      vector<Region>*        theRegions;
      vector<Channel>*       theChannels;

      vector<Histo1DEntries> the1DHistosEntries;
      vector<Histo2DEntries> the2DHistosEntries;
      vector<Histo3DEntries> the3DHistosEntries;

      vector<Histo1D> the1DHistosForPlots;
      vector<Histo2D> the2DHistosForPlots;
      vector<Histo3D> the3DHistosForPlots;


    };

}

#endif
