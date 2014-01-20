#ifndef Plot1DSuperimposed_h
#define Plot1DSuperimposed_h

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

    class Plot1DSuperimposed 
    {
      
     public:
     
      Plot1DSuperimposed();
      ~Plot1DSuperimposed();

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

              theOutput.push_back(
                                    MakePlot(theVar,theRegion,theChannel,theBackgrounds,theSignals,theGlobalOptions)
                                 );
   
          }

          return theOutput;
      }

      static Plot MakePlot(Variable* theVar, 
                           Region* theRegion, 
                           Channel* theChannel,
                           vector<Histo1DEntries*> theBackgrounds,
                           vector<Histo1DEntries*> theSignals,
                           OptionsScrewdriver theGlobalOptions)
      {

         string plotName = string("t:1DSuperimposed|v:")+theVar->getTag()
                                                 +"|r:"+theRegion->getTag()
                                                 +"|c:"+theChannel->getTag();

         Plot thePlot(plotName,"1DSuperimposed",theGlobalOptions,"");
         thePlot.SetParameter("variable",theVar->getTag());
         thePlot.SetParameter("region",theRegion->getTag());
         thePlot.SetParameter("channel",theChannel->getTag());
         thePlot.AddToInPlotInfo(theChannel->getLabel());
         thePlot.AddToInPlotInfo(theRegion->getLabel());

         bool includeSignal = theGlobalOptions.GetGlobalBoolOption("1DSuperimposed","includeSignal");
         
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

        float globalMax = 0.0;
        TH1F* firstHisto = 0;

        // Now loop on the histos
        for (unsigned int i = 0 ; i < theBackgrounds.size() ; i++)
        {
            // Get associated processClass
            ProcessClass* processClass = theBackgrounds[i]->getProcessClass();

            // Get the histo
            TH1F* histoClone = theBackgrounds[i]->getClone();
            ApplyHistoStyle(&thePlot,histoClone,processClass->getColor(),theGlobalOptions,processClass->getOptions());
            
            // Normalize histogram to unity
            if (histoClone->Integral() != 0) histoClone->Scale(1.0/histoClone->Integral());

            // Draw the histo
            if (!firstHisto) 
            {   
                histoClone->Draw("hist E0");      
                ApplyAxisStyle(&thePlot,histoClone,xlabel,ylabel,theGlobalOptions,theVar->getOptions());
                firstHisto = histoClone;
            }
            else            { histoClone->Draw("hist E0 same"); }

            // Get the max value after normalization
            if (globalMax < histoClone->GetMaximum())
                globalMax = histoClone->GetMaximum();


            // Add it to the legend
            thePlot.AddToLegend(histoClone,processClass->getLabelC() ,"l");
        }

		// Add signal if specified in the options of the plot type
        if (includeSignal)
        {
            for (unsigned int i = 0 ; i < theSignals.size() ; i++)
            {
                // Get associated processClass
                ProcessClass* processClass = theSignals[i]->getProcessClass();
                    
                // Get the histo
                TH1F* histoClone = theSignals[i]->getClone();
                ApplyHistoStyle(&thePlot,histoClone,processClass->getColor(),theGlobalOptions,processClass->getOptions());
                histoClone->SetLineStyle(9);
               
                // Normalize histogram to unity
                if (histoClone->Integral() != 0) histoClone->Scale(1.0/histoClone->Integral());

                // Draw the histo
                if (!firstHisto) 
                {   
                    histoClone->Draw("hist E0");      
                    ApplyAxisStyle(&thePlot,histoClone,xlabel,ylabel,theGlobalOptions,theVar->getOptions());
                    firstHisto = histoClone;
                }
                else            { histoClone->Draw("hist E0 same"); }

                // Get the max value after normalization
                if (globalMax < histoClone->GetMaximum())
                    globalMax = histoClone->GetMaximum();

				// Add to legend
                thePlot.AddToLegend(histoClone,processClass->getLabelC() ,"l");
            }

        }
        
        // Set max value for the plot
        firstHisto->SetMaximum(globalMax * 1.3);

        return thePlot;
      }

     private:

      static void ApplyHistoStyle(Plot* thePlot, TH1F* theHisto, Color_t color, OptionsScrewdriver theGlobalOptions, string processClassOptions = "")
      {
         theHisto->SetFillColor(0);
         theHisto->SetLineWidth(6);
         theHisto->SetLineColor(color);
         
      }

      static void ApplyAxisStyle(Plot* thePlot, TH1F* theHisto, string xlabel, string ylabel, OptionsScrewdriver theGlobalOptions, string varOptions = "")
      {    
         PlotDefaultStyles::ApplyDefaultAxisStyle(theHisto->GetXaxis(),xlabel);
         PlotDefaultStyles::ApplyDefaultAxisStyle(theHisto->GetYaxis(),ylabel);
         theHisto->SetTitle("");
         theHisto->SetStats(0);

         if (OptionsScrewdriver::GetBoolOption(varOptions,"logY")) thePlot->SetLogY();
      }


    };

}

#endif
