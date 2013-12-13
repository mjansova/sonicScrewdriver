#ifndef HistoScrewdriver_h
#define HistoScrewdriver_h

#include "interface/Common.h"
#include "interface/Variable.h"
#include "interface/ProcessClass.h"
#include "interface/Dataset.h"
#include "interface/histos/Histo1DEntries.h"
#include "interface/histos/Histo2DEntries.h"
#include "interface/histos/Histo3DEntries.h"

namespace theDoctor
{

    class HistoScrewdriver 
    {
      
     public:
     
      HistoScrewdriver(vector<Variable>*, vector<ProcessClass>*, vector<Region>*, vector<Channel>*);
      ~HistoScrewdriver();


      void AutoFillProcessClass(string processClass, float weight = 1.0);
      
      int getIndexOfVariable(string tag);
      int getIndexOfProcessClass(string tag);
      int getIndexOfRegion(string tag);
      int getIndexOfChannel(string tag);
      int getIndexOfHisto1DEntries(string tagVar,  string tagProcessClass, string tagRegion, string tagChannel);
      int getIndexOfHisto2DEntries(string tagVarX, string tagVarY, string tagProcessClass, string tagRegion, string tagChannel);
      int getIndexOfHisto3DEntries(string tagVarX, string tagVarY, string tagVarZ, string tagProcessClass, string tagRegion, string tagChannel);
      
      int getIndexOfHisto1DForPlot(string type, string tagVar, string tagRegion, string tagChannel, string otherParameters = "");


      // 1D histos
     
      void Create1DHistosEntries();

      void Fill(string var, string processClass, float value, float weight = 1.0);
      void Fill(string var,	string processClass, string region, string channel, float value, float weight = 1.0);
      void AutoFill1DProcessClass(string processClass, float weight = 1.0);

      void ApplyScaleFactor(string var, string processClass, string region, string channel, Figure scaleFactor);

      Histo1DEntries* get1DHistoEntriesPointer(string var, string processClass, string region, string channel);
      Histo1D* get1DHistoForPlotPointer(string type, string var, string region, string channel, string options);
      TH1F* get1DHistoClone(string var, string processClass, string region, string channel);
      TH1F* get1DHistoEntriesClone(string var, string processClass, string region, string channel);
      vector<Histo1DEntries>* Get1DHistosEntries();

      // 2D histos
     
      void Add2DHistoEntries(string varX, string varY); 
      void Fill(string varX, string varY, string processClass, float valueX, float valueY, float weight = 1.0);
      void AutoFill2DProcessClass(string processClass, float weight = 1.0);

      Histo2DEntries* get2DHistoPointer(string varX, string varY, string processClass, string region, string channel);
      TH2F* get2DHistoClone(string varX, string varY, string processClass, string region, string channel);
      TH2F* get2DHistoEntriesClone(string varX, string varY, string processClass, string region, string channel);
      vector<Histo2DEntries>* Get2DHistosEntries();
      
      // 3D histos
      void Add3DHistoEntries(string varX, string varY, string varZ); 
      void Fill(string varX, string varY, string varZ, string processClass, float valueX, float valueY, float valueZ, float weight = 1.0);
      void AutoFill3DProcessClass(string processClass, float weight = 1.0);
      
      Histo3DEntries* get3DHistoPointer(string varX, string varY, string varZ, string processClass, string region, string channel);
      TH3F* get3DHistoClone(string varX, string varY, string varZ, string processClass, string region, string channel);
      TH3F* get3DHistoEntriesClone(string varX, string varY, string varZ, string processClass, string region, string channel);
      vector<Histo3DEntries>* Get3DHistosEntries();

      // TODO - rename and make something more customazible/clean
      Figure GetYieldAndError(string var, string processClass, string region, string channel);

      void Add1DHistoForPlots(Histo1D theHisto) { the1DHistosForPlots.push_back(theHisto); }
      void Add2DHistoForPlots(Histo2D theHisto) { the2DHistosForPlots.push_back(theHisto); }
      void Add3DHistoForPlots(Histo3D theHisto) { the3DHistosForPlots.push_back(theHisto); }

      vector<Histo1D>* Get1DHistosForPlots() { return &the1DHistosForPlots; } 
      vector<Histo2D>* Get2DHistosForPlots() { return &the2DHistosForPlots; }
      vector<Histo3D>* Get3DHistosForPlots() { return &the3DHistosForPlots; }

     private:

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
