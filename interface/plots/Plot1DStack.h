#ifndef Plot1DStack_h
#define Plot1DStack_h

#include "interface/Common.h"

#include "interface/Plot.h"
#include "interface/Variable.h"
#include "interface/ProcessClass.h"
#include "interface/histos/Histo1D.h"
#include "interface/histos/Histo2D.h"
#include "interface/PlotDefaultStyles.h"
#include "interface/OptionsScrewdriver.h"

namespace theDoctor
{

    class Plot1DStack 
    {
      
     public:
     
      Plot1DStack();
      ~Plot1DStack();


      static void GetHistoDependencies(vector<pair<string,string> >& output)
      {
      }

      static vector<Plot> Produce(vector<Variable>* theVariables,
                                  vector<ProcessClass>* theProcessClasses,
                                  vector<Region>* theRegions,
                                  vector<Channel>* theChannels,
                                  HistoScrewdriver* theHistoScrewdriver,
                                  OptionsScrewdriver theGlobalOptions,
                                  string histoOptions)
      {
          vector<Plot> theOutput;
          
          for (unsigned int r = 0 ; r < theRegions->size()   ; r++)
          for (unsigned int v = 0 ; v < theVariables->size() ; v++)
          for (unsigned int c = 0 ; c < theChannels->size()  ; c++)
          {
              vector<Histo1DEntries*> theBackgrounds;
              vector<Histo1DEntries*> theSignals;

              Variable* theVar     = &((*theVariables)[v]);
              Region*   theRegion  = &((*theRegions)[r]);
              Channel*  theChannel = &((*theChannels)[c]);
      
              // Now loop on the histos
              for (unsigned int i = 0 ; i < theProcessClasses->size() ; i++)
              {

                  ProcessClass thisProcess = (*theProcessClasses)[i];

                  // If this processClass is not a background nor signal, we skip it
                  if ((thisProcess.getType() != "background")
                   && (thisProcess.getType() != "signal"    )) continue;

                  

                  // If it it, we add it to the relevant backgrounds
                  Histo1DEntries* thisHisto = theHistoScrewdriver->get1DHistoEntriesPointer(theVar->getTag(),
                                                                                            thisProcess.getTag(),
                                                                                            theRegion->getTag(),
                                                                                            theChannel->getTag());

                  if (thisProcess.getType() == "background") theBackgrounds.push_back(thisHisto);
                  if (thisProcess.getType() == "signal")     theSignals.push_back(thisHisto);
              }

              Histo1D* theSumBackground = theHistoScrewdriver->get1DHistoForPlotPointer("1DSumBackground",
                                                                                        theVar->getTag(),
                                                                                        theRegion->getTag(),
                                                                                        theChannel->getTag(),
                                                                                        "");
              theOutput.push_back(
                                    MakePlot(theVar,theRegion,theChannel,
                                             theBackgrounds,theSignals,theSumBackground,
                                             theGlobalOptions)
                                 );
   
          }

          return theOutput;
      }

