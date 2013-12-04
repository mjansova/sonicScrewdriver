#ifndef Histo3DEntries_h
#define Histo3DEntries_h

#include "interface/Histo3D.h"
#include "interface/ProcessClass.h"

namespace theDoctor
{

    class Histo3DEntries : public Histo3D
    {

        public:

        Histo3DEntries(Variable* theXVar_,
                       Variable* theYVar_,
                       Variable* theZVar_,
                       ProcessClass* theProcessClass_,
                       Region* theRegion_,
                       Channel* theChannel_) :
        Histo3D(theXVar_,theYVar_,theZVar_,theRegion_,theChannel_,Name("entries","Entries"))
        {
            theXVar = theXVar_;
            theYVar = theYVar_;
            theProcessClass = theProcessClass_;
            theRegion       = theRegion_;
            theChannel      = theChannel_;

            string nameHisto = string("vX:")+theXVar->getTag()
                                    +"|vY:"+theYVar->getTag()
                                    +"|vZ:"+theZVar->getTag()
                                    +"|p:"+theProcessClass->getTag()
                                    +"|r:"+theRegion->getTag()
                                    +"|c:"+theChannel->getTag()
                                    +"|t:"+theHistoType.getTag();

            theHisto->SetName(nameHisto.c_str());
            theHistoRawEntries = new TH3F("","",
                    theXVar->getNbins(),theXVar->getMin(),theXVar->getMax(),
                    theYVar->getNbins(),theYVar->getMin(),theYVar->getMax(),
                    theZVar->getNbins(),theZVar->getMin(),theZVar->getMax());

            theHistoRawEntries->SetName((nameHisto+"Raw").c_str());
            theHistoRawEntries->Sumw2();

        }; 

        ~Histo3DEntries() { };

        // Accessors
        TH3F*         getEntriesHisto()    { return theHistoRawEntries;                  };
        TH3F*         getEntriesClone()    { return (TH3F*) theHistoRawEntries->Clone(); };
        ProcessClass* getProcessClass()    { return theProcessClass;                           };
        string        getProcessClassTag() { return theProcessClass->getTag();                 };

        // Editors
        void AutoFill(float weight = 1.0)
        {
            Fill(theXVar->getAutoFillValue(),theYVar->getAutoFillValue(),theZVar->getAutoFillValue(),weight);
        }

        void Fill(float valueX = 1.0, float valueY = 1.0, float valueZ = 1.0, float weight = 1.0)
        {
            theHisto->Fill(valueX,valueY,valueZ,weight);
            theHistoRawEntries->Fill(valueX,valueY,valueZ);
        }

        private:

            ProcessClass* theProcessClass;
            TH3F* theHistoRawEntries;
    };

}

#endif