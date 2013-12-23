#ifndef Histo3DFigureOfMeritForVarXYBeingSignalParameter_h
#define Histo3DFigureOfMeritForVarXYBeingSignalParameter_h

#include <math.h>

#include "interface/histos/Histo3D.h"
#include "interface/ProcessClass.h"
#include "interface/Figure.h"

namespace theDoctor
{

    class Histo3DFigureOfMeritForVarXYBeingSignalParameter : public Histo3D 
    {
      
     public:

      ~Histo3DFigureOfMeritForVarXYBeingSignalParameter() { };
      
      Histo3DFigureOfMeritForVarXYBeingSignalParameter(Variable* theXVar,
                                                       Variable* theYVar,
                                                       Variable* theZVar,
                                                       Region* theRegion, 
                                                       Channel* theChannel,
                                                       Histo3DEntries* theSignal,
                                                       Histo1D* theSumBackground,
                                                       OptionsScrewdriver theGlobalOptions,
                                                       short int cutType) :
      Histo3D(Name("3DFigureOfMeritForVarXYBeingSignalParameter","Figure of merit"),theXVar,theYVar,theZVar,theRegion,theChannel,string("sig=")+theSignal->getProcessClassTag())
      {

          string nameHisto =  string("vX:")+theXVar->getTag()
                                   +"|vY:" +theYVar->getTag()
                                   +"|vZ:" +theZVar->getTag()
                                   +"|r:"  +theRegion->getTag()
                                   +"|c:"  +theChannel->getTag()
                                   +"|t:"  +theHistoType.getTag()
                                   +"|s:"  +theSignal->getProcessClassTag();

          theHisto->SetName(nameHisto.c_str());
              
          TH3F* sigHisto    = theSignal->getClone();
          TH1F* backgrHisto = theSumBackground->getClone();

          int   nBinsX = theXVar->getNbins(); 
          int   nBinsY = theYVar->getNbins(); 
          int   nBinsZ = theZVar->getNbins(); 
          float minZ   = theZVar->getMin();
          float maxZ   = theZVar->getMax();

          for (int i = 1 ; i < nBinsX+1 ; i++)
          for (int j = 1 ; j < nBinsY+1 ; j++)
          {
              // Read (x,y) histogram for (x,y)=(i,j)
              TH1F tempSignalHisto("","",nBinsZ,minZ,maxZ);
              for (int k = 0 ; k <= nBinsZ+1 ; k++)
                  tempSignalHisto.SetBinContent(k,sigHisto->GetBinContent(i,j,k));

              // Compute the FOM histogram
              TH1F theFigureOfMeritHisto = FigureOfMerit::Compute(&tempSignalHisto,backgrHisto,cutType,theGlobalOptions);

              // Copy it to this histogram
              for (int k = 0 ; k <= nBinsZ+1 ; k++)
              {
                  theHisto->SetBinContent(i,j,k,theFigureOfMeritHisto.GetBinContent(k));
                  theHisto->SetBinError(i,j,k,theFigureOfMeritHisto.GetBinError(k));
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
          string varZName = OptionsScrewdriver::GetStringOption(histoParameters,"varZ");

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
                  Region*       theRegion       = &((*theRegions)[r]);
                  Channel*      theChannel      = &((*theChannels)[c]);

                  // Get the sumBackground
                  Histo1D* theSumBackground = theHistoScrewdriver->get1DHistoForPlotPointer("1DSumBackground",
                                                                                            theZVar->getTag(),
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

                      Histo3DEntries* thisSignal = theHistoScrewdriver->get3DHistoEntriesPointer(theXVar->getTag(),
                                                                                                 theYVar->getTag(),
                                                                                                 theZVar->getTag(),
                                                                                                 theProcessClass->getTag(),
                                                                                                 theRegion->getTag(),
                                                                                                 theChannel->getTag());

                      // Produce the figure of merit histogram
                      theHistoScrewdriver->Add3DHistoForPlots(
                              Histo3DFigureOfMeritForVarXYBeingSignalParameter(theXVar,theYVar,theZVar,
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
