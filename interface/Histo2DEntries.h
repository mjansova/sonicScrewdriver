#ifndef Histo2DEntries_h
#define Histo2DEntries_h

#include "interface/Histo2D.h"
#include "interface/ProcessClass.h"

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
        Histo2D(theXVar_,theYVar_,theRegion_,theChannel_,Name("entries","Entries"))
        {
            theXVar = theXVar_;
            theYVar = theYVar_;
            theProcessClass = theProcessClass_;
            theRegion       = theRegion_;
            theChannel      = theChannel_;

            string nameHisto =  string("vX:")+theXVar->getTag()
                                +"|vY:"+theYVar->getTag()
                                +"|p:"+theProcessClass->getTag()
                                +"|r:"+theRegion->getTag()
                                +"|c:"+theChannel->getTag()
                                +"|t:" +theHistoType.getTag();

            theHisto->SetName(nameHisto.c_str());
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
        ProcessClass* getProcessClass()    { return theProcessClass;                           };
        string        getProcessClassTag() { return theProcessClass->getTag();                 };

        // Editors
        void AutoFill(float weight = 1.0)
        {
            Fill(theXVar->getAutoFillValue(),theYVar->getAutoFillValue(),weight);
        }

        void Fill(float valueX = 1.0, float valueY = 1.0, float weight = 1.0)
        {
            theHisto->Fill(valueX,valueY,weight);
            theHistoRawEntries->Fill(valueX,valueY);
        }

        private:

            ProcessClass* theProcessClass;
            TH2F* theHistoRawEntries;
    };

}

#endif
