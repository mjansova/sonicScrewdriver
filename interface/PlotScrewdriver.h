#ifndef PlotScrewdriver_h
#define PlotScrewdriver_h

#include "interface/Common.h"
#include "interface/OptionsScrewdriver.h"

#include "interface/Plot.h"
#include "interface/Variable.h"
#include "interface/ProcessClass.h"

// Histograms producers
#include "interface/histos/Histo1DEntries.h"
#include "interface/histos/Histo1DSumBackground.h"
#include "interface/histos/Histo1DSumData.h"
#include "interface/histos/Histo1DFigureOfMerit.h"
#include "interface/histos/Histo2DEntries.h"
#include "interface/histos/Histo2DFigureOfMeritForVarXBeingSignalParameter.h"
#include "interface/histos/Histo3DFigureOfMeritForVarXYBeingSignalParameter.h"
#include "interface/histos/Histo1DFrom2DProjection.h"
#include "interface/histos/Histo2DFrom3DProjection.h"
#include "interface/histos/Histo2DSumBackground.h"

// Plot producers
#include "interface/plots/Plot1DSuperimposed.h"
#include "interface/plots/Plot1DStack.h"
#include "interface/plots/Plot1DFigureOfMerit.h"
#include "interface/plots/Plot1DDataMCComparison.h"
#include "interface/plots/Plot2D.h"
#include "interface/plots/Plot2DSuperimposed.h"
#include "interface/plots/Plot1DFrom2DProjection.h"
#include "interface/plots/Plot2DFrom3DProjection.h"
#include "interface/plots/Plot1DDataMCComparisonFigure.h"
#include "interface/plots/Plot1DFigure.h"

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

                 if (plotType == "1DSuperimposed")     Plot1DSuperimposed    ::GetHistoDependencies(dependencies);
            else if (plotType == "1DStack")            Plot1DStack           ::GetHistoDependencies(dependencies);
            else if (plotType == "1DFigureOfMerit")    Plot1DFigureOfMerit   ::GetHistoDependencies(dependencies,options);
            else if (plotType == "1DDataMCComparison") Plot1DDataMCComparison::GetHistoDependencies(dependencies);
            else if (plotType == "2D")                 Plot2D                ::GetHistoDependencies(dependencies);
            else if (plotType == "2DSuperimposed")     Plot2DSuperimposed    ::GetHistoDependencies(dependencies);
            else if (plotType == "1DFrom2DProjection") Plot1DFrom2DProjection::GetHistoDependencies(dependencies,options);
            else if (plotType == "2DFrom3DProjection") Plot2DFrom3DProjection::GetHistoDependencies(dependencies,options);
            else if (plotType == "1DDataMCComparisonFigure")   Plot1DDataMCComparisonFigure::GetHistoDependencies(dependencies,options);
            else if (plotType == "1DFigure")                   Plot1DFigure                ::GetHistoDependencies(dependencies,options);

            else { WARNING_MSG << "Plot-type '" << plotType << "' unknown." << endl; return; }

            // Schedule histo needed for plot
            for (unsigned int i = 0 ; i < dependencies.size() ; i++)
                ScheduleHisto(dependencies[i].first,dependencies[i].second);

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
            for (unsigned int i = 0 ; i < scheduledHistos.size() ; i++)
            {
                pair<string,string> histo = scheduledHistos[i];
                string histoType = histo.first;
                string histoOptions = histo.second;

                     if (histoType == "1DSumBackground")
                    Histo1DSumBackground::Produce(theVariables,theProcessClasses,theRegions,theChannels,theHistoScrewdriver,theGlobalOptions,histoOptions);
                else if (histoType == "1DSumData")
                    Histo1DSumData      ::Produce(theVariables,theProcessClasses,theRegions,theChannels,theHistoScrewdriver,theGlobalOptions,histoOptions);
                else if (histoType == "1DDataMCRatio")
                    Histo1DDataMCRatio  ::Produce(theVariables,theProcessClasses,theRegions,theChannels,theHistoScrewdriver,theGlobalOptions,histoOptions);
                else if (histoType == "1DFigureOfMerit")
                    Histo1DFigureOfMerit::Produce(theVariables,theProcessClasses,theRegions,theChannels,theHistoScrewdriver,theGlobalOptions,histoOptions);
                else if (histoType == "1DFrom2DProjection")
                    Histo1DFrom2DProjection::Produce(theVariables,theProcessClasses,theRegions,theChannels,theHistoScrewdriver,theGlobalOptions,histoOptions);
                else if (histoType == "2DSumBackground")
                    Histo2DSumBackground::Produce(theVariables,theProcessClasses,theRegions,theChannels,theHistoScrewdriver,theGlobalOptions,histoOptions);
                else if (histoType == "2DFigureOfMeritForVarXBeingSignalParameter")
                    Histo2DFigureOfMeritForVarXBeingSignalParameter::Produce(theVariables,theProcessClasses,theRegions,theChannels,theHistoScrewdriver,theGlobalOptions,histoOptions);
                else if (histoType == "2DFrom3DProjection")
                    Histo2DFrom3DProjection::Produce(theVariables,theProcessClasses,theRegions,theChannels,theHistoScrewdriver,theGlobalOptions,histoOptions);
                else if (histoType == "3DFigureOfMeritForVarXYBeingSignalParameter")
                    Histo3DFigureOfMeritForVarXYBeingSignalParameter::Produce(theVariables,theProcessClasses,theRegions,theChannels,theHistoScrewdriver,theGlobalOptions,histoOptions);
            }
        }

        void MakePlots(vector<Variable>* theVariables,
                vector<ProcessClass>* theProcessClasses,
                vector<Region>* theRegions,
                vector<Channel>* theChannels,
                vector<pair<Name,Map3DFigure> >* theFiguresPerProcessMap,
                vector<pair<Name,Map2DFigure> >* theFiguresMap,
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

                //DEBUG_MSG << "Producing plot type " << plotType << endl;

                vector<Plot> inputFromProducer;
                if (plotType == "1DStack")
                    inputFromProducer =
                        Plot1DStack           ::Produce(theVariables, theProcessClasses, theRegions, theChannels, theHistoScrewdriver, theGlobalOptions, plotOptions);
                else if (plotType == "1DSuperimposed")
                    inputFromProducer =
                        Plot1DSuperimposed    ::Produce(theVariables, theProcessClasses, theRegions, theChannels, theHistoScrewdriver, theGlobalOptions, plotOptions);
                else if (plotType == "1DFigureOfMerit")
                    inputFromProducer =
                        Plot1DFigureOfMerit   ::Produce(theVariables, theProcessClasses, theRegions, theChannels, theHistoScrewdriver, theGlobalOptions, plotOptions);
                else if (plotType == "1DDataMCComparison")
                    inputFromProducer =
                        Plot1DDataMCComparison::Produce(theVariables, theProcessClasses, theRegions, theChannels, theHistoScrewdriver, theGlobalOptions, plotOptions);
                else if (plotType == "2D")
                    inputFromProducer =
                        Plot2D                ::Produce(theVariables, theProcessClasses, theRegions, theChannels, theHistoScrewdriver, theGlobalOptions, plotOptions);
                else if (plotType == "2DSuperimposed")
                    inputFromProducer =
                        Plot2DSuperimposed    ::Produce(theVariables, theProcessClasses, theRegions, theChannels, theHistoScrewdriver, theGlobalOptions, plotOptions);
                else if (plotType == "1DFrom2DProjection")
                    inputFromProducer =
                        Plot1DFrom2DProjection::Produce(theVariables, theProcessClasses, theRegions, theChannels, theHistoScrewdriver, theGlobalOptions, plotOptions);
                else if (plotType == "2DFrom3DProjection")
                    inputFromProducer =
                        Plot2DFrom3DProjection::Produce(theVariables, theProcessClasses, theRegions, theChannels, theHistoScrewdriver, theGlobalOptions, plotOptions);
                else if (plotType == "1DDataMCComparisonFigure")
                    inputFromProducer =
                        Plot1DDataMCComparisonFigure::Produce(theFiguresPerProcessMap, theProcessClasses, theRegions, theChannels, theGlobalOptions, plotOptions);
                else if (plotType == "1DFigure")
                    inputFromProducer =
                        Plot1DFigure               ::Produce(theFiguresMap,                               theRegions, theChannels, theGlobalOptions, plotOptions);

                for (unsigned int j = 0 ; j < inputFromProducer.size() ; j++)
                {
                    thePlots.push_back(inputFromProducer[j]);
                }
            }

        }

        void WritePlots(vector<Channel>* theChannels, vector<Region>* theRegions, string outputFolder)
        {
            system(string("mkdir -p "+outputFolder).c_str());

            TDirectory* channelDir = 0;
            TDirectory* regionDir  = 0;
            TDirectory* varDir     = 0;

            int ret;
            ret = system(("rm -f "+outputFolder+"/1DStack.root").c_str());
            ret = system(("rm -f "+outputFolder+"/1DSuperimposed.root").c_str());
            ret = system(("rm -f "+outputFolder+"/1DFigureOfMerit.root").c_str());
            ret = system(("rm -f "+outputFolder+"/1DDataMCComparison.root").c_str());
            ret = system(("rm -f "+outputFolder+"/2D.root").c_str());
            ret = system(("rm -f "+outputFolder+"/2DSuperimposed.root").c_str());
            ret = system(("rm -f "+outputFolder+"/1DFrom2DProjection.root").c_str());
            ret = system(("rm -f "+outputFolder+"/2DFrom3DProjection.root").c_str());
            ret = system(("rm -f "+outputFolder+"/1DDataMCComparisonFigure.root").c_str());
            ret = system(("rm -f "+outputFolder+"/1DFigure.root").c_str());
            // FIXME stupid fix for ret not being used
            ret = ret + 1;

            for (unsigned int i = 0 ; i < scheduledPlots.size() ; i++)
            {
                pair<string,string> plot = scheduledPlots[i];
                string plotType       = plot.first;
                string plotParameters = plot.second;
                TFile outputFile((outputFolder+"/"+plotType+".root").c_str(),"UPDATE");

                for (unsigned int c = 0; c < theChannels->size() ; c++)
                {

                    if (!outputFile.GetDirectory((*theChannels)[c].getTagC()))
                    {  channelDir = outputFile.mkdir((*theChannels)[c].getTagC());        }
                    else
                    {  channelDir = outputFile.GetDirectory((*theChannels)[c].getTagC()); }

                    channelDir->cd();

                    if ((plotType != "1DDataMCComparisonFigure") && (plotType != "1DFigure"))
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
                            if ((plotType == "2D") || (plotType == "2DSuperimposed"))
                            {
                                string varX = thePlots[j].GetParameter("variableX");
                                string varY = thePlots[j].GetParameter("variableY");

                                addPath += "/" + varX + "[vs]" + varY;
                                if (!regionDir->GetDirectory((varX+"[vs]"+varY).c_str()))
                                { varDir = regionDir->mkdir((varX+"[vs]"+varY).c_str()); varDir->cd(); }
                            }
                            else if (plotType == "2DFrom3DProjection")
                            {
                                string varX = thePlots[j].GetParameter("variableX");
                                string varY = thePlots[j].GetParameter("variableY");
                                string tagZ = thePlots[j].GetParameter("tagZ");
                                addPath += "/" + varX + "[vs]" + varY + "__" +tagZ;
                                if (!regionDir->GetDirectory((varX+"[vs]"+varY+"__"+tagZ).c_str()))
                                { varDir = regionDir->mkdir((varX+"[vs]"+varY+"__"+tagZ).c_str()); varDir->cd(); }
                            }


                            thePlots[j].Write(outputFolder,
                                    plotType+"/"+(*theChannels)[c].getTag()+"/"+(*theRegions)[r].getTag()+addPath,
                                    theGlobalOptions);
                        }
                    }

                    if ((plotType == "1DDataMCComparisonFigure") || (plotType == "1DFigure"))
                    for (unsigned int j = 0 ; j < thePlots.size() ; j++)
                    {
                        if (thePlots[j].getType() != plotType) continue;
                        if (thePlots[j].GetParameter("channel") != (*theChannels)[c].getTag()) continue;

                        thePlots[j].Write(outputFolder,
                                plotType+"/"+(*theChannels)[c].getTag(),
                                theGlobalOptions);
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

        OptionsScrewdriver GetGlobalOptions() { return theGlobalOptions; }

        private:

        OptionsScrewdriver theGlobalOptions;

        vector<Plot> thePlots;

        vector< pair<string,string> > scheduledPlots;
        vector< pair<string,string> > scheduledHistos;

    };

}

#endif
