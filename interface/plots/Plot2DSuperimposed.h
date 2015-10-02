#ifndef Plot2DSuperimposed_h
#define Plot2DSuperimposed_h

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

    class Plot2DSuperimposed
    {

     public:

      Plot2DSuperimposed();
      ~Plot2DSuperimposed();

      static void GetHistoDependencies(vector<pair<string,string> >& output)
      {
          Histo2DSumBackground::GetHistoDependencies(output);
          output.push_back(pair<string,string>("2DSumBackground",""));
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

          // Get list of 2D couples available
          vector<Histo2DEntries>* histo2DList = theHistoScrewdriver->Get2DHistosEntries();
          std::set< pair<string,string> > varCouples;

          for (unsigned int i = 0 ; i < histo2DList->size() ; i++)
          {
              string varX = ((*histo2DList)[i]).getVariableX()->getTag();
              string varY = ((*histo2DList)[i]).getVariableY()->getTag();

              pair<string,string> couple(varX,varY);

              varCouples.insert(couple);
          }

          // Produce the histo for each couple
          for (std::set< pair<string,string> >::iterator varCouple = varCouples.begin() ; varCouple != varCouples.end() ; varCouple++)
          {

              string varX = varCouple->first;
              string varY = varCouple->second;

              // Browse the (var x reg x chan)
              for (unsigned int vX = 0 ; vX < theVariables->size() ; vX++)
              for (unsigned int vY = 0 ; vY < theVariables->size() ; vY++)
              {
                  Variable* theXVar = &((*theVariables)[vX]);
                  Variable* theYVar = &((*theVariables)[vY]);

                  if (theXVar->getTag() != varX) continue;
                  if (theYVar->getTag() != varY) continue;

                  for (unsigned int r = 0 ; r < theRegions->size()   ; r++)
                  for (unsigned int c = 0 ; c < theChannels->size()  ; c++)
                  {
                      Region*   theRegion  = &((*theRegions)[r]);
                      Channel*  theChannel = &((*theChannels)[c]);

                      Histo2D* theSumBackground  = theHistoScrewdriver->get2DHistoForPlotPointer("2DSumBackground",
                                                                                                 theXVar->getTag(),
                                                                                                 theYVar->getTag(),
                                                                                                 theRegion->getTag(),
                                                                                                 theChannel->getTag(),
                                                                                                 "");

                          // Now loop on the histos
                      for (unsigned int i = 0 ; i < theProcessClasses->size() ; i++)
                      {
                          ProcessClass thisProcess = (*theProcessClasses)[i];

                          // If this processClass is not a signal, we skip it
                          if (thisProcess.getType() != "signal") continue;

                          // If it it, we add it to the relevant backgrounds
                          Histo2DEntries* theSignal = theHistoScrewdriver->get2DHistoEntriesPointer(theXVar->getTag(),
                                                                                                    theYVar->getTag(),
                                                                                                    thisProcess.getTag(),
                                                                                                    theRegion->getTag(),
                                                                                                    theChannel->getTag());

                          theOutput.push_back(
                              MakePlot(theXVar,theYVar,theRegion,theChannel,theSumBackground,theSignal,theGlobalOptions)
                              );

                      }
                  }
              }
          }

          return theOutput;
      }

      static Plot MakePlot(Variable* theXVar,
                           Variable* theYVar,
                           Region* theRegion,
                           Channel* theChannel,
                           Histo2D* theSumBackground,
                           Histo2DEntries* theSignal,
                           OptionsScrewdriver theGlobalOptions)
      {

         ProcessClass* theProcessClass = theSignal->getProcessClass();

         string plotName = string("t:2DSuperimposed|vX:")+theXVar->getTag()
                                                 +"|vY:"+theYVar->getTag()
                                                 +"|p:"+theProcessClass->getTag()
                                                 +"|r:"+theRegion->getTag()
                                                 +"|c:"+theChannel->getTag();

         Plot thePlot(plotName,"2DSuperimposed",theGlobalOptions,"");
         thePlot.SetParameter("variableX",   theXVar->getTag());
         thePlot.SetParameter("variableY",   theYVar->getTag());
         thePlot.SetParameter("processClass",theProcessClass->getTag());
         thePlot.SetParameter("region",      theRegion->getTag());
         thePlot.SetParameter("channel",     theChannel->getTag());
         thePlot.AddToInPlotInfo(theChannel->getLabel());
         thePlot.AddToInPlotInfo(theRegion->getLabel());
         thePlot.AddToInPlotInfo(theProcessClass->getLabel());

         // Prepare the labels for x and y axis
         // xlabel = labelDeLaVariable (Unité)
         // ylabel = Normalized entries / largeurDeBin Unité

         string xlabel(theXVar->getLabel());
         string ylabel(theYVar->getLabel());
         string zlabel("Normalized entries");

         // Add the unit
         if (theXVar->getUnit() != "") xlabel += string(" [") + theXVar->getUnit() + string("]");
         if (theYVar->getUnit() != "") ylabel += string(" [") + theYVar->getUnit() + string("]");

         TH2D* histoBackground = theSumBackground->getClone();
         TH2D* histoSignal     = theSignal->getClone();

         if (histoBackground->Integral() != 0) histoBackground->Scale(1.0 / histoBackground->Integral());
         if (histoSignal    ->Integral() != 0) histoSignal    ->Scale(1.0 / histoSignal    ->Integral());

         ApplyHistoStyle(&thePlot,histoSignal,    theGlobalOptions);
         ApplyHistoStyle(&thePlot,histoBackground,theGlobalOptions);
         histoBackground->SetLineColor(COLORPLOT_BLUE);
         histoSignal    ->SetLineColor(COLORPLOT_ORANGE);

         /*
         histoBackground->Draw("box");
         thePlot.Update();
         ApplyAxisStyle(&thePlot,histoSignal,xlabel,ylabel,theGlobalOptions,theXVar->getOptions(),theYVar->getOptions());
         histoSignal    ->Draw("box");

         thePlot.AddToLegend(histoBackground,"background","l");
         thePlot.AddToLegend(histoSignal    ,theProcessClass->getLabelC(),"l");
         */

         ApplyHistoStyle(&thePlot,histoBackground,theGlobalOptions);
         ApplyAxisStyle(&thePlot,histoBackground,xlabel,ylabel,theGlobalOptions,theXVar->getOptions(),theYVar->getOptions());

         thePlot.getCanvas()->SetRightMargin(0.1);
         histoBackground->Draw("COLZ");
         thePlot.Update();
         TPaletteAxis *pal = (TPaletteAxis*) histoBackground->GetListOfFunctions()->FindObject("palette");
         if (pal != 0)
         {
             pal->SetX1NDC(0.901);
             pal->SetY1NDC(0.1);
             pal->SetX2NDC(0.93);
             pal->SetY2NDC(1.0-thePlot.getCanvas()->GetTopMargin());
         }


         return thePlot;
      }

     private:

      static void ApplyHistoStyle(Plot* thePlot, TH2D* theHisto, OptionsScrewdriver theGlobalOptions)
      {
         theHisto->SetLineWidth(2);
      }

      static void ApplyAxisStyle(Plot* thePlot, TH2D* theHisto, string xlabel, string ylabel, OptionsScrewdriver theGlobalOptions, string varXOptions = "", string varYOptions = "")
      {
          PlotDefaultStyles::ApplyDefaultAxisStyle(theHisto->GetXaxis(),xlabel);
          PlotDefaultStyles::ApplyDefaultAxisStyle(theHisto->GetYaxis(),ylabel);
          //theHisto->SetTitle(title.c_str());
          theHisto->SetTitle("");
          theHisto->SetStats(0);
          thePlot->SetLogZ();
      }


    };

}

#endif
