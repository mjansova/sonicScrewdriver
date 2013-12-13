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
      
      Histo2DFigureOfMeritForVarXBeingSignalParameter(Variable* theXVar,
                           Variable* theYVar,
                           Region* theRegion, 
                           Channel* theChannel,
                           Histo2DEntries* theSignal,
                           Histo1D* theSumBackground,
                           OptionsScrewdriver theGlobalOptions,
                           short int cutType) :
      Histo2D(Name("2DFigureOfMeritForVarXBeingSignalParameter","Figure of merit"),theXVar,theYVar,theRegion,theChannel,string("sig=")+theSignal->getProcessClassTag())
      {


          string nameHisto =  string("vX:")+theXVar->getTag()
                                   +"|vY:" +theYVar->getTag()
                                   +"|r:"  +theRegion->getTag()
                                   +"|c:"  +theChannel->getTag()
                                   +"|t:"  +theHistoType.getTag()
                                   +"|s:"  +theSignal->getProcessClassTag();

          theHisto->SetName(nameHisto.c_str());
              
          TH1F* backgrHisto = theSumBackground->getClone();

          int   nBinsX = theXVar->GetNbins(); 
          float minX   = theXVar->getMin();
          float maxX   = theXVar->getMax();

          for (int i = 1 ; i < nBinsX+1 ; i++)
          {
              int nBinsY = theFigureOfMeritHisto.GetNbinsY();

              // Read bin i on x axis
              TH1F tempSignalHisto("tempSignalHisto","",nBinsX,minX,maxX);
              for (int j = 0 ; j <= nBinsY+1 ; j++)
                  tempSignalHisto->SetBinContent(j,theSignal->GetBinContent(i,j));

              // Compute the FOM histogram
              TH1F* signalHisto = theSignal->getClone();
              TH1F theFigureOfMeritHisto = FigureOfMerit::Compute(signalHisto,backgrHisto,cutType,theGlobalOptions);

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
          output.push_back(pair<string,string>("1DSumBackground",options));
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
                  string cutType_ = OptionsScrewdriver::GetStringOption(histoParameters,"cutTypeVarY");
                  int cutType = 0;
                  if (cutType_ == string("keepLowValues"))  cutType = -1; 
                  else if (cutType_ == string("keepHighValues")) cutType =  1; 

                  // Loop on the signals
                  for (unsigned int p = 0 ; p < theProcessClasses->size() ; p++)
                  {
                      ProcessClass* theProcessClass = &((*theProcessClasses)[p]);
                      if (theProcessClass->getType() != "signal") continue;

                      Histo2DEntries* thisSignal = theHistoScrewdriver->get2DHistoEntriesPointer(theYVar->getTag(),
                                                                                                theXVar->getTag(),
                                                                                                theProcessClass->getTag(),
                                                                                                theRegion->getTag(),
                                                                                                theChannel->getTag());

                      // Produce the figure of merit histogram
                      theHistoScrewdriver->Add2DHistoForPlots(
                              Histo2DFigureOfMeritForVarXBeingSignalParameter(theXVar,theYVar
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
