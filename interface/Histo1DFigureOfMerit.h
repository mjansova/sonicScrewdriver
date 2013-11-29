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

      Histo1DFigureOfMerit(Variable* theVar_, 
                           vector<ProcessClass>* theProcessClasses_, 
                           Region* theRegion_, 
                           Channel* theChannel_,
                           HistoScrewdriver* theHistoScrewdriver,
                           OptionsScrewdriver theGlobalOptions,
                           string histoParameters) : 
      Histo1D(Name("1DFigureOfMerit","Figure of merit"),theVar_,theRegion_,theChannel_,histoParameters)
      {

          string nameHisto =  string("v:")+theVar->getTag()
                                   +"|r:" +theRegion->getTag()
                                   +"|c:" +theChannel->getTag();
                                   +"|t:" +theHistoType.getTag();

          theHisto->SetName(nameHisto.c_str());

        // #################################
        // #####                       #####
        // ###       SM / Background     ###
        // #####                       #####
        // #################################

            // Get SumBackground from theHistoScrewdriver
            TH1F* sumBackground;    // TODO FIXME

        // #################################
        // #####                       #####
        // ###          Signal           ###
        // #####                       #####
        // #################################
        
            // Get the signal asked

        string signalName_ = OptionsScrewdriver::GetStringOption(histoParameters,"signal");

        TH1F* signalHisto = 0;
        for (int i = theProcessClasses_->size()-1 ; i >= 0 ; i--)
        {
            ProcessClass theProcessClass = (*theProcessClasses_)[i];

            // If this processClass is data or SM monte-carlo, we skip it
            if (theProcessClass.getType() != "signal") continue;

            // Keep only signal specified in options (or keep all signal if none)
            if ((signalName_ != "") && (theProcessClass.getTag() != signalName_)) continue;

            TH1F* histoClone = theHistoScrewdriver->get1DHistoClone(theVar->getTag(),
                                                                    theProcessClass.getTag(),
                                                                    theRegion->getTag(),
                                                                    theChannel->getTag());

            if (signalHisto == 0) signalHisto = (TH1F*) histoClone->Clone();
            else signalHisto->Add(histoClone);

        }

        // ###########################
        // #####                 #####
        // ###   Compute the FOM   ###
        // #####                 #####
        // ###########################   
       
            // Compute the ratio

        string cutType_ = OptionsScrewdriver::GetStringOption(histoParameters,"cutType");
        int cutType = -1;
             if (cutType_ == string("keepLowValues"))  cutType = 0; 
        else if (cutType_ == string("keepHighValues")) cutType = 1; 

        TH1F* significanceHisto = (TH1F*) signalHisto->Clone();
        int   nBins = significanceHisto->GetNbinsX();
        float S = 0.0;
        float B = 0.0;
        float f = theGlobalOptions.GetGlobalFloatOption("figureOfMerit","backgroundSystematicUncertainty");

        float includeMCUnc = 0.0;
        for (int i = 1 ; i <= nBins ; i++)
        {

            if (cutType == 1)
            {
                S = signalHisto->Integral(i,nBins+1);
                B = sumBackground->Integral(i,nBins+1);
            }
            else if (cutType == 0)
            {
                S = signalHisto->Integral(0,i);
                B = sumBackground->Integral(0,i);
            }
          
            float MCUncSquared = 0.0;
            /*
            if (includeMCUnc > 0.0)
            {
                for (unsigned int j = 0 ; j < backgrounds.size() ; j++)
                {
                    float BTot_Lumi  = backgrounds[j]->Integral(0,nBins+1);
                    float BTot_RawMC = backgrounds[j]->GetEntries();
                    float BSel_Lumi  = backgrounds[j]->Integral(i,nBins+1);
                    float BSel_RawMC = BSel_Lumi * BTot_RawMC / BTot_Lumi;

                    //cout << "BSel_Lumi = " << BSel_Lumi << endl;
                    //cout << "MCunc contribution = " << (   ((BSel_RawMC+1) * (BSel_RawMC+2)) / ((BTot_RawMC+2) * (BTot_RawMC+3))
                    //                                     - ((BSel_RawMC+1) * (BSel_RawMC+1)) / ((BTot_RawMC+2) * (BTot_RawMC+2))
                    //                                   ) * BTot_Lumi * BTot_Lumi << endl;

                    MCUncSquared += (   ((BSel_RawMC+1) * (BSel_RawMC+2)) / ((BTot_RawMC+2) * (BTot_RawMC+3))
                                      - ((BSel_RawMC+1) * (BSel_RawMC+1)) / ((BTot_RawMC+2) * (BTot_RawMC+2))
                                     ) * BTot_Lumi * BTot_Lumi;

                }
                //cout << " " << endl;
            }
            */

            if (B < 1) B = 1;
            float sOverSqrtB = 0.0;
            if (S >= 3) sOverSqrtB = S / sqrt(B + f*f * B*B + includeMCUnc * MCUncSquared);

            significanceHisto->SetBinContent(i,sOverSqrtB);
            significanceHisto->SetBinError(i,0.0);
        
        }
       
      }; 

      static void GetHistoDependencies(vector<pair<string,string> >& output, string options = "")
      {
          output.push_back(pair<string,string>("1DSumBackground",options));
      }

      ~Histo1DFigureOfMerit() { };

     private:


    };

}

#endif
