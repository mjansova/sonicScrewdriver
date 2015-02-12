#ifndef Plot2DFrom3DProjection_h
#define Plot2DFrom3DProjection_h

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

    class Plot2DFrom3DProjection
    {

     public:

      Plot2DFrom3DProjection();
      ~Plot2DFrom3DProjection();

      static void GetHistoDependencies(vector<pair<string,string> >& output, string options = "")
      {
          Histo2DFrom3DProjection::GetHistoDependencies(output,options);
          output.push_back(pair<string,string>("2DFrom3DProjection",options));
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
          string varZName       = OptionsScrewdriver::GetStringOption(plotOptions,"varZ");
          string projectionType = OptionsScrewdriver::GetStringOption(plotOptions,"projectionType");

          string optionsTag =  string("vZ=")+varZName
                                   +",proj="+projectionType;

          // Browse the (var x reg x chan) space
          for (unsigned int vX = 0 ; vX < theVariables->size() ; vX++)
          for (unsigned int vY = 0 ; vY < theVariables->size() ; vY++)
          for (unsigned int vZ = 0 ; vZ < theVariables->size() ; vZ++)
          {
              Variable*     theXVar          = &((*theVariables)[vX]);
              Variable*     theYVar          = &((*theVariables)[vY]);
              Variable*     theZVar          = &((*theVariables)[vZ]);

              if (theXVar->getTag() != varXName) continue;
              if (theYVar->getTag() != varYName) continue;
              if (theZVar->getTag() != varZName) continue;

              for (unsigned int r = 0 ; r < theRegions->size()   ; r++)
              for (unsigned int c = 0 ; c < theChannels->size()  ; c++)
              {
                  vector<Histo2D*> theHistos;
                  vector<ProcessClass*> theHistosProcessClasses;

                  Region*   theRegion  = &((*theRegions)[r]);
                  Channel*  theChannel = &((*theChannels)[c]);

                  // Now loop on the histos
                  for (unsigned int i = 0 ; i < theProcessClasses->size() ; i++)
                  {
                      ProcessClass thisProcess = (*theProcessClasses)[i];

                      /*
                      if ((projectionType == "maxFigureOfMeritForVarXYBeingSignalParameter")
                       || (projectionType == "cutOptimalFigureOfMeritForVarXYBeingSignalParameter"))
                      {
                          if (thisProcess.getType() != "signal") continue;
                      }*/

                      // Get the histo
                      Histo2D* thisHisto = theHistoScrewdriver->get2DHistoForPlotPointer("2DFrom3DProjection",
                                                                                         theXVar->getTag(),
                                                                                         theYVar->getTag(),
                                                                                         theRegion->getTag(),
                                                                                         theChannel->getTag(),
                                                                                         optionsTag+",p="+thisProcess.getTag());
                      theOutput.push_back(
                          MakePlot(theXVar,theYVar,theZVar,&thisProcess,theRegion,theChannel,thisHisto,theGlobalOptions,plotOptions)
                          );
                  }


              }
          }
          return theOutput;
      }

      static Plot MakePlot(Variable* theXVar,
                           Variable* theYVar,
                           Variable* theZVar,
                           ProcessClass* theProcessClass,
                           Region* theRegion,
                           Channel* theChannel,
                           Histo2D* theHisto,
                           OptionsScrewdriver theGlobalOptions,
                           string plotOptions)
      {

         string projectionType = OptionsScrewdriver::GetStringOption(plotOptions,"projectionType");

         string plotName = string("t:2DFrom3DProjection|vX:")+theXVar->getTag()
                                                     +"|vY:" +theYVar->getTag()
                                                     +"|vZ:" +theZVar->getTag()
                                                     +"|r:"  +theRegion->getTag()
                                                     +"|c:"  +theChannel->getTag()
                                                     +"|P:"  +theProcessClass->getTag();
                                                     +"|p:"  +projectionType;
         // Prepare the labels for x and y axis

         string xlabel(theXVar->getLabel());
         string ylabel(theYVar->getLabel());
         string zlabel = OptionsScrewdriver::GetStringOption(plotOptions,"labelZ");

         // Add the unit
         if (theXVar->getUnit() != "") xlabel += string(" [") + theXVar->getUnit() + string("]");
         if (theYVar->getUnit() != "") ylabel += string(" [") + theYVar->getUnit() + string("]");

         Plot thePlot(plotName,"2DFrom3DProjection",theGlobalOptions,plotOptions);
         thePlot.SetParameter("variableX",theXVar->getTag());
         thePlot.SetParameter("variableY",theYVar->getTag());
         thePlot.SetParameter("variableZ",theZVar->getTag());
         thePlot.SetParameter("processClass",theProcessClass->getTag());
         thePlot.SetParameter("region",theRegion->getTag());
         thePlot.SetParameter("channel",theChannel->getTag());
         thePlot.SetParameter("projectionType",projectionType);
         thePlot.SetParameter("tagZ",OptionsScrewdriver::GetStringOption(plotOptions,"tagZ"));

         thePlot.AddToInPlotInfo(theChannel->getLabel());
         thePlot.AddToInPlotInfo(theRegion->getLabel());
         thePlot.AddToInPlotInfo(theProcessClass->getLabel());
         thePlot.AddToInPlotInfo(zlabel);

         // Get the histo
         TH2F* histoClone = theHisto->getClone();

         ApplyHistoStyle(&thePlot,histoClone,theProcessClass->getColor(),theGlobalOptions,theProcessClass->getOptions());
         ApplyAxisStyle(&thePlot,histoClone,xlabel,ylabel,theGlobalOptions,theXVar->getOptions(),theYVar->getOptions());

         thePlot.getCanvas()->SetRightMargin(0.1);
         histoClone->Draw("COLZ");
         thePlot.Update();
         TPaletteAxis *pal = (TPaletteAxis*) histoClone->GetListOfFunctions()->FindObject("palette");
         if (pal != 0)
         {
             pal->SetX1NDC(0.901);
             pal->SetY1NDC(0.1);
             pal->SetX2NDC(0.93);
             pal->SetY2NDC(1.0-thePlot.getCanvas()->GetTopMargin());
         }

         return thePlot;


        return thePlot;
      }

     private:

      static void ApplyHistoStyle(Plot* thePlot, TH2F* theHisto, Color_t color, OptionsScrewdriver theGlobalOptions, string processClassOptions = "")
      {
      }

      static void ApplyAxisStyle(Plot* thePlot, TH2F* theHisto, string xlabel, string ylabel, OptionsScrewdriver theGlobalOptions, string varXOptions = "", string varYOptions = "")
      {
          PlotDefaultStyles::ApplyDefaultAxisStyle(theHisto->GetXaxis(),xlabel);
          PlotDefaultStyles::ApplyDefaultAxisStyle(theHisto->GetYaxis(),ylabel);
          //theHisto->SetTitle(title.c_str());
          theHisto->SetTitle("");
          theHisto->SetStats(0);
          //thePlot->SetLogZ();
      }

    };

}

#endif
