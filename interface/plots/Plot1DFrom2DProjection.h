#ifndef Plot1DFrom2DProjection_h
#define Plot1DFrom2DProjection_h

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

    class Plot1DFrom2DProjection
    {

     public:

      Plot1DFrom2DProjection();
      ~Plot1DFrom2DProjection();

      static void GetHistoDependencies(vector<pair<string,string> >& output, string options = "")
      {
          Histo1DFrom2DProjection::GetHistoDependencies(output,options);
          output.push_back(pair<string,string>("1DFrom2DProjection",options));
      }

      static vector<Plot> Produce(vector<Variable>* theVariables,
                                  vector<ProcessClass>* theProcessClasses,
                                  vector<Region>* theRegions,
                                  vector<Channel>* theChannels,
                                  HistoScrewdriver* theHistoScrewdriver,
                                  OptionsScrewdriver theGlobalOptions,
                                  string plotOptions)
      {
          vector<Plot> theOutput;

          string varXName       = OptionsScrewdriver::GetStringOption(plotOptions,"varX");
          string varYName       = OptionsScrewdriver::GetStringOption(plotOptions,"varY");
          string projectionType = OptionsScrewdriver::GetStringOption(plotOptions,"projectionType");

          string optionsTag =  string("vY=")+varYName
                                   +",proj="+projectionType;

          // Browse the (var x reg x chan) space
          for (unsigned int vX = 0 ; vX < theVariables->size() ; vX++)
          for (unsigned int vY = 0 ; vY < theVariables->size() ; vY++)
          {
              Variable*     theXVar          = &((*theVariables)[vX]);
              Variable*     theYVar          = &((*theVariables)[vY]);

              if (theXVar->getTag() != varXName) continue;
              if (theYVar->getTag() != varYName) continue;

              for (unsigned int r = 0 ; r < theRegions->size()   ; r++)
              for (unsigned int c = 0 ; c < theChannels->size()  ; c++)
              {
                  vector<Histo1D*> theHistos;
                  vector<ProcessClass*> theHistosProcessClasses;

                  Region*   theRegion  = &((*theRegions)[r]);
                  Channel*  theChannel = &((*theChannels)[c]);

                  // Now loop on the histos
                  for (unsigned int i = 0 ; i < theProcessClasses->size() ; i++)
                  {
                      ProcessClass thisProcess = (*theProcessClasses)[i];

                      if ((projectionType == "maxFigureOfMeritForVarXBeingSignalParameter")
                       || (projectionType == "cutOptimalFigureOfMeritForVarXBeingSignalParameter"))
                      {
                          if (thisProcess.getType() != "signal") continue;
                      }

                      // Get the histo
                      Histo1D* thisHisto = theHistoScrewdriver->get1DHistoForPlotPointer("1DFrom2DProjection",
                                                                                         theXVar->getTag(),
                                                                                         theRegion->getTag(),
                                                                                         theChannel->getTag(),
                                                                                         optionsTag+",p="+thisProcess.getTag());

                      // Add it to the vector
                      theHistos.push_back(thisHisto);
                      theHistosProcessClasses.push_back(&((*theProcessClasses)[i]));
                  }

                  theOutput.push_back(
                          MakePlot(theXVar,theYVar,theRegion,theChannel,theHistos,theHistosProcessClasses,theGlobalOptions,plotOptions)
                          );

              }
          }
          return theOutput;
      }

      static Plot MakePlot(Variable* theXVar,
                           Variable* theYVar,
                           Region* theRegion,
                           Channel* theChannel,
                           vector<Histo1D*> theHistos,
                           vector<ProcessClass*> theHistosProcessClasses,
                           OptionsScrewdriver theGlobalOptions,
                           string plotOptions)
      {

         string projectionType = OptionsScrewdriver::GetStringOption(plotOptions,"projectionType");

         string plotName = string("t:1DFrom2DProjection|vX:")+theXVar->getTag()
                                                     +"|vY:" +theYVar->getTag()
                                                     +"|r:"  +theRegion->getTag()
                                                     +"|c:"  +theChannel->getTag()
                                                     +"|p:"  +projectionType;


         Plot thePlot(plotName,"1DFrom2DProjection",theGlobalOptions,plotOptions);
         thePlot.SetParameter("variableX",theXVar->getTag());
         thePlot.SetParameter("variableY",theYVar->getTag());
         thePlot.SetParameter("region",theRegion->getTag());
         thePlot.SetParameter("channel",theChannel->getTag());
         thePlot.SetParameter("projectionType",projectionType);
         thePlot.SetParameter("tagY",OptionsScrewdriver::GetStringOption(plotOptions,"tagY"));

         thePlot.AddToInPlotInfo(theChannel->getLabel());
         thePlot.AddToInPlotInfo(theRegion->getLabel());

         // Prepare the labels for x and y axis
         // xlabel = labelDeLaVariable (Unité)
         // ylabel = Normalized entries / largeurDeBin Unité

         string xlabel(theXVar->getLabel());
         string ylabel = OptionsScrewdriver::GetStringOption(plotOptions,"labelY");
         ylabel += " / " + floatToString(theXVar->getBinWidth());

         // Add the unit
         if (theXVar->getUnit() != "")
         {
             xlabel += string(" [") + theXVar->getUnit() + string("]");
             ylabel += string(" ") + theXVar->getUnit();
         }

        float globalMax = 0.0;
        TH1D* firstHisto = 0;

        for (unsigned int i = 0 ; i < theHistos.size() ; i++)
        {
            // Get associated processClass
            ProcessClass* processClass = theHistosProcessClasses[i];

            // Get the histo
            TH1D* histoClone = theHistos[i]->getClone();
            ApplyHistoStyle(&thePlot,histoClone,processClass->getColor(),theGlobalOptions,processClass->getOptions());

            // Draw the histo
            if (!firstHisto)
            {
                histoClone->Draw("hist E0");
                ApplyAxisStyle(&thePlot,histoClone,xlabel,ylabel,theGlobalOptions,theXVar->getOptions());
                firstHisto = histoClone;
            }
            else
            {
                histoClone->Draw("hist E0 same");
            }

            // Get the max value
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

      static void ApplyHistoStyle(Plot* thePlot, TH1D* theHisto, Color_t color, OptionsScrewdriver theGlobalOptions, string processClassOptions = "")
      {
         theHisto->SetFillColor(0);
         theHisto->SetLineWidth(6);
         theHisto->SetLineColor(color);

      }

      static void ApplyAxisStyle(Plot* thePlot, TH1D* theHisto, string xlabel, string ylabel, OptionsScrewdriver theGlobalOptions, string varOptions = "")
      {
         PlotDefaultStyles::ApplyDefaultAxisStyle(theHisto->GetXaxis(),xlabel, theGlobalOptions);
         PlotDefaultStyles::ApplyDefaultAxisStyle(theHisto->GetYaxis(),ylabel, theGlobalOptions);
         theHisto->SetTitle("");
         theHisto->SetStats(0);
      }


    };

}

#endif
