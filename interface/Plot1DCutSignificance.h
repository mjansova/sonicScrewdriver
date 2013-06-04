#ifndef Plot1DCutSignificance_h
#define Plot1DCutSignificance_h

#include <math.h>

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

    class Plot1DCutSignificance 
    {
      
     public:
     
      Plot1DCutSignificance();
      ~Plot1DCutSignificance();

      static void MakePlot(Variable* theVar, 
                           vector<ProcessClass>* theProcessClasses, 
                           Region* theRegion,
                           Channel* theChannel,
                           HistoScrewdriver* theHistoScrewdriver, 
                           Plot* thePlot,
                           string plotTypeOptions = "",
                           string plotOptions = "")
      {

         // Prepare the labels for x and y axis
         // xlabel = labelDeLaVariable (Unité)
         // ylabel = Normalized entries / largeurDeBin Unité

         string xlabel("Cut on "+theVar->getLabel());
         string ylabel("Significance #epsilon_{S}/#sqrt{#epsilon{B}}");

         // Add the unit
         if (theVar->getUnit() != "")
            xlabel += string(" [") + theVar->getUnit() + string("]");

        // #################################
        // #####                       #####
        // ###       SM / Background     ###
        // #####                       #####
        // #################################

        // Create stack histo + another histo for the error plotting for the background sum
        TH1F* sumBackground = 0;

        // Now loop on the histos
        for (int i = theProcessClasses->size()-1 ; i >= 0 ; i--)
        {

            // If this processClass is not a background, we skip it
            if ((*theProcessClasses)[i].getType() != "background") continue;

            // Get the options for the processClass
            string processClassOptions = (*theProcessClasses)[i].getOptions();
            
            // Get the histo
            TH1F* histoClone = theHistoScrewdriver->get1DHistoClone(theVar->getTag(),(*theProcessClasses)[i].getTag(),theRegion->getTag(),theChannel->getTag());

            // Add it to the sumBackground (error will be extracted from the sum of bkg)
            if (sumBackground == 0)   sumBackground =  (TH1F*) histoClone->Clone();
            else                      sumBackground->Add(histoClone);
        }

        // #################################
        // #####                       #####
        // ###          Signal           ###
        // #####                       #####
        // #################################
        
        TH1F* signalHisto = 0;
        for (int i = theProcessClasses->size()-1 ; i >= 0 ; i--)
        {
            // If this processClass is not data, we skip it
            if ((*theProcessClasses)[i].getType() != "signal") continue;

            TH1F* histoClone = theHistoScrewdriver->get1DHistoClone(theVar->getTag(),(*theProcessClasses)[i].getTag(),theRegion->getTag(),theChannel->getTag());

            if (signalHisto == 0) signalHisto = (TH1F*) histoClone->Clone();
            else signalHisto->Add(histoClone);

        }

        // #################################
        // #####                       #####
        // ###   Ratio effS/sqrt(effB)   ###
        // #####                       #####
        // #################################   
       
        string cutType_ = OptionsScrewdriver::getStringOption(plotTypeOptions,"cutType");
        int cutType = -1;
             if (cutType_ == string("keepLowValues"))  cutType = 0; 
        else if (cutType_ == string("keepHighValues")) cutType = 1; 

        TH1F* significanceHisto = (TH1F*) signalHisto->Clone();
        int nBins = significanceHisto->GetNbinsX();
        float totalYieldS = signalHisto->Integral(0,nBins+1);
        float totalYieldB = sumBackground->Integral(0,nBins+1);
        float effS = 0.0;
        float effB = 0.0;
        for (int i = 1 ; i <= nBins ; i++)
        {

            if (cutType == 1)
            {
                effS = signalHisto->Integral(i,nBins+1) / totalYieldS;
                effB = sumBackground->Integral(i,nBins+1) / totalYieldB;
            }
            else if (cutType == 0)
            {
                effS = signalHisto->Integral(0,i) / totalYieldS;
                effB = sumBackground->Integral(0,i) / totalYieldB;
            }
           
            float sOverSqrtB = 0.0;
            if (effB != 0) sOverSqrtB = effS / sqrt(effB);

            significanceHisto->SetBinContent(i,sOverSqrtB);
            significanceHisto->SetBinError(i,0.0);
        }
       
        // Compute and draw the ratio
        ApplyAxisStyle(thePlot,significanceHisto,xlabel,ylabel,plotOptions);
        ApplyHistoStyle(thePlot,significanceHisto,COLORPLOT_AZURE,plotOptions);
        significanceHisto->Draw("hist E0");

      }

     private:

      static void ApplyHistoStyle(Plot* thePlot, TH1F* theHisto, Color_t color, string plotOptions = "")
      {
         theHisto->SetFillColor(0);
         theHisto->SetLineWidth(6);
         theHisto->SetLineColor(color);
         
      }

      static void ApplyAxisStyle(Plot* thePlot, TH1F* theHisto, string xlabel, string ylabel, string plotOptions = "")
      {	
         PlotDefaultStyles::ApplyDefaultAxisStyle(theHisto->GetXaxis(),xlabel);
         PlotDefaultStyles::ApplyDefaultAxisStyle(theHisto->GetYaxis(),ylabel);
         theHisto->SetTitle("");
         theHisto->SetStats(0);
      }

    };

}
#endif
