#ifndef Histo1DDataMCRatio_h
#define Histo1DDataMCRatio_h


#include "interface/histos/Histo1D.h"
#include "interface/Figure.h"

namespace theDoctor
{

    class Histo1DDataMCRatio : public Histo1D
    {

        public:

        Histo1DDataMCRatio(Variable* theVar_,
                           Region* theRegion_,
                           Channel* theChannel_,
                           Histo1D* theSumData,
                           Histo1D* theSumBackground,
                           OptionsScrewdriver theGlobalOptions) :
        Histo1D(Name("1DDataMCRatio","Data/MC"),theVar_,theRegion_,theChannel_)
        {
            string nameHisto =  string("v:")+theVar->getTag()
                +"|r:" +theRegion->getTag()
                +"|c:" +theChannel->getTag()
                +"|t:" +theHistoType.getTag();

            theHisto->SetName(nameHisto.c_str());

            int regionRebin = 0;
            if (OptionsScrewdriver::GetFloatOption(theRegion->getOptions(),"rebin") != -1.0)
            {
                regionRebin = (int) OptionsScrewdriver::GetFloatOption(theRegion->getOptions(),"rebin");
                if (theVar->getNbins() % regionRebin != 0)
                {
                    WARNING_MSG << "Cannot rebin " << theVar->getTag() << " in region " << theRegion->getTag() << endl;
                    regionRebin = 0;
                }
            }

            TH1D* histoSumData = theSumData->getClone();
            TH1D* histoSumBackground = theSumBackground->getClone();

            if (regionRebin)
            {
                theHisto->Rebin(regionRebin);
                histoSumData->Rebin(regionRebin);
                histoSumBackground->Rebin(regionRebin);
            }

            theHisto->Add(histoSumData);

            // Set the MC errors to 0 so that we can split the uncertainties of MC | Data in the ratio plot
            bool splitUncertaintiesInRatio = (bool) theGlobalOptions.GetGlobalBoolOption("DataMCRatio","splitUncertainties");
            if (splitUncertaintiesInRatio)
            {
               for(int bin=0; bin < histoSumBackground->GetNbinsX(); bin++)
               {
                  histoSumBackground->SetBinError(bin, 0);
               }
            }

            theHisto->Divide(histoSumBackground);

        }

        ~Histo1DDataMCRatio() { }

        static void GetHistoDependencies(vector<pair<string,string> >& output, string options = "")
        {
            output.push_back(pair<string,string>("1DSumBackground",options));
            output.push_back(pair<string,string>("1DSumData",options));
        }

        static void Produce(vector<Variable>* theVariables,
                            vector<ProcessClass>* theProcessClasses,
                            vector<Region>* theRegions,
                            vector<Channel>* theChannels,
                            HistoScrewdriver* theHistoScrewdriver,
                            OptionsScrewdriver theGlobalOptions,
                            string histoOptions)
        {
            // Browse the (var x reg x chan) space
            for (unsigned int v = 0 ; v < theVariables->size() ; v++)
            for (unsigned int r = 0 ; r < theRegions->size()   ; r++)
            for (unsigned int c = 0 ; c < theChannels->size()  ; c++)
            {

                vector<Histo1D*> theDatas;

                Variable* theVar     = &((*theVariables)[v]);
                Region*   theRegion  = &((*theRegions)[r]);
                Channel*  theChannel = &((*theChannels)[c]);

                Histo1D* theSumData = theHistoScrewdriver->get1DHistoForPlotPointer("1DSumData",
                                                                                    theVar->getTag(),
                                                                                    theRegion->getTag(),
                                                                                    theChannel->getTag(),
                                                                                    "");

                Histo1D* theSumBackground = theHistoScrewdriver->get1DHistoForPlotPointer("1DSumBackground",
                                                                                          theVar->getTag(),
                                                                                          theRegion->getTag(),
                                                                                          theChannel->getTag(),
                                                                                          "");

                theDatas.push_back(theSumBackground);

                theHistoScrewdriver->Add1DHistoForPlots(
                        Histo1DDataMCRatio(theVar,theRegion,theChannel,theSumData,theSumBackground,theGlobalOptions)
                        );
            }
        }

        private:

    };

}

#endif
