#ifndef Histo2DFigureOfMeritForVarXBeingSignalParameter_h
#define Histo2DFigureOfMeritForVarXBeingSignalParameter_h

#include <math.h>

#include "interface/histos/Histo2D.h"
#include "interface/ProcessClass.h"
#include "interface/Figure.h"

namespace theDoctor
{

    class Histo2DFigureOfMeritForVarXBeingSignalParameter : public Histo2D
    {

     public:

      ~Histo2DFigureOfMeritForVarXBeingSignalParameter() { };

      Histo2DFigureOfMeritForVarXBeingSignalParameter(Variable* theXVar_,
                           Variable* theYVar_,
                           Region* theRegion_,
                           Channel* theChannel_,
                           Histo2DEntries* theSignal,
                           Histo1D* theSumBackground,
                           OptionsScrewdriver theGlobalOptions,
                           short int cutType) :
      Histo2D(Name("2DFigureOfMeritForVarXBeingSignalParameter","Figure of merit"),theXVar_,theYVar_,theRegion_,theChannel_,string("sig=")+theSignal->getProcessClassTag())
      {

          string nameHisto =  string("vX:")+theXVar_->getTag()
                                   +"|vY:" +theYVar_->getTag()
                                   +"|r:"  +theRegion_->getTag()
                                   +"|c:"  +theChannel_->getTag()
                                   +"|t:"  +theHistoType.getTag()
                                   +"|s:"  +theSignal->getProcessClassTag();

          theHisto->SetName(nameHisto.c_str());

          TH2D* sigHisto    = theSignal->getClone();
          TH1D* backgrHisto = theSumBackground->getClone();

          int   nBinsX = theXVar_->getNbins();
          int   nBinsY = theYVar_->getNbins();
          float minY   = theYVar_->getMin();
          float maxY   = theYVar_->getMax();

          for (int i = 1 ; i < nBinsX+1 ; i++)
          {
              // Read y histogram for x=i
              TH1D tempSignalHisto("","",nBinsY,minY,maxY);
              for (int j = 0 ; j <= nBinsY+1 ; j++)
                  tempSignalHisto.SetBinContent(j,sigHisto->GetBinContent(i,j));

              // Compute the FOM histogram
              TH1D theFigureOfMeritHisto = FigureOfMerit::Compute(&tempSignalHisto,backgrHisto,cutType,theGlobalOptions);

              // Copy it to this histogram
              for (int j = 0 ; j <= nBinsY+1 ; j++)
              {
                  theHisto->SetBinContent(i,j,theFigureOfMeritHisto.GetBinContent(j));
                  theHisto->SetBinError(i,j,theFigureOfMeritHisto.GetBinError(j));
              }

          }
      };

      static void GetHistoDependencies(vector<pair<string,string> >& output, string options = "")
      {
          output.push_back(pair<string,string>("1DSumBackground",""));
      }


      static void Produce(vector<Variable>* theVariables,
                          vector<ProcessClass>* theProcessClasses,
                          vector<Region>* theRegions,
                          vector<Channel>* theChannels,
                          HistoScrewdriver* theHistoScrewdriver,
                          OptionsScrewdriver theGlobalOptions,
                          string histoParameters)
      {

          string varXName = OptionsScrewdriver::GetStringOption(histoParameters,"varX");
          string varYName = OptionsScrewdriver::GetStringOption(histoParameters,"varY");

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
                  Region*       theRegion       = &((*theRegions)[r]);
                  Channel*      theChannel      = &((*theChannels)[c]);

                  // Get the sumBackground
                  Histo1D* theSumBackground = theHistoScrewdriver->get1DHistoForPlotPointer("1DSumBackground",
                                                                                            theYVar->getTag(),
                                                                                            theRegion->getTag(),
                                                                                            theChannel->getTag(),
                                                                                            "");
                  // Get the cut type we're using for this variable
                  string cutType_ = OptionsScrewdriver::GetStringOption(histoParameters,"cutType");
                  int cutType = 0;
                  if (cutType_ == string("keepLowValues"))  cutType = -1;
                  else if (cutType_ == string("keepHighValues")) cutType =  1;

                  // Loop on the signals
                  for (unsigned int p = 0 ; p < theProcessClasses->size() ; p++)
                  {
                      ProcessClass* theProcessClass = &((*theProcessClasses)[p]);
                      if (theProcessClass->getType() != "signal") continue;

                      Histo2DEntries* thisSignal = theHistoScrewdriver->get2DHistoEntriesPointer(theXVar->getTag(),
                                                                                                 theYVar->getTag(),
                                                                                                 theProcessClass->getTag(),
                                                                                                 theRegion->getTag(),
                                                                                                 theChannel->getTag());

                      // Produce the figure of merit histogram
                      theHistoScrewdriver->Add2DHistoForPlots(
                              Histo2DFigureOfMeritForVarXBeingSignalParameter(theXVar,theYVar,
                                                                              theRegion,
                                                                              theChannel,
                                                                              thisSignal,
                                                                              theSumBackground,
                                                                              theGlobalOptions,
                                                                              cutType)
                               );
                  }
              }
          }
     }


     private:


    };

}

#endif
