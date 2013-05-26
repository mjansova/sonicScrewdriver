#ifndef Plot1DStack_h
#define Plot1DStack_h

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

    class Plot1DStack 
    {
      
     public:
     
      Plot1DStack();
      ~Plot1DStack();

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

        // Create stack histo
        THStack* theStack = new THStack("","");

        // Now loop on the histos
        for (int i = theProcessClasses->size()-1 ; i >= 0 ; i--)
        {

            // Get the options for the processClass
            string processClassOptions = (*theProcessClasses)[i].getOptions();

            // Get the histo
            TH1F* histoClone = theHistoScrewdriver->get1DHistoClone(theVar->getTag(),(*theProcessClasses)[i].getTag(),theRegion->getTag(),theChannel->getTag());

            // Change style of histo and add it to legend
            ApplyHistoStyle(thePlot,histoClone,(*theProcessClasses)[i].getColor(),plotOptions,processClassOptions);
         
            pointersForLegend.push_back(histoClone);
            labelsForLegend.push_back((*theProcessClasses)[i].getLabel());

            // Add histo to stack
            theStack->Add(histoClone);
        }

        // Apply axis style and plot the stack
        theStack->Draw("HIST");
        ApplyAxisStyle(thePlot,theStack,xlabel,ylabel,plotOptions,theVar->getOptions());

     
        // Add stuff to legend in reverse order
        for (unsigned int leg_i = 0 ; leg_i < pointersForLegend.size() ; leg_i++)
        {
            thePlot->AddToLegend(pointersForLegend[pointersForLegend.size()-1-leg_i],
                                 labelsForLegend[pointersForLegend.size()-1-leg_i].c_str(),
                                 "f");
        }

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

      static void ApplyAxisStyle(Plot* thePlot, THStack* theStack, string xlabel, string ylabel, string plotOptions = "", string varOptions = "")
      { 
         PlotDefaultStyles::ApplyDefaultAxisStyle(theStack->GetXaxis(),xlabel);
         PlotDefaultStyles::ApplyDefaultAxisStyle(theStack->GetYaxis(),ylabel);
         theStack->SetTitle("");
         //theStack->SetStats(0);
         thePlot->SetLogY();
      }

    };

}

#endif
