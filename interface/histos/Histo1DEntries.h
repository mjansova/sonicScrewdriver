#ifndef Histo1DEntries_h
#define Histo1DEntries_h

#include <cstring>
#include <exception>

#include "interface/histos/Histo1D.h"
#include "interface/ProcessClass.h"
#include "interface/Figure.h"
#include "interface/OptionsScrewdriver.h"

namespace theDoctor
{

    class Histo1DEntries : public Histo1D
    {
    public:
      uint32_t positive;
      uint32_t negative;

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

          uint32_t arraySize = theVar->getArrSize();
          if(arraySize != 0)
          {
              double array[arraySize];
              
              if(theVar->getAutoFillValue() == NULL)
                  throw std::runtime_error("1D: The Pointer with values which should be filled is NULL, even though the array size is not zero!");
              memcpy(array, theVar->getAutoFillValue(), arraySize*sizeof(double));
              for(uint32_t f = 0; f < arraySize; f++)
              {
                  Fill(array[f],weight);  
              }
          }
      }
      void Fill(double value = 1.0, float weight = 1.0) const
      {

          if ((!OptionsScrewdriver::GetBoolOption(theVar->getOptions(),"noUnderflowInFirstBin"))
           && (value < theVar->getMin())) value = theVar->getMin();

          if ((!OptionsScrewdriver::GetBoolOption(theVar->getOptions(),"noOverflowInLastBin"))
           && (value >= theVar->getMax())) value = theVar->getMax() - 0.001; // FIXME Find a better way to do this

          theHisto->Fill(value,weight);
          theHistoRawEntries->Fill(value);
          /*if (theProcessClass->getType() == "data")
              cout << "1D filled for data with weight " << weight << " value " << value << endl;*/

      }


      void CheckAndFillYields(vector<Figure>* yieldsVect)
      {
          uint32_t nBins = theHisto->GetNbinsX();
          if(yieldsVect->size() != nBins)
              throw std::runtime_error("The vector of bin yields has not same lenght as histogram bins");

          for(uint32_t b=0; b<nBins; b++)
          {
              double bError;
              double bValue = theHisto->IntegralAndError(0,theHisto->GetNbinsX()+1, bError);
              
              if(bValue< yieldsVect->at(b).value())
              {
                  theHisto->SetBinContent(b+1, yieldsVect->at(b).value());
                  theHisto->SetBinError(b+1, yieldsVect->at(b).error());
                  cout << "value " << bValue << " was set to value " << yieldsVect->at(b).value() << endl; 
              }
              else
              {
                  //cout << "vector updated from " << yieldsVect->at(b) << " to " << bValue << endl;
                  yieldsVect->at(b) = Figure(bValue,bError);
              }
          }

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

     public:
     

    };

}

#endif
