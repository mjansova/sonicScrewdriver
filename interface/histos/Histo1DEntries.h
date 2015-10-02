#ifndef Histo1DEntries_h
#define Histo1DEntries_h


#include "interface/histos/Histo1D.h"
#include "interface/ProcessClass.h"
#include "interface/Figure.h"
#include "interface/OptionsScrewdriver.h"

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
              +"|c:" +theChannel->getTag()
              +"|t:" +theHistoType.getTag();

          theHisto->SetName(nameHisto.c_str());
          if (theVar_->usingCustomBinning())
              theHistoRawEntries = new TH1D("","",theVar->getNbins(),theVar->getMin(),theVar->getMax());
          else
              theHistoRawEntries = new TH1D("","",theVar->getNbins(),theVar->getCustomBinning());

          theHistoRawEntries->SetName((nameHisto+"Raw").c_str());
          theHistoRawEntries->Sumw2();
      };

      ~Histo1DEntries() { };

      // Accessors
      ProcessClass*  getProcessClass()     const { return theProcessClass;       };
      string         getProcessClassTag()  const { return theProcessClass->getTag(); };
      TH1D*          getEntriesHisto()     const { return theHistoRawEntries;                   };
      TH1D*          getEntriesClone()     const { return (TH1D*) theHistoRawEntries->Clone();  };

      Figure GetYieldAndError() const
      {
          double error;
          double yield = theHisto->IntegralAndError(0,theHisto->GetNbinsX()+1,error);

          return Figure(yield,error);
      }

      // Editors
      void AutoFill(float weight = 1.0) const
      {
          // Ignore data event for blinded regions
          if ((theProcessClass->getType() == "data")
           && (OptionsScrewdriver::GetBoolOption(theRegion->getOptions(),"blinded"))) return;

          Fill(theVar->getAutoFillValue(),weight);
      }

      void Fill(float value = 1.0, float weight = 1.0) const
      {
          //DEBUG_MSG << "integral : " << GetYieldAndError().Print() << endl;

          if ((!OptionsScrewdriver::GetBoolOption(theVar->getOptions(),"noUnderflowInFirstBin"))
           && (value < theVar->getMin())) value = theVar->getMin();

          if ((!OptionsScrewdriver::GetBoolOption(theVar->getOptions(),"noOverflowInLastBin"))
           && (value >= theVar->getMax())) value = theVar->getMax() - 0.001; // FIXME Find a better way to do this

          theHisto->Fill(value,weight);
          theHistoRawEntries->Fill(value);
      }

      void ApplyScaleFactor(Figure scaleFactor) const
      {
            TH1D histoScaleFactor("histoScaleFactor","",theVar->getNbins(),theVar->getMin(),theVar->getMax());

            for (int i = 0 ; i <= histoScaleFactor.GetNbinsX()+1 ; i++)
            {
                histoScaleFactor.SetBinContent(i,scaleFactor.value());
                histoScaleFactor.SetBinError(i,scaleFactor.error());
            }

            theHisto->Multiply(&histoScaleFactor);
      }

     private:

      ProcessClass* theProcessClass;
      TH1D* theHistoRawEntries;

    };

}

#endif
