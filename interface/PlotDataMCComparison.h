#ifndef PlotDataMCComparison_h
#define PlotDataMCComparison_h

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

    class PlotDataMCComparison 
    {
      
     public:
     
      PlotDataMCComparison();
      ~PlotDataMCComparison();

      static void MakePlot(Variable* theVar, 
                           vector<ProcessClass>* theProcessClasses, 
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
         string ylabel("Entries / ");
         
         // Get the bin width and concatenate it with ylabel
         std::ostringstream ss;
         ss.precision(3);
         ss << theHistoScrewdriver->get1DHistoClone(theVar->getTag(),(*theProcessClasses)[0].getTag(),theRegion->getTag(),theChannel->getTag())->GetBinWidth(1);
         ylabel += ss.str();

         // Add the unit
         if (theVar->getUnit() != "")
         {
            xlabel += string(" [") + theVar->getUnit() + string("]");
            ylabel += string(" ") + theVar->getUnit();
         }


        vector<TH1F*> pointersForLegend;
        vector<string> labelsForLegend;

        // #################################
        // #####                       #####
        // ###       SM / Background     ###
        // #####                       #####
        // #################################

        // Create stack histo + another histo for the error plotting for the background sum
        thePlot->AddPad(0,0,1,0.8);
        THStack* stackBackground = new THStack("","");
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

            // Change style of histo and add it to legend
            ApplyHistoStyle(thePlot,histoClone,(*theProcessClasses)[i].getColor(),plotOptions,processClassOptions);
        
            // Add to legend
            pointersForLegend.push_back(histoClone);
            labelsForLegend.push_back((*theProcessClasses)[i].getLabel());

            // Add it to the sumBackground (error will be extracted from the sum of bkg)
            if (sumBackground == 0)   sumBackground =  (TH1F*) histoClone->Clone();
            else                      sumBackground->Add(histoClone);

            // Add histo to stack
            stackBackground->Add(histoClone);
        }

        // Apply axis style and plot the stack
        stackBackground->Draw("HIST");
        ApplyAxisStyle(thePlot,stackBackground,xlabel,ylabel,plotOptions,theVar->getOptions());

        // Apply style to error histo and draw the errors on top of the stack
        ApplyErrorStyle(thePlot,sumBackground,plotOptions);
        //sumBackground->Draw("SAMEPE1X0");

        // #################################
        // #####                       #####
        // ###           Data            ###
        // #####                       #####
        // #################################
        
        TH1F* dataHisto = 0;

        for (int i = theProcessClasses->size()-1 ; i >= 0 ; i--)
        {

            // If this processClass is not data, we skip it
            if ((*theProcessClasses)[i].getType() != "data") continue;

            TH1F* histoClone = theHistoScrewdriver->get1DHistoClone(theVar->getTag(),(*theProcessClasses)[i].getTag(),theRegion->getTag(),theChannel->getTag());

            if (dataHisto == 0) dataHisto = (TH1F*) histoClone->Clone();
            else dataHisto->Add(histoClone);

        }

        pointersForLegend.push_back(dataHisto);
        labelsForLegend.push_back(string("data"));

        // Apply style to data and draw it
        ApplyDataStyle(thePlot,dataHisto,plotOptions);
        dataHisto->Draw("SAME E");
        
        // #################################
        // #####                       #####
        // ###          Legend           ###
        // #####                       #####
        // #################################
     
        // Add stuff to legend in reverse order
        for (unsigned int leg_i = 0 ; leg_i < pointersForLegend.size() ; leg_i++)
        {
            string style;
            if (labelsForLegend[pointersForLegend.size()-1-leg_i] == string("data")) style = "pl";
            else style = "f";

            thePlot->AddToLegend(pointersForLegend[pointersForLegend.size()-1-leg_i],
                                 labelsForLegend[pointersForLegend.size()-1-leg_i].c_str(),
                                 style.c_str());
        }

        // #################################
        // #####                       #####
        // ###      Ratio data/SM        ###
        // #####                       #####
        // #################################   
        
        // Compute and draw the ratio
        
        thePlot->AddPad(0,0.8,1,1);

        TH1F* ratio = (TH1F*) dataHisto->Clone();
        ratio->Divide(sumBackground);
        
        TF1* unity = new TF1("unity","1",-1000,1000);
        unity->SetLineColor(kBlack);
        unity->SetLineStyle(1);
        unity->SetLineWidth(1);

        ApplyRatioStyle(thePlot,ratio,plotOptions);
        ApplyRatioAxisStyle(thePlot,ratio,plotOptions,theVar->getOptions());

        ratio->Draw("E");
        unity->Draw("SAME");
        
        //padStack->cd();
        thePlot->SetActive();


        // Temporary code to save stuff for analysis ...
    /* 
        if (theVar->getTag() == "MET_beforeTauVeto") ratioForMETBeforeTauVeto = (TH1F*) ratio->Clone();
        if (theVar->getTag() == "MET_afterTauVeto")  ratioForMETAfterTauVeto  = (TH1F*) ratio->Clone();
        
        if (theVar->getTag() == "MT_beforeTauVeto")  ratioForMTBeforeTauVeto  = (TH1F*) ratio->Clone();
        if (theVar->getTag() == "MT_afterTauVeto")   ratioForMTAfterTauVeto  = (TH1F*) ratio->Clone();

        if (theVar->getTag() == "MET_beforeTauVeto")
        {
            yieldDataBeforeTauVeto = dataHisto->Integral();
            yieldMCBeforeTauVeto   = sumBackground->Integral();
        }
        if (theVar->getTag() == "MET_afterTauVeto")
        {
            yieldDataAfterTauVeto = dataHisto->Integral();
            yieldMCAfterTauVeto   = sumBackground->Integral();
        }
    */


      }

     private:

      static void ApplyHistoStyle(Plot* thePlot, TH1F* theHisto, Color_t color, string plotOptions = "", string processClassOptions = "")
      {
         theHisto->SetFillColor(color);

        
         if (OptionsScrewdriver::isInOptions(processClassOptions,"noLine")) 
         {  
             theHisto->SetLineColor(color);
             theHisto->SetLineWidth(1);
             theHisto->SetLineStyle(3);
         }
         else 
         {
       
            theHisto->SetLineColor(kBlack);
            theHisto->SetLineWidth(2);
         }
      }

      static void ApplyErrorStyle(Plot* thePlot, TH1F* theError, string plotOptions = "")
      {
         theError->SetMarkerSize(0);
         theError->SetLineWidth(2);
         theError->SetFillStyle(0);
      }

      static void ApplyDataStyle(Plot* thePlot, TH1F* theData, string plotOptions = "")
      {
         theData->SetMarkerStyle(8);
         theData->SetMarkerSize(1);
         theData->SetLineWidth(1);
         theData->SetLineColor(kBlack);
         theData->SetFillStyle(0);
      }

      static void ApplyAxisStyle(Plot* thePlot, THStack* theStack, string xlabel, string ylabel, string plotOptions = "", string varOptions = "")
      { 
         PlotDefaultStyles::ApplyDefaultAxisStyle(theStack->GetXaxis(),xlabel);
         PlotDefaultStyles::ApplyDefaultAxisStyle(theStack->GetYaxis(),ylabel);
         theStack->SetTitle("");
         if (OptionsScrewdriver::getBoolOption(varOptions,"logY")) thePlot->SetLogY();
      }

      static void ApplyRatioStyle(Plot* thePlot, TH1F* theRatio, string plotOptions = "")
      {
         theRatio->SetMarkerStyle(8);
         theRatio->SetMarkerSize(1);
         theRatio->SetLineWidth(1);
         theRatio->SetLineColor(kBlack);
         theRatio->SetFillStyle(0);
      }

      static void ApplyRatioAxisStyle(Plot* thePlot, TH1F* theRatio, string plotOptions = "", string varOptions = "")
      { 
         // Y axis
         PlotDefaultStyles::ApplyDefaultAxisStyle(theRatio->GetYaxis(),string("data/SM"));
         theRatio->GetYaxis()->CenterTitle();
         theRatio->GetYaxis()->SetTickLength(0.015);
         theRatio->GetYaxis()->SetLabelSize(0.15);
         theRatio->GetYaxis()->SetTitleSize(0.17);
         theRatio->GetYaxis()->SetTitleOffset(0.25);
         theRatio->SetMaximum(1.5);
         theRatio->SetMinimum(0.5);
         theRatio->GetYaxis()->SetNdivisions(4);
         
         // X axis
         PlotDefaultStyles::ApplyDefaultAxisStyle(theRatio->GetXaxis(),string(""));
         theRatio->GetXaxis()->SetLabelSize(0.0);
         theRatio->GetXaxis()->SetTickLength(0.1);

         // Misc stuff
         theRatio->SetTitle("");
         theRatio->SetStats(0);

      }
    };

}
#endif
