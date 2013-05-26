#ifndef Plot1DCustom_h
#define Plot1DCustom_h

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

    class Plot1DCustom 
    {
      
     public:
     
      Plot1DCustom();
      ~Plot1DCustom();

      static void MakePlot(Variable* theVar, 
                           vector<ProcessClass>* theProcessClasses, 
                           Region* theRegion, 
                           Channel* theChannel, 
                           HistoScrewdriver* theHistoScrewdriver, 
                           Plot* thePlot, 
                           string plotOptions = "",
                           string customOptions = "")
      {

         // Prepare the labels for x and y axis
         // xlabel = labelDeLaVariable (Unité)
         // ylabel = Normalized entries / largeurDeBin Unité

         string xlabel(theVar->getLabel());
         string ylabel("Normalized entries / ");
         
         // Checking custom yLabel
         string custom_yLabel = OptionsScrewdriver::getStringOption(customOptions,"yLabel");
         if (custom_yLabel != "") ylabel = custom_yLabel + " / ";
         
         // Get the bin width and concatenate it with ylabel
         std::ostringstream ss;
         ss.precision(3);
         ss << theHistoScrewdriver->get1DHistoClone(theVar->getTag(),(*theProcessClasses)[0].getTag(),theRegion->getTag(),theChannel->getTag())->GetBinWidth(1);
         ylabel += ss.str();

         // Add the unit
         if (theVar->getUnit() != "")
         {
            xlabel += " [";
            xlabel += theVar->getUnit() + "]";
            ylabel += " ";
            ylabel += theVar->getUnit();
         }

        float globalMax = 0.0;
        TH1F* firstHisto = NULL;

        string normMode = OptionsScrewdriver::getStringOption(customOptions,"normMode");
         
        // Now loop on the histos
        for (unsigned int i = 0 ; i < theProcessClasses->size() ; i++)
        {
            // If this processClass is not a background, we skip it
            if ((*theProcessClasses)[i].getType() != "background") continue;

            string processClassOptions = (*theProcessClasses)[i].getOptions();
                
            TH1F* histoClone = theHistoScrewdriver->get1DHistoClone(theVar->getTag(),(*theProcessClasses)[i].getTag(),theRegion->getTag(),theChannel->getTag());
            ApplyHistoStyle(thePlot,histoClone,(*theProcessClasses)[i].getColor(),plotOptions,(*theProcessClasses)[i].getOptions());

            // Add to legend
            thePlot->AddToLegend(histoClone,(*theProcessClasses)[i].getLabelC() ,"l");

            // Normalize histogram to unity
            
            if (normMode == "binByBin")
            {
                TH1F* histoEntriesClone = theHistoScrewdriver->get1DHistoEntriesClone(theVar->getTag(),
                                                                                      (*theProcessClasses)[i].getTag(),
                                                                                      theRegion->getTag(),
                                                                                      theChannel->getTag() );
                histoClone->Divide(histoEntriesClone);
            }
            else
            {
                histoClone->Scale(1.0/histoClone->Integral());
            }

            // Get the max value after normalization
            if (globalMax < histoClone->GetMaximum())
                globalMax = histoClone->GetMaximum();
            
            if (firstHisto == NULL) 
            {
                ApplyAxisStyle(thePlot,histoClone,xlabel,ylabel,plotOptions,theVar->getOptions());
                firstHisto = histoClone;
                histoClone->Draw("hist E0");
            }
            else histoClone->Draw("hist E0 same");

        }

        // Set max value for the plot
        firstHisto->SetMaximum(globalMax * 1.1);

      }

     private:

      static void ApplyHistoStyle(Plot* thePlot, TH1F* theHisto, Color_t color, string plotOptions = "", string processClassOptions = "")
      {
         theHisto->SetFillColor(0);
         theHisto->SetLineWidth(6);
         theHisto->SetLineColor(color);
         
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
