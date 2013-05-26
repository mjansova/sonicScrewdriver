#ifndef HistoScrewdriver_h
#define HistoScrewdriver_h

#include "interface/Common.h"
#include "interface/Variable.h"
#include "interface/ProcessClass.h"
#include "interface/Dataset.h"
#include "interface/Histo1D.h"
#include "interface/Histo2D.h"
#include "interface/Histo3D.h"

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
      int getIndexOfHisto1D(string tagVar,  string tagProcessClass, string tagRegion, string tagChannel);
      int getIndexOfHisto2D(string tagVarX, string tagVarY, string tagProcessClass, string tagRegion, string tagChannel);
      int getIndexOfHisto3D(string tagVarX, string tagVarY, string tagVarZ, string tagProcessClass, string tagRegion, string tagChannel);


      // 1D histos
     
      void Create1DHistos();

      void Fill(string var, string processClass, float value, float weight = 1.0);
      void Fill(string var,	string processClass, string region, string channel, float value, float weight = 1.0);
      void AutoFill1DProcessClass(string processClass, float weight = 1.0);

      void ApplyScaleFactor(string var, string processClass, string region, string channel, Figure scaleFactor);

      Histo1D* get1DHistoPointer(string var, string processClass, string region, string channel);
      TH1F* get1DHistoClone(string var, string processClass, string region, string channel);
      TH1F* get1DHistoEntriesClone(string var, string processClass, string region, string channel);

      // 2D histos
     
      void Add2DHisto(string varX, string varY, bool autoFill = true, 
              int nBinsX = -1, float minX = -1.0, float maxX = -1.0, 
              int nBinsY = -1, float minY = -1.0, float maxY = -1.0);
      void Fill(string varX, string varY, string processClass, float valueX, float valueY, float weight = 1.0);
      void AutoFill2DProcessClass(string processClass, float weight = 1.0);

      Histo2D* get2DHistoPointer(string varX, string varY, string processClass, string region, string channel);
      TH2F* get2DHistoClone(string varX, string varY, string processClass, string region, string channel);
      TH2F* get2DHistoEntriesClone(string varX, string varY, string processClass, string region, string channel);
      vector<Histo2D>* Get2DHistoList();
      
      // 3D histos
      void Add3DHisto(string varX, string varY, string varZ, bool autoFill = true, 
              int nBinsX = -1, float minX = -1.0, float maxX = -1.0, 
              int nBinsY = -1, float minY = -1.0, float maxY = -1.0,
              int nBinsZ = -1, float minZ = -1.0, float maxZ = -1.0);
      void Fill(string varX, string varY, string varZ, string processClass, float valueX, float valueY, float valueZ, float weight = 1.0);
      void AutoFill3DProcessClass(string processClass, float weight = 1.0);
      
      Histo3D* get3DHistoPointer(string varX, string varY, string varZ, string processClass, string region, string channel);
      TH3F* get3DHistoClone(string varX, string varY, string varZ, string processClass, string region, string channel);
      TH3F* get3DHistoEntriesClone(string varX, string varY, string varZ, string processClass, string region, string channel);
      vector<Histo3D>* Get3DHistoList();


      // TODO - rename and make something more customazible/clean
      Figure GetYieldAndError(string var, string processClass, string region, string channel);

     private:

      vector<Variable>*      theVariables;
      vector<ProcessClass>*  theProcessClasses;
      vector<Region>*        theRegions;
      vector<Channel>*       theChannels;
      
      vector<Histo1D> the1DHistos;
      vector<Histo2D> the2DHistos;
      vector<Histo3D> the3DHistos;

    };

}

#endif
