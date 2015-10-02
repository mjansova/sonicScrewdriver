#ifndef Histo1DFrom2DProjection_h
#define Histo1DFrom2DProjection_h

#include <math.h>

#include "interface/histos/Histo1D.h"
#include "interface/ProcessClass.h"
#include "interface/Figure.h"
#include "interface/FigureOfMerit.h"

namespace theDoctor
{

    class Histo1DFrom2DProjection : public Histo1D
    {

     public:

      static void GetHistoDependencies(vector<pair<string,string> >& output, string options = "")
      {

          string projectionType = OptionsScrewdriver::GetStringOption(options,"projectionType");

          if ((projectionType == "maxFigureOfMeritForVarXBeingSignalParameter")
          ||  (projectionType == "cutOptimalFigureOfMeritForVarXBeingSignalParameter"))
          {
              Histo2DFigureOfMeritForVarXBeingSignalParameter::GetHistoDependencies(output);
              output.push_back(pair<string,string>("2DFigureOfMeritForVarXBeingSignalParameter",options));
          }
      }

      static void Produce(vector<Variable>* theVariables,
                          vector<ProcessClass>* theProcessClasses,
                          vector<Region>* theRegions,
                          vector<Channel>* theChannels,
                          HistoScrewdriver* theHistoScrewdriver,
                          OptionsScrewdriver theGlobalOptions,
                          string histoParameters)
      {

          string varXName       = OptionsScrewdriver::GetStringOption(histoParameters,"varX");
          string varYName       = OptionsScrewdriver::GetStringOption(histoParameters,"varY");
          string projectionType = OptionsScrewdriver::GetStringOption(histoParameters,"projectionType");

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
              for (unsigned int p = 0 ; p < theProcessClasses->size() ; p++)
              {
                  Region*       theRegion       = &((*theRegions)[r]);
                  Channel*      theChannel      = &((*theChannels)[c]);
                  ProcessClass* theProcessClass = &((*theProcessClasses)[p]);

                  if ((projectionType == "maxFigureOfMeritForVarXBeingSignalParameter")
                   || (projectionType == "cutOptimalFigureOfMeritForVarXBeingSignalParameter"))
                  {
                      if (theProcessClass->getType() != "signal") continue;

                      Histo2D* thisProcessHisto = theHistoScrewdriver->get2DHistoForPlotPointer("2DFigureOfMeritForVarXBeingSignalParameter",
                                                                                                theXVar->getTag(),
                                                                                                theYVar->getTag(),
                                                                                                theRegion->getTag(),
                                                                                                theChannel->getTag(),
                                                                                                string("sig=")+theProcessClass->getTag());
                      // Produce the plot
                      theHistoScrewdriver->Add1DHistoForPlots(
                              Histo1DFrom2DProjection(theXVar,
                                                      theYVar,
                                                      theRegion,
                                                      theChannel,
                                                      theProcessClass,
                                                      thisProcessHisto,
                                                      theGlobalOptions,
                                                      histoParameters)
                                                             );


                  }
                  else
                  {
                      Histo2DEntries* thisProcessHisto = theHistoScrewdriver->get2DHistoEntriesPointer(theXVar->getTag(),
                                                                                                       theYVar->getTag(),
                                                                                                       theProcessClass->getTag(),
                                                                                                       theRegion->getTag(),
                                                                                                       theChannel->getTag());

                      // Produce the plot
                      theHistoScrewdriver->Add1DHistoForPlots(
                              Histo1DFrom2DProjection(theXVar,
                                                      theYVar,
                                                      theRegion,
                                                      theChannel,
                                                      theProcessClass,
                                                      thisProcessHisto,
                                                      theGlobalOptions,
                                                      histoParameters)
                                                             );
                  }
              }
          }
     }


      ~Histo1DFrom2DProjection() { };

      Histo1DFrom2DProjection(Variable* theXVar,
                              Variable* theYVar,
                              Region*   theRegion_,
                              Channel*  theChannel_,
                              ProcessClass* theProcessClass,
                              Histo2D* theInputHisto,
                              OptionsScrewdriver theGlobalOptions,
                              string histoParameters) :
      Histo1D(Name("1DFrom2DProjection",""),theXVar,theRegion_,theChannel_,string("vY=")+theYVar->getTag()
                                                                             +",proj="+OptionsScrewdriver::GetStringOption(histoParameters,"projectionType")
                                                                                +",p="+theProcessClass->getTag())
      {

          string nameHisto =  string("v:")+theXVar->getTag()
                                   +"|r:" +theRegion_->getTag()
                                   +"|c:" +theChannel_->getTag()
                                   +"|t:" +theHistoType.getTag()
                                   +"|p:" +theProcessClass->getTag();

          theHisto->SetName(nameHisto.c_str());

          string projectionType = OptionsScrewdriver::GetStringOption(histoParameters,"projectionType");

          int   nBinsX = theXVar->getNbins();
          int   nBinsY = theYVar->getNbins();
          float minY   = theYVar->getMin();
          float maxY   = theYVar->getMax();

          TH2D* theInputHisto_ = theInputHisto->getClone();

          for (int i = 1 ; i < nBinsX+1 ; i++)
          {
              // Read bin i on x axis
              TH1D iIshBinHisto("tempHisto","",nBinsY,minY,maxY);
              for (int j = 0 ; j <= nBinsY+1 ; j++)
              {
                  iIshBinHisto.SetBinContent(j,theInputHisto_->GetBinContent(i,j));
                  iIshBinHisto.SetBinError  (j,theInputHisto_->GetBinError  (i,j));
              }

              if (projectionType == "mean")
              {
                  theHisto->SetBinContent(i,iIshBinHisto.GetMean() );
                  theHisto->SetBinError(  i,iIshBinHisto.GetMeanError());
              }
              else if (projectionType == "meanWithSigma")
              {
                  theHisto->SetBinContent(i,iIshBinHisto.GetMean() );
                  theHisto->SetBinError(  i,iIshBinHisto.GetRMS()  );
              }
              else if (projectionType == "maxFigureOfMeritForVarXBeingSignalParameter")
              {
                  theHisto->SetBinContent(i,iIshBinHisto.GetMaximum() );
                  theHisto->SetBinError(  i,0.0);
              }
              else if (projectionType == "cutOptimalFigureOfMeritForVarXBeingSignalParameter")
              {
                  theHisto->SetBinContent(i,iIshBinHisto.GetBinCenter(iIshBinHisto.GetMaximumBin()));
                  theHisto->SetBinError(  i,0.0);
              }
              else
              {
                  theHisto->SetBinContent(i,0.0);
                  theHisto->SetBinError(  i,0.0);
              }


          }

      };

     private:


    };

}

#endif
