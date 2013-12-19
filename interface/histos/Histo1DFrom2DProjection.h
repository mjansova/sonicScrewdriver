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
              for (unsigned int p = 0 ; p < theProcessClasses->size() ; p++)
              {
                  Region*       theRegion       = &((*theRegions)[r]);
                  Channel*      theChannel      = &((*theChannels)[c]);
                  ProcessClass* theProcessClass = &((*theProcessClasses)[p]);

                  Histo2DEntries* thisProcessHisto = theHistoScrewdriver->get2DHistoEntriesPointer(theXVar->getTag(),
                                                                                                   theYVar->getTag(),
                                                                                                   theProcessClass->getTag(),
                                                                                                   theRegion->getTag(),
                                                                                                   theChannel->getTag());

                  // Produce the figure of merit histogram
                  theHistoScrewdriver->Add1DHistoForPlots(
                          Histo1DFrom2DProjection(theXVar,
                                                  theYVar,
                                                  theRegion,
                                                  theChannel,
                                                  thisProcessHisto,
                                                  theGlobalOptions,
                                                  histoParameters)
                                                         );
              }
          }
     }


      ~Histo1DFrom2DProjection() { };
      
      Histo1DFrom2DProjection(Variable* theXVar, 
                              Variable* theYVar, 
                              Region*   theRegion, 
                              Channel*  theChannel,
                              Histo2DEntries* theInputHisto,
                              OptionsScrewdriver theGlobalOptions,
                              string histoParameters) :
      Histo1D(Name("1DFrom2DProjection",""),theXVar,theRegion,theChannel,string("vY=")+theYVar->getTag()
                                                                             +",proj="+OptionsScrewdriver::GetStringOption(histoParameters,"projectionType")
                                                                                +",p="+theInputHisto->getProcessClassTag())
      {

          string nameHisto =  string("v:")+theXVar->getTag()
                                   +"|r:" +theRegion->getTag()
                                   +"|c:" +theChannel->getTag()
                                   +"|t:" +theHistoType.getTag()
                                   +"|p:" +theInputHisto->getProcessClassTag();

          theHisto->SetName(nameHisto.c_str());

          string projectionType = OptionsScrewdriver::GetStringOption(histoParameters,"projectionType");

          int   nBinsX = theXVar->getNbins(); 
          int   nBinsY = theYVar->getNbins(); 
          float minY   = theYVar->getMin();
          float maxY   = theYVar->getMax();

          TH2F* theInputHisto_ = theInputHisto->getClone();

          for (int i = 1 ; i < nBinsX+1 ; i++)
          {
              // Read bin i on x axis
              TH1F iIshBinHisto("tempHisto","",nBinsY,minY,maxY);
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
