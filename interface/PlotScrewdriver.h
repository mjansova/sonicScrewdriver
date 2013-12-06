#ifndef PlotScrewdriver_h
#define PlotScrewdriver_h

#include "interface/Common.h"
#include "interface/OptionsScrewdriver.h"

#include "interface/Plot.h"
#include "interface/Variable.h"
#include "interface/ProcessClass.h"

// Histograms producers
#include "interface/Histo1DEntries.h"
#include "interface/Histo1DSumBackground.h"
#include "interface/Histo1DSumData.h"
#include "interface/Histo1DFigureOfMerit.h"
#include "interface/Histo2DEntries.h"

// Plot producers
#include "interface/Plot1DSuperpRenorm.h"
#include "interface/Plot1DStack.h"
#include "interface/Plot1DFigureOfMerit.h"
#include "interface/Plot1DDataMCComparison.h"
#include "interface/Plot2D.h"
#include "interface/Plot2DProjectedTo1D.h"
#include "interface/Plot3DProjectedTo2D.h"

namespace theDoctor
{

    class PlotScrewdriver 
    {

        public:

        PlotScrewdriver() { }
        ~PlotScrewdriver() { }

        //    thePlots.push_back(Plot("",type,options));


        void ScheduleHisto(string histoType, string options = "")
        {
            pair<string,string> histo(histoType,options);

            for (unsigned int i = 0 ; i < scheduledHistos.size() ; i++)
            {
                if (scheduledHistos[i] == histo) return;
            }

            scheduledHistos.push_back(histo);
        };

        void SchedulePlots(string plotType, string options = "")
        {
            vector<pair<string,string> > dependencies;

                 if (plotType == "1DSuperpRenorm")     Plot1DSuperpRenorm    ::GetHistoDependencies(dependencies);
            else if (plotType == "1DStack")            Plot1DStack           ::GetHistoDependencies(dependencies);
            else if (plotType == "1DFigureOfMerit")    Plot1DFigureOfMerit   ::GetHistoDependencies(dependencies,options);
            else if (plotType == "1DDataMCComparison") Plot1DDataMCComparison::GetHistoDependencies(dependencies);
            else if (plotType == "2D")                 Plot2D                ::GetHistoDependencies(dependencies);
            else if (plotType == "2DProjectedTo1D")    Plot2DProjectedTo1D   ::GetHistoDependencies(dependencies);
            else if (plotType == "3DProjectedTo2D")    Plot3DProjectedTo2D   ::GetHistoDependencies(dependencies);
            else { WARNING_MSG << "Plot-type '" << plotType << "' unknown." << endl; return; }

            // Schedule histo needed for plot
            for (unsigned int i = 0 ; i < dependencies.size() ; i++) ScheduleHisto(dependencies[i].first,dependencies[i].second);

            // Schedule plot
            pair<string,string> plot(plotType,options);
            scheduledPlots.push_back(plot);

        };

        void MakeHistoForPlots(vector<Variable>* theVariables,
                vector<ProcessClass>* theProcessClasses,
                vector<Region>* theRegions,
                vector<Channel>* theChannels,
                HistoScrewdriver* theHistoScrewdriver)
        {
            DEBUG_MSG << "in make histo for plots" << endl;
            for (unsigned int i = 0 ; i < scheduledHistos.size() ; i++)
            {

                pair<string,string> histo = scheduledHistos[i];
                string histoType = histo.first;
                string histoOptions = histo.second;

                DEBUG_MSG << " scheduledHisto : " << histoType << " ; " << histoOptions << endl;

                if (histoType == "1DSumBackground") 
                    Histo1DSumBackground::Produce(theVariables,theProcessClasses,theRegions,theChannels,theHistoScrewdriver,theGlobalOptions,histoOptions);
                else if (histoType == "1DSumData") 
                    Histo1DSumData      ::Produce(theVariables,theProcessClasses,theRegions,theChannels,theHistoScrewdriver,theGlobalOptions,histoOptions);
                else if (histoType == "1DDataMCRatio") 
                    Histo1DDataMCRatio  ::Produce(theVariables,theProcessClasses,theRegions,theChannels,theHistoScrewdriver,theGlobalOptions,histoOptions);
                else if (histoType == "1DFigureOfMerit")
                    Histo1DFigureOfMerit::Produce(theVariables,theProcessClasses,theRegions,theChannels,theHistoScrewdriver,theGlobalOptions,histoOptions);
            }
        }


