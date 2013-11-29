#ifndef Plot2D_h
#define Plot2D_h

#include "interface/Common.h"

#include "interface/Plot.h"
#include "interface/Variable.h"
#include "interface/ProcessClass.h"
#include "interface/Histo1D.h"
#include "interface/Histo2D.h"
#include "interface/PlotDefaultStyles.h"
#include "interface/OptionsScrewdriver.h"

namespace theDoctor
{

    class Plot2D
    {
      
     public:
     
      Plot2D();
      ~Plot2D();


      static void MakePlot(Variable* theVarX, 
                           Variable* theVarY, 
                           ProcessClass* theProcessClass, 
                           Region* theRegion, 
                           Channel* theChannel, 
                           HistoScrewdriver* theHistoScrewdriver, 
                           Plot* thePlot, 
                           string plotOptions = "")
      {

         // Prepare the labels for x and y axis
         // xlabel = labelDeLaVariable (Unité)
         // ylabel = Normalized entries / largeurDeBin Unité

         string xlabel(theVarX->getLabel());
         string ylabel(theVarY->getLabel());
         
         // Add the units
         if (theVarX->getUnit() != "")
            xlabel += " [" + theVarX->getUnit() + "]";
         if (theVarY->getUnit() != "")
            ylabel += " [" + theVarY->getUnit() + "]";

         // Get the options
         string processClassOptions = theProcessClass->getOptions();
               
         // Get clone of the histo
         TH2F* histoClone = theHistoScrewdriver->get2DHistoClone(theVarX->getTag(),
                                                                 theVarY->getTag(),
                                                                 theProcessClass->getTag(),
                                                                 theRegion->getTag(),
                                                                 theChannel->getTag());

         ApplyHistoStyle(thePlot,histoClone,theProcessClass->getColor(),plotOptions,theProcessClass->getOptions());

         // Normalize histogram to unity
         //histoClone->Scale(1.0/histoClone->Integral());
            
         ApplyAxisStyle(thePlot,histoClone,xlabel,ylabel,theProcessClass->getTag(),plotOptions,theVarX->getOptions(),theVarY->getOptions());
         histoClone->Draw("COLZ");

      }

      static void GetHistoDependencies(vector<pair<string,string> >& output)
      {
      }

      static void ApplyHistoStyle(Plot* thePlot, TH2F* theHisto, Color_t color, string plotOptions = "", string processClassOptions = "")
      {
      }

      static void ApplyAxisStyle(Plot* thePlot, TH2F* theHisto, string xlabel, string ylabel, string title, string plotOptions = "", string varXOptions = "", string varYOptions = "")
      {	
         PlotDefaultStyles::ApplyDefaultAxisStyle(theHisto->GetXaxis(),xlabel);
         PlotDefaultStyles::ApplyDefaultAxisStyle(theHisto->GetYaxis(),ylabel);
         //theHisto->SetTitle(title.c_str());
         theHisto->SetTitle("");
         theHisto->SetStats(0);
         thePlot->SetLogZ();
      }

    };

}

#endif
