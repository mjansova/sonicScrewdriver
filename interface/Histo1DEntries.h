#ifndef Histo1DEntries_h
#define Histo1DEntries_h


#include "interface/Histo1D.h"
#include "interface/ProcessClass.h"
#include "interface/Figure.h"

namespace theDoctor
{

    class Histo1DEntries : public Histo1D 
    {
      
     public:

      Histo1DEntries(Variable* theVar_, 
                     ProcessClass* theProcessClass_, 
                     Region* theRegion_, 
                     Channel* theChannel_) :
      Histo1D(Name("1DEntries","Entries"),theVar_,theRegion_,theChannel_)
      {

          theProcessClass = theProcessClass_;

          string nameHisto =  string("v:")+theVar->getTag()
                                   +"|p:" +theProcessClass->getTag()
                                   +"|r:" +theRegion->getTag()
                                   +"|c:" +theChannel->getTag();
                                   +"|t:" +theHistoType.getTag();

          theHisto->SetName(nameHisto.c_str());
          theHistoRawEntries = new TH1F("","",theVar->getNbins(),theVar->getMin(),theVar->getMax());
          theHistoRawEntries->SetName((nameHisto+"Raw").c_str());
          theHistoRawEntries->Sumw2();
      }; 

      ~Histo1DEntries() { };

      // Accessors
      ProcessClass*  getProcessClass()     { return theProcessClass;       };  
      string         getProcessClassTag()  { return theProcessClass->getTag(); };
      TH1F* 		 getEntriesHisto()     { return theHistoRawEntries;                   };
      TH1F*          getEntriesClone()     { return (TH1F*) theHistoRawEntries->Clone();  };

      Figure GetYieldAndError()
      {
          double error;
          double yield = theHisto->IntegralAndError(0,theHisto->GetNbinsX()+1,error);

          return Figure(yield,error);
      }

      // Editors
      void AutoFill(float weight = 1.0)
      {
          Fill(theVar->getAutoFillValue(),weight);
      }
          
      void Fill(float value = 1.0, float weight = 1.0)
      {
          theHisto->Fill(value,weight);
          theHistoRawEntries->Fill(value);
      }
      
      void ApplyScaleFactor(Figure scaleFactor)
      { 
            TH1F histoScaleFactor("histoScaleFactor","",theVar->getNbins(),theVar->getMin(),theVar->getMax());
        
            for (int i = 0 ; i <= histoScaleFactor.GetNbinsX()+1 ; i++)
            {
                histoScaleFactor.SetBinContent(i,scaleFactor.value());
                histoScaleFactor.SetBinError(i,scaleFactor.error());
            }

            theHisto->Multiply(&histoScaleFactor);
      }

     private:

      ProcessClass* theProcessClass;
      TH1F* theHistoRawEntries;
    
    };

}

#endif
