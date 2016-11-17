#ifndef Histo2DEntries_h
#define Histo2DEntries_h

#include "interface/histos/Histo2D.h"
#include "interface/ProcessClass.h"
#include "interface/OptionsScrewdriver.h"

namespace theDoctor
{

    class Histo2DEntries : public Histo2D
    {

        public:

        Histo2DEntries(Variable* theXVar_,
                       Variable* theYVar_,
                       ProcessClass* theProcessClass_,
                       Region* theRegion_,
                       Channel* theChannel_) :
        Histo2D(Name("2DEntries","Entries"), theXVar_,theYVar_,theRegion_,theChannel_)
        {
            theProcessClass = theProcessClass_;

            string nameHisto =  string("vX:")+theXVar->getTag()
                                     +"|vY:" +theYVar->getTag()
                                     +"|p:"  +theProcessClass->getTag()
                                     +"|r:"  +theRegion->getTag()
                                     +"|c:"  +theChannel->getTag()
                                     +"|t:"  +theHistoType.getTag();

            theHisto->SetName(nameHisto.c_str());

            if (theXVar_->usingCustomBinning() && theYVar_->usingCustomBinning())
              theHistoRawEntries = new TH2D("","",
                                            theXVar->getNbins(),theXVar->getCustomBinning(),
                                            theYVar->getNbins(),theYVar->getCustomBinning());

            else if (theXVar_->usingCustomBinning())
              theHistoRawEntries = new TH2D("","",
                                            theXVar->getNbins(),theXVar->getCustomBinning(),
                                            theYVar->getNbins(),theYVar->getMin(),theYVar->getMax());
            else if (theXVar_->usingCustomBinning())
              theHistoRawEntries = new TH2D("","",
                                            theXVar->getNbins(),theXVar->getMin(),theXVar->getMax(),
                                            theYVar->getNbins(),theYVar->getCustomBinning());
            else
              theHistoRawEntries = new TH2D("","",
                                            theXVar->getNbins(),theXVar->getMin(),theXVar->getMax(),
                                            theYVar->getNbins(),theYVar->getMin(),theYVar->getMax());

            theHistoRawEntries->SetName((nameHisto+"Raw").c_str());
            theHistoRawEntries->Sumw2();

        };

        ~Histo2DEntries() { };

        // Accessors
        TH2D*         getEntriesHisto()    { return theHistoRawEntries;                  };
        TH2D*         getEntriesClone()    { return (TH2D*) theHistoRawEntries->Clone(); };
        ProcessClass* getProcessClass()    { return theProcessClass;                     };
        string        getProcessClassTag() { return theProcessClass->getTag();           };

        // Editors
        void AutoFill(float weight = 1.0)
        {
          // Ignore data event for blinded regions
          if ((theProcessClass->getType() == "data")
           && (OptionsScrewdriver::GetBoolOption(theRegion->getOptions(),"blinded"))) return;

          uint32_t arraySizeX = theXVar->getArrSize();
          uint32_t arraySizeY = theYVar->getArrSize();

          if(arraySizeX != 0 && arraySizeY != 0)
          {
              double arrayX[arraySizeX];
              double arrayY[arraySizeY];
              if( theXVar->getAutoFillValue() == NULL || theYVar->getAutoFillValue() == NULL)
                  throw std::runtime_error("2D: The Pointer with values which should be filled is NULL, even though the array size is not zero!");
              memcpy(arrayX, theXVar->getAutoFillValue(), arraySizeX*sizeof(double));
              memcpy(arrayY, theYVar->getAutoFillValue(), arraySizeY*sizeof(double));
              for(uint32_t f = 0; f < arraySizeX; f++)
              {
                  for(uint32_t g = 0; g < arraySizeY; g++)
                  {
                      Fill(arrayX[f], arrayY[g], weight);  //@MJ@ TODO this needs to be tested!!!! 3D still needs to be implemented!
                      //std::cout << " value X: " << arrayX[f] << ", weight: " << weight << " value Y: " << arrayY[g]<< std::endl;
                  }
              }
          }
        }

        void Fill(double valueX = 1.0, double valueY = 1.0, float weight = 1.0)
        {

            if ((!OptionsScrewdriver::GetBoolOption(theXVar->getOptions(),"noUnderflowInFirstBin"))
             && (valueX < theXVar->getMin())) valueX = theXVar->getMin();

            if ((!OptionsScrewdriver::GetBoolOption(theXVar->getOptions(),"noOverflowInLastBin"))
             && (valueX >= theXVar->getMax())) valueX = theXVar->getMax() - 0.001; // FIXME Find a better way to do this

            if ((!OptionsScrewdriver::GetBoolOption(theYVar->getOptions(),"noUnderflowInFirstBin"))
             && (valueY < theYVar->getMin())) valueY = theYVar->getMin();

            if ((!OptionsScrewdriver::GetBoolOption(theYVar->getOptions(),"noOverflowInLastBin"))
             && (valueY >= theYVar->getMax())) valueY = theYVar->getMax() - 0.001; // FIXME Find a better way to do this

            theHisto->Fill(valueX,valueY,weight);
            theHistoRawEntries->Fill(valueX,valueY);
        }

        private:

            ProcessClass* theProcessClass;
            TH2D* theHistoRawEntries;
    };

}

#endif
