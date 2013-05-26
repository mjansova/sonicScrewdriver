#ifndef PlotTaggingEfficiency_h
#define PlotTaggingEfficiency_h

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

    class PlotTaggingEfficiency 
    {
      
     public:
     
      PlotTaggingEfficiency();
      ~PlotTaggingEfficiency();

      static void MakePlot(Variable* theVar, 
                           ProcessClass* theGenObjectsProcessClass, 
                           ProcessClass* theMatchedRecoObjectsProcessClass, 
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
         string ylabel("Tagging efficiency");
         
         // Add the unit
         if (theVar->getUnit() != "")
         {
            xlabel += " [";
            xlabel += theVar->getUnit() + "]";
         }

        TH1F* genObjectsHisto = theHistoScrewdriver->get1DHistoClone(theVar->getTag(),
                                                                     theGenObjectsProcessClass->getTag(),
                                                                     theRegion->getTag(),
                                                                     theChannel->getTag());
        
        /*TH1F* matchedRecoObjectsHisto = theHistoScrewdriver->get1DHistoClone(theVar->getTag(),
                                                                     theMatchedRecoObjectsProcessClass.getTag(),
                                                                     theRegion->getTag(),
                                                                     theChannel->getTag());*/

        // Compute tagging efficiency
        TH1F* taggingEfficiency = theHistoScrewdriver->get1DHistoClone(theVar->getTag(),
                                                                     theMatchedRecoObjectsProcessClass->getTag(),
                                                                     theRegion->getTag(),
                                                                     theChannel->getTag());
        taggingEfficiency->Divide(genObjectsHisto);

        // Apply style and draw

        ApplyHistoStyle(thePlot,taggingEfficiency,plotOptions);
        ApplyAxisStyle(thePlot,taggingEfficiency,xlabel,ylabel,plotOptions,theVar->getOptions());
        taggingEfficiency->Draw("E0");
        taggingEfficiency->SetMaximum(taggingEfficiency->GetMaximum() * 1.1);

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