      static Plot MakePlot(Variable* theVar, 
                           Region* theRegion, 
                           Channel* theChannel,
                           vector<Histo1DEntries*> theBackgrounds,
                           vector<Histo1DEntries*> theSignals,
                           Histo1D* theSumBackground,
                           OptionsScrewdriver theGlobalOptions)
      {

         string plotName = string("t:1DStack|v:")+theVar->getTag()
                                          +"|r:"+theRegion->getTag()
                                          +"|c:"+theChannel->getTag();

         Plot thePlot(plotName,"1DStack",theGlobalOptions,"");
         thePlot.SetParameter("variable",theVar->getTag());
         thePlot.SetParameter("region",theRegion->getTag());
         thePlot.SetParameter("channel",theChannel->getTag());
         thePlot.AddToInPlotInfo(theChannel->getLabel());
         thePlot.AddToInPlotInfo(theRegion->getLabel());
         
         string includeSignal = theGlobalOptions.GetGlobalStringOption("1DStack","includeSignal");
         float  factorSignal  = theGlobalOptions.GetGlobalFloatOption( "1DStack","factorSignal");
         string factorSignalStr = floatToString(factorSignal);
         
         // Prepare the labels for x and y axis
         // xlabel = labelDeLaVariable (Unité)
         // ylabel = Normalized entries / largeurDeBin Unité

         string xlabel(theVar->getLabel());
         string ylabel("Entries / ");
         ylabel += floatToString(theVar->getBinWidth());

         // Add the unit
         if (theVar->getUnit() != "")
         {
            xlabel += string(" [") + theVar->getUnit() + string("]");
            ylabel += string(" ") + theVar->getUnit();
         }

        vector<TH1F*>  pointersForLegend;
        vector<string> labelsForLegend;
        vector<string> optionsForLegend;

        // Create stack histo
        THStack* theStack = new THStack("","");

        // Now loop on the histos
        for (int i = theBackgrounds.size()-1 ; i >= 0 ; i--)
        {
            // Get associated processClass
            ProcessClass* processClass = theBackgrounds[i]->getProcessClass();

            // Get the histo
            TH1F* histoClone = theBackgrounds[i]->getClone();

            // Change style of histo and add it to legend
            ApplyHistoStyle(&thePlot,histoClone,processClass->getColor(),theGlobalOptions,processClass->getOptions());
         
            pointersForLegend.push_back(histoClone);
            optionsForLegend.push_back(string("f"));
			labelsForLegend.push_back(processClass->getLabel());

            // Add histo to stack
            theStack->Add(histoClone);
        }

        // Apply axis style and plot the stack
        theStack->Draw("HIST");
        ApplyAxisStyle(&thePlot,theStack,xlabel,ylabel,theGlobalOptions,theVar->getOptions());

		// Add signal if specified in the options of the plot type
        if ((includeSignal != "") && (includeSignal != "no") && (includeSignal != "false"))
        {
            TH1F* histoSumBackground = theSumBackground->getClone();
            for (unsigned int i = 0 ; i < theSignals.size() ; i++)
            {
                // Get associated processClass
                ProcessClass* processClass = theSignals[i]->getProcessClass();
                    
                TH1F* histoClone = theSignals[i]->getClone();
                ApplyHistoSignalStyle(&thePlot,histoClone,processClass->getColor(),theGlobalOptions,processClass->getOptions());
                histoClone->Scale(factorSignal);
                
                if (includeSignal == "stack") 
                    histoClone->Add(histoSumBackground);
               
                histoClone->Draw("hist same");
                
				// Add to legend
		        pointersForLegend.insert(pointersForLegend.begin(),histoClone);
		        optionsForLegend.insert(optionsForLegend.begin(),string("l"));
                if (factorSignal == 1.0)
        		    labelsForLegend.insert(labelsForLegend.begin(),processClass->getLabel());
                else
        		    labelsForLegend.insert(labelsForLegend.begin(),processClass->getLabel()+"(#times"+factorSignalStr+")");
            }
        }
        
        // Add stuff to legend in reverse order
        for (unsigned int leg_i = 0 ; leg_i < pointersForLegend.size() ; leg_i++)
        {
            thePlot.AddToLegend(pointersForLegend[pointersForLegend.size()-1-leg_i],
                                labelsForLegend[pointersForLegend.size()-1-leg_i].c_str(),
                                optionsForLegend[pointersForLegend.size()-1-leg_i].c_str());
        }


        return thePlot;
      }

     private:

      static void ApplyHistoStyle(Plot* thePlot, TH1F* theHisto, Color_t color, OptionsScrewdriver theGlobalOptions, string processClassOptions = "")
      {
         theHisto->SetFillColor(color);
         theHisto->SetLineColor(kBlack);
         theHisto->SetLineWidth(2);
      }
	  
	  static void ApplyHistoSignalStyle(Plot* thePlot, TH1F* theHisto, Color_t color, OptionsScrewdriver theGlobalOptions, string processClassOptions = "")
      {
         theHisto->SetFillColor(0);
         theHisto->SetLineWidth(3);
         theHisto->SetLineColor(color);
		 theHisto->SetLineStyle(9);
	  }

      static void ApplyAxisStyle(Plot* thePlot, THStack* theStack, string xlabel, string ylabel, OptionsScrewdriver theGlobalOptions, string varOptions = "")
      { 
         theStack->SetMaximum(theStack->GetMaximum() * 1.3);
         PlotDefaultStyles::ApplyDefaultAxisStyle(theStack->GetXaxis(),xlabel);
         PlotDefaultStyles::ApplyDefaultAxisStyle(theStack->GetYaxis(),ylabel);
         theStack->SetTitle("");
         if (OptionsScrewdriver::GetBoolOption(varOptions,"logY")) thePlot->SetLogY();
      }

    };

}

#endif
