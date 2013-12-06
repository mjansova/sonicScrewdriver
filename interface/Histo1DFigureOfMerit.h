#ifndef Histo1DFigureOfMerit_h
#define Histo1DFigureOfMerit_h

#include <math.h>

#include "interface/Histo1D.h"
#include "interface/ProcessClass.h"
#include "interface/Figure.h"

namespace theDoctor
{

    class Histo1DFigureOfMerit : public Histo1D 
    {
      
     public:

      ~Histo1DFigureOfMerit() { };
      
      Histo1DFigureOfMerit(Variable* theVar, 
                           Region* theRegion, 
                           Channel* theChannel,
                           Histo1DEntries* theSignal,
                           Histo1D* theSumBackground,
                           OptionsScrewdriver theGlobalOptions,
                           short int cutType) :
      Histo1D(Name("1DFigureOfMerit","Figure of merit"),theVar,theRegion,theChannel,string("sig=")+theSignal->getProcessClassTag())
      {

          string nameHisto =  string("v:")+theVar->getTag()
                                   +"|r:" +theRegion->getTag()
                                   +"|c:" +theChannel->getTag()
                                   +"|t:" +theHistoType.getTag()
                                   +"|s:" +theSignal->getProcessClassTag();

          theHisto->SetName(nameHisto.c_str());
            
          float f = theGlobalOptions.GetGlobalFloatOption("FigureOfMerit","backgroundSystematicUncertainty");

          int nBins = theVar->getNbins(); 

          TH1F* signalHisto = theSignal->getClone();
          TH1F* backgrHisto = theSumBackground->getClone();

          for (int i = 1 ; i <= nBins ; i++)
          {

            float S;
            float B;

            // Keep-high-value case
                  if (cutType == 1) { S = signalHisto->Integral(i,nBins+1); B = backgrHisto->Integral(i,nBins+1); }
            // Keep-low-value case
            else if (cutType == -1) { S = signalHisto->Integral(0,i);       B = backgrHisto->Integral(0,i);       }
            
            if (B < 1) B = 1;
            float sOverSqrtB = 0.0;
            if (S >= 3) sOverSqrtB = S / sqrt(B + f*f * B*B);
            
            theHisto->SetBinContent(i,sOverSqrtB);
            theHisto->SetBinError(i,0.0);
        
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

          // Browse the (var x reg x chan) space
          for (unsigned int v = 0 ; v < theVariables->size() ; v++)
          for (unsigned int r = 0 ; r < theRegions->size()   ; r++)
          for (unsigned int c = 0 ; c < theChannels->size()  ; c++)
          {
              Variable*     theVar          = &((*theVariables)[v]);
              Region*       theRegion       = &((*theRegions)[r]);
              Channel*      theChannel      = &((*theChannels)[c]);
                
              DEBUG_MSG << "v = " << theVar->getTag() << " ; r = " << theRegion->getTag() << " ; c = " << theChannel->getTag() << endl;

              // Get the sumBackground
              Histo1D* theSumBackground = theHistoScrewdriver->get1DHistoForPlotPointer("1DSumBackground",
                                                                                        theVar->getTag(),
                                                                                        theRegion->getTag(),
                                                                                        theChannel->getTag(),
                                                                                        "");
              // Get the cut type we're using for this variable
              DEBUG_MSG << "histoParameters : " << histoParameters << endl;
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

                  // Produce the figure of merit histogram
                  theHistoScrewdriver->Add1DHistoForPlots(
                                                            Histo1DFigureOfMerit(theVar,
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


     private:


    };

}

#endif
