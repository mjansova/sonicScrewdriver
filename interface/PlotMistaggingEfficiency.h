#ifndef PlotMistaggingEfficiency_h
#define PlotMistaggingEfficiency_h

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

    class PlotMistaggingEfficiency 
    {
      
     public:
     
      PlotMistaggingEfficiency();
      ~PlotMistaggingEfficiency();

      static void MakePlot(Variable* theVar, 
                           ProcessClass* theUnmatchedCandObjectsProcessClass, 
                           ProcessClass* theUnmatchedRecoObjectsProcessClass, 
                           Region* theRegion, 
                           Channel* theChannel, 
                           HistoScrewdriver* theHistoScrewdriver, 
                           Plot* thePlot, 
                           string plotOptions = "")
      {

         // Prepare the labels for x and y axis
         // xlabel = labelDeLaVariable (Unité)
         // ylabel = Normalized entries / largeurDeBin Unité

         string xlabel(theVar->getLabel());
         string ylabel("Mistagging efficiency");
         
         // Add the unit
         if (theVar->getUnit() != "")
         {
            xlabel += " [";
            xlabel += theVar->getUnit() + "]";
         }

        TH1F* genObjectsHisto = theHistoScrewdriver->get1DHistoClone(theVar->getTag(),
                                                                     theUnmatchedCandObjectsProcessClass->getTag(),
                                                                     theRegion->getTag(),
                                                                     theChannel->getTag());
        
        /*TH1F* matchedRecoObjectsHisto = theHistoScrewdriver->get1DHistoClone(theVar->getTag(),
                                                                     theUnmatchedRecoObjectsProcessClass->getTag(),
                                                                     theRegion->getTag(),
                                                                     theChannel->getTag());*/

        // Compute tagging efficiency
        TH1F* mistaggingEfficiency = theHistoScrewdriver->get1DHistoClone(theVar->getTag(),
                                                                     theUnmatchedRecoObjectsProcessClass->getTag(),
                                                                     theRegion->getTag(),
                                                                     theChannel->getTag());
        mistaggingEfficiency->Divide(genObjectsHisto);

        // Apply style and draw

        ApplyHistoStyle(thePlot,mistaggingEfficiency,plotOptions);
        ApplyAxisStyle(thePlot,mistaggingEfficiency,xlabel,ylabel,plotOptions,theVar->getOptions());
        mistaggingEfficiency->Draw("E0");
        mistaggingEfficiency->SetMaximum(mistaggingEfficiency->GetMaximum() * 1.1);

      }

     private:

      static void ApplyHistoStyle(Plot* thePlot, TH1F* theHisto, string plotOptions = "")
      {
         theHisto->SetFillColor(0);
         theHisto->SetLineWidth(4);
         theHisto->SetLineColor(COLORPLOT_AZURE);
         
      }

      static void ApplyAxisStyle(Plot* thePlot, TH1F* theHisto, string xlabel, string ylabel, string plotOptions = "", string varOptions = "")
      {	
         PlotDefaultStyles::ApplyDefaultAxisStyle(theHisto->GetXaxis(),xlabel);
         PlotDefaultStyles::ApplyDefaultAxisStyle(theHisto->GetYaxis(),ylabel);
         theHisto->SetTitle("");
         theHisto->SetStats(0);

         if (OptionsScrewdriver::getBoolOption(varOptions,"logY")) thePlot->SetLogY();
      }

    };

}
#endif
