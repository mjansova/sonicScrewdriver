#ifndef Histo1DDataMCRatio_h
#define Histo1DDataMCRatio_h


#include "interface/Histo1D.h"
#include "interface/Figure.h"

namespace theDoctor
{

    class Histo1DDataMCRatio : public Histo1D 
    {

        public:

        Histo1DDataMCRatio(Variable* theVar_, 
                vector<ProcessClass>* theProcessClasses_, 
                Region* theRegion_, 
                Channel* theChannel_,
                HistoScrewdriver* theHistoScrewdriver,
                OptionsScrewdriver theGlobalOptions,
                string histoParameters) : 
            Histo1D(Name("dataMCRatio","Data/MC"),theVar_,theRegion_,theChannel_)
        {
            string nameHisto =  string("v:")+theVar->getTag()
                +"|r:" +theRegion->getTag()
                +"|c:" +theChannel->getTag()
                +"|t:" +theHistoType.getTag();

            theHisto->SetName(nameHisto.c_str());

            DEBUG_MSG << endl;
        } 

        ~Histo1DDataMCRatio() { }

        static void GetHistoDependencies(vector<pair<string,string> >& output, string options = "")
        {
            output.push_back(pair<string,string>("1DSumBackground",options));
        }

        private:

    };

}

#endif
