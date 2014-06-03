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
              theHistoRawEntries = new TH2F("","",
                                            theXVar->getNbins(),theXVar->getCustomBinning(),
                                            theYVar->getNbins(),theYVar->getCustomBinning());

            else if (theXVar_->usingCustomBinning())
              theHistoRawEntries = new TH2F("","",
                                            theXVar->getNbins(),theXVar->getCustomBinning(),
                                            theYVar->getNbins(),theYVar->getMin(),theYVar->getMax());
            else if (theXVar_->usingCustomBinning())
              theHistoRawEntries = new TH2F("","",
                                            theXVar->getNbins(),theXVar->getMin(),theXVar->getMax(),
                                            theYVar->getNbins(),theYVar->getCustomBinning());
            else
              theHistoRawEntries = new TH2F("","",
                                            theXVar->getNbins(),theXVar->getMin(),theXVar->getMax(),
                                            theYVar->getNbins(),theYVar->getMin(),theYVar->getMax());

            theHistoRawEntries->SetName((nameHisto+"Raw").c_str());
            theHistoRawEntries->Sumw2();

        }; 

        ~Histo2DEntries() { };

        // Accessors
        TH2F*         getEntriesHisto()    { return theHistoRawEntries;                  };
        TH2F*         getEntriesClone()    { return (TH2F*) theHistoRawEntries->Clone(); };
        ProcessClass* getProcessClass()    { return theProcessClass;                     };
        string        getProcessClassTag() { return theProcessClass->getTag();           };

        // Editors
        void AutoFill(float weight = 1.0)
        {
          // Ignore data event for blinded regions
          if ((theProcessClass->getType() == "data")
           && (OptionsScrewdriver::GetBoolOption(theRegion->getOptions(),"blinded"))) return;

            Fill(theXVar->getAutoFillValue(),theYVar->getAutoFillValue(),weight);
        }

        void Fill(float valueX = 1.0, float valueY = 1.0, float weight = 1.0)
        {
          
            if ((!OptionsScrewdriver::GetBoolOption(theXVar->getOptions(),"noUnderflowInFirstBin"))
             && (valueX < theXVar->getMin())) valueX = theXVar->getMin();
          
            if ((!OptionsScrewdriver::GetBoolOption(theXVar->getOptions(),"noOverflowInLastBin"))
             && (valueX > theXVar->getMax())) valueX = theXVar->getMax() - 0.001; // FIXME Find a better way to do this 

            if ((!OptionsScrewdriver::GetBoolOption(theYVar->getOptions(),"noUnderflowInFirstBin"))
             && (valueY < theYVar->getMin())) valueY = theYVar->getMin();
          
            if ((!OptionsScrewdriver::GetBoolOption(theYVar->getOptions(),"noOverflowInLastBin"))
             && (valueY > theYVar->getMax())) valueY = theYVar->getMax() - 0.001; // FIXME Find a better way to do this 

            theHisto->Fill(valueX,valueY,weight);
            theHistoRawEntries->Fill(valueX,valueY);
        }

        private:

            ProcessClass* theProcessClass;
            TH2F* theHistoRawEntries;
    };

}

#endif
