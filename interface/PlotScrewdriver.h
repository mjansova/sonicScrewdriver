#ifndef PlotScrewdriver_h
#define PlotScrewdriver_h

#include "interface/Common.h"
#include "interface/OptionsScrewdriver.h"

#include "interface/Plot.h"
#include "interface/Variable.h"
#include "interface/ProcessClass.h"
#include "interface/Histo1DEntries.h"
#include "interface/Histo1DSumBackground.h"
#include "interface/Histo1DFigureOfMerit.h"
#include "interface/Histo2DEntries.h"

// Plot hammers
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

            Plot* AddPlot(string type, string name1, string name2, string name3, string name4, string name5, string name6, string options = "")
            { 
                string name;
                if ((type == "1DSuperpRenorm") || (type == "1DStack") || (type == "1DDataMCComparison") || (type == "1DFigureOfMerit"))
                    name = string("var:")+name1+"|region:"+name2+"|channel:"+name3+"|type:"+type;

                else if ((type == "2D") || (type== "2DProjectedTo1D"))
                    name = string("varX:")+name1+"|varY:"+name2+"|processClass:"+name3+"|region:"+name4+"|channel:"+name5+"|type:"+type;

                else if (type== "3DProjectedTo2D")
                    name = string("varX:")+name1+"|varY:"+name2+"|varZ:"+name3+"|processClass:"+name4+"|region:"+name5+"|channel:"+name6+"|type:"+type;

                else
                    name = name1+"_"+name2+"_"+name3+"_"+name4+"_"+name5;

                thePlots.push_back(Plot(name,type,options));
                return &(thePlots[thePlots.size()-1]); 
            };


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
                else if (plotType == "1DFigureOfMerit")    Plot1DFigureOfMerit   ::GetHistoDependencies(dependencies);
                else if (plotType == "1DDataMCComparison") Plot1DDataMCComparison::GetHistoDependencies(dependencies);
                else if (plotType == "2D")                 Plot2D                ::GetHistoDependencies(dependencies);
                else if (plotType == "2DProjectedTo1D")    Plot2DProjectedTo1D   ::GetHistoDependencies(dependencies);
                else if (plotType == "3DProjectedTo2D")    Plot3DProjectedTo2D   ::GetHistoDependencies(dependencies);
                else
                {
                    WARNING_MSG << "Plot-type '" << plotType << "' unknown." << endl;
                    return;
                }

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

                    else
                    if ((histoType == "1DFigureOfMerit") || (histoType == "1DDataMCRatio"))
                        for (unsigned int v = 0 ; v < theVariables->size() ; v++)
                        for (unsigned int r = 0 ; r < theRegions->size()   ; r++)
                        for (unsigned int c = 0 ; c < theChannels->size()  ; c++)
                        {
                            Variable* theVar     = &((*theVariables)[v]);
                            Region*   theRegion  = &((*theRegions)[r]);
                            Channel*  theChannel = &((*theChannels)[c]);

                            DEBUG_MSG << "v = " << theVar->getTag() << " ; r = " << theRegion->getTag() << " ; c = " << theChannel->getTag() << endl;
                                
                            if (histoType == "1DFigureOfMerit")
                            {
                                string varTag = OptionsScrewdriver::GetStringOption(histoOptions,"var");
                                if (varTag != theVar->getTag()) continue;
                            }

                            //if (histoType == "1DSumBackground") 
                            //    theHistoScrewdriver->Add1DHistoForPlots(Histo1DSumBackground(theVar,theProcessClasses,theRegion,theChannel,theHistoScrewdriver));
                            if (histoType == "1DFigureOfMerit") 
                                theHistoScrewdriver->Add1DHistoForPlots(Histo1DFigureOfMerit(theVar,theProcessClasses,theRegion,theChannel,theHistoScrewdriver,theGlobalOptions,histoOptions));
                            else if (histoType == "1DDataMCRatio") 
                                theHistoScrewdriver->Add1DHistoForPlots(Histo1DDataMCRatio(theVar,theProcessClasses,theRegion,theChannel,theHistoScrewdriver,theGlobalOptions,histoOptions));
                        }
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

                vector<Histo1DEntries>* the1DHistosEntries  = theHistoScrewdriver->Get1DHistosEntries();
                //vector<Histo2D>* the2DHistosEntries  = theHistoScrewdriver->Get2DHistosEntries();
                //vector<Histo3D>* the3DHistosEntries  = theHistoScrewdriver->Get3DHistosEntries();

                vector<Histo1D>*   the1DHistosForPlots = theHistoScrewdriver->Get1DHistosForPlots();
                //vector<Histo2D>* the2DHistosForPlots = theHistoScrewdriver->Get2DHistosForPlots();
                //vector<Histo3D>* the3DHistosForPlots = theHistoScrewdriver->Get3DHistosForPlots();

                // For each scheduled plot
                for (unsigned int i = 0 ; i < scheduledPlots.size() ; i++)
                {
                    pair<string,string> plot = scheduledPlots[i];
                    string plotType = plot.first;
                    string plotOptions = plot.second;

                    Plot*     thePlot    = AddPlot(plotType,"","","","","","",options);

                    if ((plotType == "1DStack") || (plotType == "1DSuperpRenorm"))
                        for (unsigned int v = 0 ; v < theVariables->size() ; v++)
                        {
                            for (unsigned int r = 0 ; r < theRegions->size()   ; r++)
                                for (unsigned int c = 0 ; c < theChannels->size()  ; c++)
                                {
                                    Variable* theVar     = &((*theVariables)[v]);
                                    Region*   theRegion  = &((*theRegions)[r]);
                                    Channel*  theChannel = &((*theChannels)[c]);
                                }
                        }


                    if (plotType == "1DFigureOfMerit")
                        for (unsigned int i = 0 ; i < the1DHistosForPlots->size() ; i++)
                        {
                            Histo1D* theHisto = &((*the1DHistosForPlots)[i]);
                            if ((theHisto->getHistoTypeTag()    == plotType) 
                                    && (theHisto->getHistoParameters() == plotOptions))
                            {
                                Plot1DFigureOfMerit::MakePlot(dynamic_cast<Histo1DFigureOfMerit*>(theHisto),thePlot,"","");
                            }

                        }
                }
                /*
                   if ((plotType == "1DSuperpRenorm") || (plotType == "1DStack") || (plotType == "1DDataMCComparison"))
                   for (unsigned int r = 0 ; r < theRegions->size()   ; r++)
                   for (unsigned int v = 0 ; v < theVariables->size() ; v++)
                   for (unsigned int c = 0 ; c < theChannels->size()  ; c++)
                   {
                   Variable* theVar     = &((*theVariables)[v]);
                   Region*   theRegion  = &((*theRegions)[r]);
                   Channel*  theChannel = &((*theChannels)[c]);

                   Plot*     thePlot    = AddPlot(plotType,theVar->getTag(),theRegion->getTag(),theChannel->getTag(),"","","",options);

                   if (plotType == "1DSuperpRenorm")        Plot1DSuperpRenorm::MakePlot(theVar,theProcessClasses,theRegion,theChannel,theHistoScrewdriver,thePlot,plotOptions,options);
                   else if (plotType == "1DStack")          Plot1DStack::MakePlot(theVar,theProcessClasses,theRegion,theChannel,theHistoScrewdriver,thePlot,plotOptions,options);
                   else if (plotType == "1DDataMCComparison") Plot1DDataMCComparison::MakePlot(theVar,theProcessClasses,theRegion,theChannel,theHistoScrewdriver,thePlot,plotOptions,options);
                   }

                   if (plotType == "2D")
                   {
                   vector<Histo2DEntries>* histo2DList = theHistoScrewdriver->Get2DHistoList();

                   for (unsigned int j = 0 ; j < histo2DList->size() ; j++)
                   {
                   Variable*     theVarX         = (*histo2DList)[j].getVariableX();
                   Variable*     theVarY         = (*histo2DList)[j].getVariableY();
                   ProcessClass* theProcessClass = (*histo2DList)[j].getProcessClass();
                   Region*       theRegion       = (*histo2DList)[j].getRegion();
                   Channel*      theChannel      = (*histo2DList)[j].getChannel();

                   Plot* thePlot = AddPlot(plotType,theVarX->getTag(),
                   theVarY->getTag(),
                   theProcessClass->getTag(),
                   theRegion->getTag(),
                   theChannel->getTag(),"",options);

                   Plot2D::MakePlot(theVarX,theVarY,theProcessClass,theRegion,theChannel,theHistoScrewdriver,thePlot,options);
                   }
                   }
                   if (plotType == "2DProjectedTo1D")
                   {
                   vector<Histo2DEntries>* histo2DList = theHistoScrewdriver->Get2DHistoList();
                   string varXTag = OptionsScrewdriver::getStringOption(plotOptions,"varX");
                   string varYTag = OptionsScrewdriver::getStringOption(plotOptions,"varY");
                   string labelY  = OptionsScrewdriver::getStringOption(plotOptions,"labelY");

                   for (unsigned int j = 0 ; j < histo2DList->size() ; j++)
                   {
                   Variable*     theVarX         = (*histo2DList)[j].getVariableX();
                   Variable*     theVarY         = (*histo2DList)[j].getVariableY();

                   if (theVarX->getTag() != varXTag) continue; 
                   if (theVarY->getTag() != varYTag) continue; 

                   ProcessClass* theProcessClass = (*histo2DList)[j].getProcessClass();
                   Region*       theRegion       = (*histo2DList)[j].getRegion();
                   Channel*      theChannel      = (*histo2DList)[j].getChannel();

                   Plot* thePlot = AddPlot(plotType,theVarX->getTag(),
                   labelY,
                   theProcessClass->getTag(),
                   theRegion->getTag(),
                   theChannel->getTag(),options);

                   Plot2DProjectedTo1D::MakePlot(theVarX,theVarY,theProcessClass,theRegion,theChannel,theHistoScrewdriver,thePlot,plotOptions,options);
                   }
                   }

                   if (plotType == "3DProjectedTo2D")
                   {
                   vector<Histo3DEntries>* histo3DList = theHistoScrewdriver->Get3DHistoList();
                   string varXTag = OptionsScrewdriver::getStringOption(plotOptions,"varX");
                string varYTag = OptionsScrewdriver::getStringOption(plotOptions,"varY");
                string varZTag = OptionsScrewdriver::getStringOption(plotOptions,"varZ");
                string labelZ  = OptionsScrewdriver::getStringOption(plotOptions,"labelZ");

                for (unsigned int j = 0 ; j < histo3DList->size() ; j++)
                {
                    Variable*     theVarX         = (*histo3DList)[j].getVariableX();
                    Variable*     theVarY         = (*histo3DList)[j].getVariableY();
                    Variable*     theVarZ         = (*histo3DList)[j].getVariableZ();

                    if (theVarX->getTag() != varXTag) continue; 
                    if (theVarY->getTag() != varYTag) continue; 
                    if (theVarZ->getTag() != varZTag) continue; 

                    ProcessClass* theProcessClass = (*histo3DList)[j].getProcessClass();
                    Region*       theRegion       = (*histo3DList)[j].getRegion();
                    Channel*      theChannel      = (*histo3DList)[j].getChannel();

                    Plot* thePlot = AddPlot(plotType,theVarX->getTag(),
                            theVarY->getTag(),
                            labelZ,
                            theProcessClass->getTag(),
                            theRegion->getTag(),
                            theChannel->getTag(),options);

                    Plot3DProjectedTo2D::MakePlot(theVarX,theVarY,theVarZ,theProcessClass,theRegion,theChannel,theHistoScrewdriver,thePlot,plotOptions,options);
                }
            }
            }
            */
    };

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
                        if (thePlots[j].infoFromCanvasName("channel") != (*theChannels)[c].getTag()) continue;
                        if (thePlots[j].infoFromCanvasName("region")  !=  (*theRegions)[r].getTag()) continue;

                        string addPath = "";
                        // For 2D-histos, create a subfolder varX[vs]varY
                        if ((plotType == "2D") || (plotType == "2DProjectedTo1D") || (plotType == "3DProjectedTo2D"))
                        {
                            string varX = thePlots[j].infoFromCanvasName("varX");
                            string varY = thePlots[j].infoFromCanvasName("varY");

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

    void SetOption(string category, string field, float value)  { theGlobalOptions.SetOption(category,field,value); }
    void SetOption(string category, string field, string value) { theGlobalOptions.SetOption(category,field,value); }
    void SetOption(string category, string field, bool value)   { theGlobalOptions.SetOption(category,field,value); }
    void SetOption(string category, string field, int value)    { theGlobalOptions.SetOption(category,field,value); }

    private:

          OptionsScrewdriver theGlobalOptions;

          vector<Plot> thePlots;

          vector< pair<string,string> > scheduledPlots;
          vector< pair<string,string> > scheduledHistos;

    };

}

#endif