        void MakePlots(vector<Variable>* theVariables,
                vector<ProcessClass>* theProcessClasses,
                vector<Region>* theRegions,
                vector<Channel>* theChannels,
                HistoScrewdriver* theHistoScrewdriver,
                string options = "")
        {
            MakeHistoForPlots(theVariables,
                              theProcessClasses,
                              theRegions,
                              theChannels,
                              theHistoScrewdriver);

            // For each scheduled plot
            for (unsigned int i = 0 ; i < scheduledPlots.size() ; i++)
            {
                pair<string,string> plot = scheduledPlots[i];
                string plotType          = plot.first;
                string plotOptions       = plot.second;

                DEBUG_MSG << "make plot : " << plotType << " ; options = " << plotOptions << endl;

                vector<Plot> inputFromProducer;
                if (plotType == "1DStack")
                    inputFromProducer = Plot1DStack           ::Produce(theVariables, theProcessClasses, theRegions, theChannels, theHistoScrewdriver, theGlobalOptions);
                if (plotType == "1DSuperpRenorm")
                    inputFromProducer = Plot1DSuperpRenorm    ::Produce(theVariables, theProcessClasses, theRegions, theChannels, theHistoScrewdriver, theGlobalOptions);
                if (plotType == "1DFigureOfMerit")
                    inputFromProducer = Plot1DFigureOfMerit   ::Produce(theVariables, theProcessClasses, theRegions, theChannels, theHistoScrewdriver, theGlobalOptions);
                /*
                if (plotType == "1DDataMCComparison")
                    inputFromProducer = Plot1DDataMCComparison::Produce(theVariables, theProcessClasses, theRegions, theChannels, theHistoScrewdriver, theGlobalOptions);
                if (plotType == "2D")
                    inputFromProducer = Plot2D                ::Produce(theVariables, theProcessClasses, theRegions, theChannels, theHistoScrewdriver, theGlobalOptions);
                if (plotType == "2DProjectedTo1D")
                    inputFromProducer = Plot2DProjectedTo1D   ::Produce(theVariables, theProcessClasses, theRegions, theChannels, theHistoScrewdriver, theGlobalOptions);
                if (plotType == "3DProjectedTo2D")
                    inputFromProducer = Plot3DProjectedTo2D   ::Produce(theVariables, theProcessClasses, theRegions, theChannels, theHistoScrewdriver, theGlobalOptions);
                */

                for (unsigned int j = 0 ; j < inputFromProducer.size() ; j++)
                {
                    thePlots.push_back(inputFromProducer[j]);
                }
            }

        }

        void WritePlots(vector<Channel>* theChannels, vector<Region>* theRegions, string outputFolder, string infoText, string options = "")
        {

            TDirectory* channelDir = 0;
            TDirectory* regionDir  = 0;
            TDirectory* varDir     = 0; 

            int ret;
            ret = system(("rm -f "+outputFolder+"/1DStack.root").c_str());
            ret = system(("rm -f "+outputFolder+"/1DSuperpRenorm.root").c_str());
            ret = system(("rm -f "+outputFolder+"/1DFigureOfMerit.root").c_str());
            ret = system(("rm -f "+outputFolder+"/1DDataMCComparison.root").c_str());
            ret = system(("rm -f "+outputFolder+"/2D.root").c_str());
            ret = system(("rm -f "+outputFolder+"/2DProjectedTo1D.root").c_str());
            ret = system(("rm -f "+outputFolder+"/3DProjectedTo2D.root").c_str());
            // Fix "ret not used" warning
            ret = ret + 1;

            for (unsigned int i = 0 ; i < scheduledPlots.size() ; i++)
            {
                pair<string,string> plot = scheduledPlots[i];
                string plotType = plot.first;
                TFile outputFile((outputFolder+"/"+plotType+".root").c_str(),"UPDATE");

                for (unsigned int c = 0; c < theChannels->size() ; c++)
                {

                    if (!outputFile.GetDirectory((*theChannels)[c].getTagC()))
                    {  channelDir = outputFile.mkdir((*theChannels)[c].getTagC());        }
                    else
                    {  channelDir = outputFile.GetDirectory((*theChannels)[c].getTagC()); }

                    channelDir->cd();

                    for (unsigned int r = 0 ; r < theRegions->size() ; r++)
                    {
                        if (!channelDir->GetDirectory((*theRegions)[r].getTagC()))
                        { regionDir = channelDir->mkdir((*theRegions)[r].getTagC());        }
                        else
                        { regionDir = channelDir->GetDirectory((*theRegions)[r].getTagC()); }

                        regionDir->cd();

                        for (unsigned int j = 0 ; j < thePlots.size() ; j++)
                        {
                            if (thePlots[j].getType() != plotType) continue;
                            if (thePlots[j].GetParameter("channel") != (*theChannels)[c].getTag()) continue;
                            if (thePlots[j].GetParameter("region")  !=  (*theRegions)[r].getTag()) continue;

                            string addPath = "";
                            // For 2D-histos, create a subfolder varX[vs]varY
                            if ((plotType == "2D") || (plotType == "2DProjectedTo1D") || (plotType == "3DProjectedTo2D"))
                            {
                                string varX = thePlots[j].GetParameter("variableX");
                                string varY = thePlots[j].GetParameter("variableY");

                                addPath += "/" + varX + "[vs]" + varY;
                                if (!regionDir->GetDirectory((varX+"[vs]"+varY).c_str()))
                                { varDir = regionDir->mkdir((varX+"[vs]"+varY).c_str()); varDir->cd(); }

                            }

                            thePlots[j].Write(outputFolder+"/"+plotType
                                    +"/"+(*theChannels)[c].getTag()
                                    +"/"+(*theRegions)[r].getTag()
                                    +addPath,
                                    infoText,options);
                        }
                    }
                }
                outputFile.Close();
            }
        };

        // ##########################
        // #   Options management   #
        // ##########################

        void SetGlobalFloatOption(string category, string field, float value)   { theGlobalOptions.SetGlobalFloatOption(category,field,value);  }
        void SetGlobalStringOption(string category, string field, string value) { theGlobalOptions.SetGlobalStringOption(category,field,value); }
        void SetGlobalBoolOption(string category, string field, bool value)     { theGlobalOptions.SetGlobalBoolOption(category,field,value);   }
        void SetGlobalIntOption(string category, string field, int value)       { theGlobalOptions.SetGlobalIntOption(category,field,value);    }

        private:

        OptionsScrewdriver theGlobalOptions;

        vector<Plot> thePlots;

        vector< pair<string,string> > scheduledPlots;
        vector< pair<string,string> > scheduledHistos;

    };

}

#endif
