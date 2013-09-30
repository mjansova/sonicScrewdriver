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
         string ylabel("Figure of merit");

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
        vector<TH1F*> backgrounds;

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

            // Renormalize to raw number of entries
            backgrounds.push_back(histoClone);
        }

        // #################################
        // #####                       #####
        // ###          Signal           ###
        // #####                       #####
        // #################################
        
        string signalName_ = OptionsScrewdriver::getStringOption(plotTypeOptions,"signal");


        TH1F* signalHisto = 0;
        for (int i = theProcessClasses->size()-1 ; i >= 0 ; i--)
        {
            // If this processClass is data or SM monte-carlo, we skip it
            if ((*theProcessClasses)[i].getType() != "signal") continue;

            // Keep only signal specified in options (or keep all signal if none)
            if ((signalName_ != "") && ((*theProcessClasses)[i].getTag() != signalName_)) continue;

            TH1F* histoClone = theHistoScrewdriver->get1DHistoClone(theVar->getTag(),(*theProcessClasses)[i].getTag(),theRegion->getTag(),theChannel->getTag());

            if (signalHisto == 0) signalHisto = (TH1F*) histoClone->Clone();
            else signalHisto->Add(histoClone);

        }

        // ####################################
        // #####                          #####
        // ###   Ratio S/sqrt(B + (fB)^2)   ###
        // #####                          #####
        // ####################################   
       
        string cutType_ = OptionsScrewdriver::getStringOption(plotTypeOptions,"cutType");
        int cutType = -1;
             if (cutType_ == string("keepLowValues"))  cutType = 0; 
        else if (cutType_ == string("keepHighValues")) cutType = 1; 

        TH1F* significanceHisto = (TH1F*) signalHisto->Clone();
        int   nBins = significanceHisto->GetNbinsX();
        float S = 0.0;
        float B = 0.0;
        float f = OptionsScrewdriver::getFloatOption(plotTypeOptions,"backgroundSystematicUncertainty");

        float includeMCUnc = 0.0;
        for (int i = 1 ; i <= nBins ; i++)
        {

            if (cutType == 1)
            {
                S = signalHisto->Integral(i,nBins+1);
                B = sumBackground->Integral(i,nBins+1);
            }
            else if (cutType == 0)
            {
                S = signalHisto->Integral(0,i);
                B = sumBackground->Integral(0,i);
            }
          
            float MCUncSquared = 0.0;
            /*
            if (includeMCUnc > 0.0)
            {
                for (unsigned int j = 0 ; j < backgrounds.size() ; j++)
                {
                    float BTot_Lumi  = backgrounds[j]->Integral(0,nBins+1);
                    float BTot_RawMC = backgrounds[j]->GetEntries();
                    float BSel_Lumi  = backgrounds[j]->Integral(i,nBins+1);
                    float BSel_RawMC = BSel_Lumi * BTot_RawMC / BTot_Lumi;

                    //cout << "BSel_Lumi = " << BSel_Lumi << endl;
                    //cout << "MCunc contribution = " << (   ((BSel_RawMC+1) * (BSel_RawMC+2)) / ((BTot_RawMC+2) * (BTot_RawMC+3))
                    //                                     - ((BSel_RawMC+1) * (BSel_RawMC+1)) / ((BTot_RawMC+2) * (BTot_RawMC+2))
                    //                                   ) * BTot_Lumi * BTot_Lumi << endl;

                    MCUncSquared += (   ((BSel_RawMC+1) * (BSel_RawMC+2)) / ((BTot_RawMC+2) * (BTot_RawMC+3))
                                      - ((BSel_RawMC+1) * (BSel_RawMC+1)) / ((BTot_RawMC+2) * (BTot_RawMC+2))
                                     ) * BTot_Lumi * BTot_Lumi;

                }
                //cout << " " << endl;
            }
            */

            if (B < 1) B = 1;
            float sOverSqrtB = 0.0;
            if (S >= 3) sOverSqrtB = S / sqrt(B + f*f * B*B + includeMCUnc * MCUncSquared);

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
