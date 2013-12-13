#ifndef Plot1DFigureOfMerit_h
#define Plot1DFigureOfMerit_h

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

    class Plot1DFigureOfMerit 
    {
      
     public:
     
      Plot1DFigureOfMerit();
      ~Plot1DFigureOfMerit();

      static void GetHistoDependencies(vector<pair<string,string> >& output, string options = "")
      {
          Histo1DSumBackground::GetHistoDependencies(output); 
          output.push_back(pair<string,string>("1DSumBackground",""));
          output.push_back(pair<string,string>("1DFigureOfMerit",options));
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
          
          string varName              = OptionsScrewdriver::GetStringOption(histoOptions,"var");
          
          // Browse the (var x reg x chan) space
          for (unsigned int v = 0 ; v < theVariables->size() ; v++)
          {

              Variable* theVar = &((*theVariables)[v]);
              if (theVar->getTag() != varName) continue;
              
              for (unsigned int r = 0 ; r < theRegions->size()   ; r++)
              for (unsigned int c = 0 ; c < theChannels->size()  ; c++)
              {
                  vector<Histo1D*> theSignals;
                  vector<ProcessClass*> theSignalProcessClasses;

                  Region*   theRegion  = &((*theRegions)[r]);
                  Channel*  theChannel = &((*theChannels)[c]);

                  // Now loop on the histos
                  for (unsigned int i = 0 ; i < theProcessClasses->size() ; i++)
                  {

                      ProcessClass thisProcess = (*theProcessClasses)[i];

                      // If this processClass is not a signal, we skip it
                      if (thisProcess.getType() != "signal") continue;

                      // Get the histo
                      Histo1D* thisHisto = theHistoScrewdriver->get1DHistoForPlotPointer("1DFigureOfMerit",
                              theVar->getTag(),
                              theRegion->getTag(),
                              theChannel->getTag(),
                              string("sig=")+thisProcess.getTag());
                      // Add it to the vector
                      theSignals.push_back(thisHisto);
                      theSignalProcessClasses.push_back(&((*theProcessClasses)[i]));
                  }

                  theOutput.push_back(
                          MakePlot(theVar,theRegion,theChannel,theSignals,theSignalProcessClasses,theGlobalOptions)
                          );

              }
          }
          return theOutput;
      }

      static Plot MakePlot(Variable* theVar, 
                           Region* theRegion, 
                           Channel* theChannel,
                           vector<Histo1D*> theSignals,
                           vector<ProcessClass*> theSignalProcessClasses,
                           OptionsScrewdriver theGlobalOptions)
      {

         string plotName = string("t:1DFigureOfMerit|v:")+theVar->getTag()
                                                  +"|r:"+theRegion->getTag()
                                                  +"|c:"+theChannel->getTag();

         Plot thePlot(plotName,"1DFigureOfMerit",theGlobalOptions,"");
         thePlot.SetParameter("variable",theVar->getTag());
         thePlot.SetParameter("region",theRegion->getTag());
         thePlot.SetParameter("channel",theChannel->getTag());
         
         thePlot.AddToInPlotInfo(theChannel->getLabel());
         thePlot.AddToInPlotInfo(theRegion->getLabel());

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
        
        for (unsigned int i = 0 ; i < theSignals.size() ; i++)
        {
            // Get associated processClass
            ProcessClass* processClass = theSignalProcessClasses[i];

            // Get the histo
            TH1F* histoClone = theSignals[i]->getClone();
            ApplyHistoStyle(&thePlot,histoClone,processClass->getColor(),theGlobalOptions,processClass->getOptions());

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
      }


    };

}

#endif
