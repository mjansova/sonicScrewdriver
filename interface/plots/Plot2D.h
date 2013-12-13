#ifndef Plot2D_h
#define Plot2D_h

#include "interface/Common.h"

#include "interface/Plot.h"
#include "interface/Variable.h"
#include "interface/ProcessClass.h"
#include "interface/histos/Histo2D.h"
#include "interface/PlotDefaultStyles.h"
#include "interface/OptionsScrewdriver.h"

namespace theDoctor
{

    class Plot2D 
    {

        public:

            Plot2D();
            ~Plot2D();

            static void GetHistoDependencies(vector<pair<string,string> >& output)
            {
            }

            static vector<Plot> Produce(vector<Variable>* theVariables,
                                        vector<ProcessClass>* theProcessClasses,
                                        vector<Region>* theRegions,
                                        vector<Channel>* theChannels,
                                        HistoScrewdriver* theHistoScrewdriver,
                                        OptionsScrewdriver theGlobalOptions,
                                        string histoOptions)
            {
                vector<Plot> theOutput;
      
                vector<Histo2DEntries>* the2DHistosEntries = theHistoScrewdriver->Get2DHistosEntries();

                for (unsigned int h = 0 ; h < the2DHistosEntries->size()   ; h++)
                {
                    Histo2DEntries* theHisto        = &((*the2DHistosEntries)[h]); 
                    Variable*       theXVar         = theHisto->getVariableX();
                    Variable*       theYVar         = theHisto->getVariableY();
                    ProcessClass*   theProcessClass = theHisto->getProcessClass();
                    Region*         theRegion       = theHisto->getRegion();
                    Channel*        theChannel      = theHisto->getChannel();

                    theOutput.push_back(
                                          MakePlot(theXVar,theYVar,theProcessClass,theRegion,theChannel,theHisto,theGlobalOptions)
                                       );

                }

                return theOutput;
            }

            static Plot MakePlot(Variable*          theXVar,
                                 Variable*          theYVar,
                                 ProcessClass*      theProcessClass,
                                 Region*            theRegion, 
                                 Channel*           theChannel,
                                 Histo2DEntries*    theHisto,
                                 OptionsScrewdriver theGlobalOptions)
            {

                string plotName = string("t:2D|vX:")+theXVar->getTag()
                                            +"|vY:" +theYVar->getTag()
                                            +"|p:"  +theProcessClass->getTag()
                                            +"|r:"  +theRegion->getTag()
                                            +"|c:"  +theChannel->getTag();

                Plot thePlot(plotName,"2D",theGlobalOptions,"");
                thePlot.SetParameter("variableX",   theXVar->getTag());
                thePlot.SetParameter("variableY",   theYVar->getTag());
                thePlot.SetParameter("processClass",theProcessClass->getTag());
                thePlot.SetParameter("region",      theRegion->getTag());
                thePlot.SetParameter("channel",     theChannel->getTag());
          
                thePlot.AddToInPlotInfo(theChannel->getLabel());
                thePlot.AddToInPlotInfo(theRegion->getLabel());
                thePlot.AddToInPlotInfo(theProcessClass->getLabel());

                // Prepare the labels for x and y axis
                // xlabel = labelDeLaVariable (Unité)
                // ylabel = Normalized entries / largeurDeBin Unité

                string xlabel(theXVar->getLabel());
                string ylabel(theYVar->getLabel());
                string zlabel("Entries");

                // Add the unit
                if (theXVar->getUnit() != "") xlabel += string(" [") + theXVar->getUnit() + string("]");
                if (theYVar->getUnit() != "") ylabel += string(" [") + theYVar->getUnit() + string("]");

                TH2F* histoClone = theHisto->getClone();

                ApplyHistoStyle(&thePlot,histoClone,theProcessClass->getColor(),theGlobalOptions,theProcessClass->getOptions());
                ApplyAxisStyle(&thePlot,histoClone,xlabel,ylabel,theGlobalOptions,theXVar->getOptions(),theYVar->getOptions());

                thePlot.getCanvas()->SetRightMargin(0.1);
                histoClone->Draw("COLZ");
                thePlot.Update();
                TPaletteAxis *pal = (TPaletteAxis*) histoClone->GetListOfFunctions()->FindObject("palette");
                pal->SetX1NDC(0.901);
                pal->SetY1NDC(0.1);
                pal->SetX2NDC(0.93);
                pal->SetY2NDC(1.0-thePlot.getCanvas()->GetTopMargin());

                return thePlot;
            }


        private:

            static void ApplyHistoStyle(Plot* thePlot, TH2F* theHisto, Color_t color, OptionsScrewdriver theGlobalOptions, string processClassOptions = "")
            {
            }

            static void ApplyAxisStyle(Plot* thePlot, TH2F* theHisto, string xlabel, string ylabel, OptionsScrewdriver theGlobalOptions, string varXOptions = "", string varYOptions = "")
            {	
                PlotDefaultStyles::ApplyDefaultAxisStyle(theHisto->GetXaxis(),xlabel);
                PlotDefaultStyles::ApplyDefaultAxisStyle(theHisto->GetYaxis(),ylabel);
                //theHisto->SetTitle(title.c_str());
                theHisto->SetTitle("");
                theHisto->SetStats(0);
                thePlot->SetLogZ();
            }

    };

}

#endif
