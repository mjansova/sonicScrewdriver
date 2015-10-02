#ifndef Histo2DFrom3DProjection_h
#define Histo2DFrom3DProjection_h

#include <math.h>

#include "interface/histos/Histo3D.h"
#include "interface/ProcessClass.h"
#include "interface/Figure.h"
#include "interface/FigureOfMerit.h"

namespace theDoctor
{

    class Histo2DFrom3DProjection : public Histo2D
    {

     public:

      static void GetHistoDependencies(vector<pair<string,string> >& output, string options = "")
      {
          string projectionType = OptionsScrewdriver::GetStringOption(options,"projectionType");

          if ((projectionType == "maxFigureOfMeritForVarXYBeingSignalParameter")
          ||  (projectionType == "cutOptimalFigureOfMeritForVarXYBeingSignalParameter"))
              output.push_back(pair<string,string>("3DFigureOfMeritForVarXYBeingSignalParameter",options));
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
          string varZName       = OptionsScrewdriver::GetStringOption(histoParameters,"varZ");
          string projectionType = OptionsScrewdriver::GetStringOption(histoParameters,"projectionType");

          // Browse the (var x reg x chan) space
          for (unsigned int vX = 0 ; vX < theVariables->size() ; vX++)
          for (unsigned int vY = 0 ; vY < theVariables->size() ; vY++)
          for (unsigned int vZ = 0 ; vZ < theVariables->size() ; vZ++)
          {
              Variable* theXVar = &((*theVariables)[vX]);
              Variable* theYVar = &((*theVariables)[vY]);
              Variable* theZVar = &((*theVariables)[vZ]);

              if (theXVar->getTag() != varXName) continue;
              if (theYVar->getTag() != varYName) continue;
              if (theZVar->getTag() != varZName) continue;

              for (unsigned int r = 0 ; r < theRegions->size()   ; r++)
              for (unsigned int c = 0 ; c < theChannels->size()  ; c++)
              for (unsigned int p = 0 ; p < theProcessClasses->size() ; p++)
              {
                  Region*       theRegion       = &((*theRegions)[r]);
                  Channel*      theChannel      = &((*theChannels)[c]);
                  ProcessClass* theProcessClass = &((*theProcessClasses)[p]);
                  /*
                  if ((projectionType == "maxFigureOfMeritForVarXYBeingSignalParameter")
                   || (projectionType == "cutOptimalFigureOfMeritForVarXYBeingSignalParameter"))
                  {
                      if (theProcessClass->getType() != "signal") continue;

                      Histo3D* thisProcessHisto = theHistoScrewdriver->get3DHistoForPlotPointer("3DFigureOfMeritForVarXYBeingSignalParameter",
                                                                                                theXVar->getTag(),
                                                                                                theYVar->getTag(),
                                                                                                theZVar->getTag(),
                                                                                                theRegion->getTag(),
                                                                                                theChannel->getTag(),
                                                                                                string("sig=")+theProcessClass->getTag());
                      // Produce the plot
                      theHistoScrewdriver->Add2DHistoForPlots(
                              Histo2DFrom3DProjection(theXVar,
                                                      theYVar,
                                                      theZVar,
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
                  */
                      Histo3DEntries* thisProcessHisto = theHistoScrewdriver->get3DHistoEntriesPointer(theXVar->getTag(),
                                                                                                       theYVar->getTag(),
                                                                                                       theZVar->getTag(),
                                                                                                       theProcessClass->getTag(),
                                                                                                       theRegion->getTag(),
                                                                                                       theChannel->getTag());

                      // Produce the plot
                      theHistoScrewdriver->Add2DHistoForPlots(
                              Histo2DFrom3DProjection(theXVar,
                                                      theYVar,
                                                      theZVar,
                                                      theRegion,
                                                      theChannel,
                                                      theProcessClass,
                                                      thisProcessHisto,
                                                      theGlobalOptions,
                                                      histoParameters)
                                                     );
                  //}
              }
          }
     }


      ~Histo2DFrom3DProjection() { };

      Histo2DFrom3DProjection(Variable* theXVar_,
                              Variable* theYVar_,
                              Variable* theZVar_,
                              Region*   theRegion_,
                              Channel*  theChannel_,
                              ProcessClass* theProcessClass,
                              Histo3DEntries* theInputHisto,
                              OptionsScrewdriver theGlobalOptions,
                              string histoParameters) :
      Histo2D(Name("2DFrom3DProjection",""),theXVar_,theYVar_,theRegion_,theChannel_,string("vZ=")+theZVar_->getTag()
                                                                                     +",proj="+OptionsScrewdriver::GetStringOption(histoParameters,"projectionType")
                                                                                        +",p="+theProcessClass->getTag())
      {

          string nameHisto =  string("vX:")+theXVar_->getTag()
                                   +"|vY:"+theYVar_->getTag()
                                   +"|r:" +theRegion_->getTag()
                                   +"|c:" +theChannel_->getTag()
                                   +"|t:" +theHistoType.getTag()
                                   +"|p:" +theProcessClass->getTag();

          theHisto->SetName(nameHisto.c_str());

          string projectionType = OptionsScrewdriver::GetStringOption(histoParameters,"projectionType");

          int   nBinsX = theXVar_->getNbins();
          int   nBinsY = theYVar_->getNbins();
          int   nBinsZ = theZVar_->getNbins();
          float minZ   = theZVar_->getMin();
          float maxZ   = theZVar_->getMax();

          TH3D* theInputHisto_ = theInputHisto->getHisto();

          for (int i = 0 ; i <= nBinsX+1 ; i++)
          for (int j = 0 ; j <= nBinsY+1 ; j++)
          {
              // Read bin (i,j)
              TH1D ijIshBinHisto("tempHisto","",nBinsZ,minZ,maxZ);
              for (int k = 0 ; k <= nBinsZ+1 ; k++)
              {
                  ijIshBinHisto.SetBinContent(k,theInputHisto_->GetBinContent(i,j,k));
                  ijIshBinHisto.SetBinError  (k,theInputHisto_->GetBinError  (i,j,k));
              }

              if (projectionType == "mean")
              {
                  theHisto->SetBinContent(i,j,ijIshBinHisto.GetMean() );
                  theHisto->SetBinError(  i,j,ijIshBinHisto.GetMeanError());
              }
              else if (projectionType == "maxFigureOfMeritForVarXYBeingSignalParameter")
              {
                  theHisto->SetBinContent(i,j,ijIshBinHisto.GetMaximum() );
                  theHisto->SetBinError(  i,j,0.0);
              }
              else if (projectionType == "cutOptimalFigureOfMeritForVarXYBeingSignalParameter")
              {
                  theHisto->SetBinContent(i,j,ijIshBinHisto.GetBinCenter(ijIshBinHisto.GetMaximumBin()));
                  theHisto->SetBinError(  i,j,0.0);
              }
              else
              {
                  theHisto->SetBinContent(i,j,0.0);
                  theHisto->SetBinError(  i,j,0.0);
              }


          }

      };

     private:


    };

}

#endif
