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
                           string plotTypeOptions = "",
                           string generalOptions = "")
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
        vector<string> optionsForLegend;

        // Create stack histo
        THStack* theStack = new THStack("","");
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
            ApplyHistoStyle(thePlot,histoClone,(*theProcessClasses)[i].getColor(),generalOptions,processClassOptions);
         
            pointersForLegend.push_back(histoClone);
            optionsForLegend.push_back(string("f"));
			labelsForLegend.push_back((*theProcessClasses)[i].getLabel());

			// Add it to the sumBackground (this is for when signal is included, in stacked mode)
            if (sumBackground == 0)   sumBackground =  (TH1F*) histoClone->Clone();
            else                      sumBackground->Add(histoClone);

            // Add histo to stack
            theStack->Add(histoClone);
        }

        // Apply axis style and plot the stack
        theStack->Draw("HIST");
        ApplyAxisStyle(thePlot,theStack,xlabel,ylabel,generalOptions,theVar->getOptions());

		// Add signal if specified in the options of the plotType
        if (OptionsScrewdriver::GetBoolOption(plotTypeOptions,"includeSignal"))
        {
            float factor = OptionsScrewdriver::GetFloatOption(plotTypeOptions,"factorSignal");
            string factorStr = OptionsScrewdriver::GetStringOption(plotTypeOptions,"factorSignal");
            if (factor == -1.0) factor = 1.0;

            for (unsigned int i = 0 ; i < theProcessClasses->size() ; i++)
            {
                if ((*theProcessClasses)[i].getType() != "signal") continue; 

                string processClassOptions = (*theProcessClasses)[i].getOptions();
                    
                TH1F* histoClone = theHistoScrewdriver->get1DHistoClone(theVar->getTag(),(*theProcessClasses)[i].getTag(),theRegion->getTag(),theChannel->getTag());
                ApplyHistoSignalStyle(thePlot,histoClone,(*theProcessClasses)[i].getColor(),generalOptions,processClassOptions);
                histoClone->Scale(factor);
                
                if (OptionsScrewdriver::GetStringOption(plotTypeOptions,"includeSignalHow") == "stack") 
					histoClone->Add(sumBackground);		
                
				// Add to legend
		        pointersForLegend.insert(pointersForLegend.begin(),histoClone);
		        optionsForLegend.insert(optionsForLegend.begin(),string("l"));
                if (factor == 1.0)
        		    labelsForLegend.insert(labelsForLegend.begin(),(*theProcessClasses)[i].getLabel());
                else
        		    labelsForLegend.insert(labelsForLegend.begin(),factorStr+"#times"+(*theProcessClasses)[i].getLabel());

                histoClone->Draw("hist same");
            }
        }
        

     
        // Add stuff to legend in reverse order
        for (unsigned int leg_i = 0 ; leg_i < pointersForLegend.size() ; leg_i++)
        {
            thePlot->AddToLegend(pointersForLegend[pointersForLegend.size()-1-leg_i],
                                 labelsForLegend[pointersForLegend.size()-1-leg_i].c_str(),
                                 optionsForLegend[pointersForLegend.size()-1-leg_i].c_str());
        }

      }

      static void GetHistoDependencies(vector<pair<string,string> >& output)
      {
      }

     private:

      static void ApplyHistoStyle(Plot* thePlot, TH1F* theHisto, Color_t color, string generalOptions = "", string processClassOptions = "")
      {
         theHisto->SetFillColor(color);
         theHisto->SetLineColor(kBlack);
         theHisto->SetLineWidth(2);
      }
	  
	  static void ApplyHistoSignalStyle(Plot* thePlot, TH1F* theHisto, Color_t color, string generalOptions = "", string processClassOptions = "")
      {
         theHisto->SetFillColor(0);
         theHisto->SetLineWidth(3);
         theHisto->SetLineColor(color);
		 theHisto->SetLineStyle(9);
	  }

      static void ApplyAxisStyle(Plot* thePlot, THStack* theStack, string xlabel, string ylabel, string generalOptions = "", string varOptions = "")
      { 
         PlotDefaultStyles::ApplyDefaultAxisStyle(theStack->GetXaxis(),xlabel);
         PlotDefaultStyles::ApplyDefaultAxisStyle(theStack->GetYaxis(),ylabel);
         theStack->SetTitle("");
         if (OptionsScrewdriver::GetBoolOption(varOptions,"logY")) thePlot->SetLogY();
      }

    };

}

#endif
