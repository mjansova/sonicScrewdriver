#ifndef Histo1DFigureOfMerit_h
#define Histo1DFigureOfMerit_h

#include <math.h>

#include "interface/histos/Histo1D.h"
#include "interface/ProcessClass.h"
#include "interface/Figure.h"
#include "interface/FigureOfMerit.h"

namespace theDoctor
{

    class Histo1DFigureOfMerit : public Histo1D
    {

     public:

      ~Histo1DFigureOfMerit() { };

      Histo1DFigureOfMerit(Variable* theVar_,
                           Region* theRegion_,
                           Channel* theChannel_,
                           const Histo1DEntries* theSignal,
                           const Histo1D* theSumBackground,
                           OptionsScrewdriver theGlobalOptions,
                           short int cutType) :
      Histo1D(Name("1DFigureOfMerit","Figure of merit"),theVar_,theRegion_,theChannel_,string("sig=")+theSignal->getProcessClassTag())
      {

          string nameHisto =  string("v:")+theVar_->getTag()
                                   +"|r:" +theRegion_->getTag()
                                   +"|c:" +theChannel_->getTag()
                                   +"|t:" +theHistoType.getTag()
                                   +"|s:" +theSignal->getProcessClassTag();

          theHisto->SetName(nameHisto.c_str());

          TH1D* signalHisto          = theSignal->getHisto();
          TH1D* sumBackgroundHisto   = theSumBackground->getHisto();
          TH1D theFigureOfMeritHisto = FigureOfMerit::Compute(signalHisto,sumBackgroundHisto,cutType,theGlobalOptions);

          // Copy it to this histogram
          int nBins = theFigureOfMeritHisto.GetNbinsX();
          for (int i = 0 ; i <= nBins+1 ; i++)
          {
              theHisto->SetBinContent(i,theFigureOfMeritHisto.GetBinContent(i));
              theHisto->SetBinError(i,theFigureOfMeritHisto.GetBinError(i));
          }
          //DEBUG_MSG << "var = " << theSumBackground->getVariable() << "bkg =    " << theSumBackground->getHisto() << endl;
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
          string varName = OptionsScrewdriver::GetStringOption(histoParameters,"var");
          // Browse the (var x reg x chan) space
          for (unsigned int v = 0 ; v < theVariables->size() ; v++)
          {
              Variable*     theVar          = &((*theVariables)[v]);
              if (theVar->getTag() != varName) continue;

              for (unsigned int r = 0 ; r < theRegions->size()   ; r++)
              for (unsigned int c = 0 ; c < theChannels->size()  ; c++)
              {
                  Region*       theRegion       = &((*theRegions)[r]);
                  Channel*      theChannel      = &((*theChannels)[c]);

                  // Get the sumBackground
                  Histo1D const* theSumBackground = theHistoScrewdriver->get1DHistoForPlotPointer("1DSumBackground",
                                                                                                  theVar->getTag(),
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

                      Histo1DEntries* thisSignal = theHistoScrewdriver->get1DHistoEntriesPointer(theVar->getTag(),
                              theProcessClass->getTag(),
                              theRegion->getTag(),
                              theChannel->getTag());

                      //theSumBackground->dump();
                      // Produce the figure of merit histogram
                      Histo1DFigureOfMerit tmp(theVar,
                                  theRegion,
                                  theChannel,
                                  thisSignal,
                                  theSumBackground,
                                  theGlobalOptions,
                                  cutType);
                      //theSumBackground->dump();
                      theHistoScrewdriver->Add1DHistoForPlots(tmp);
                      //theSumBackground->dump();
                  }
              }
          }
     }


     private:


    };

}

#endif
