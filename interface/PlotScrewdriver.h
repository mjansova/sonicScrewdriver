#ifndef PlotScrewdriver_h
#define PlotScrewdriver_h

#include "interface/Common.h"

#include "interface/Plot.h"
#include "interface/Variable.h"
#include "interface/ProcessClass.h"
#include "interface/Histo1D.h"
#include "interface/Histo2D.h"

// Plot hammers
#include "interface/Plot1DSuperpRenorm.h"
#include "interface/Plot1DStack.h"
#include "interface/Plot1DCutSignificance.h"
#include "interface/PlotDataMCComparison.h"
#include "interface/Plot2D.h"
#include "interface/Plot2DProjectedTo1D.h"
#include "interface/Plot3DProjectedTo2D.h"
                    
namespace theDoctor
{

    class PlotScrewdriver 
    {
      
     public:
     
      PlotScrewdriver() { };
      ~PlotScrewdriver() { };

      Plot* AddPlot(string type, string name1, string name2, string name3, string name4, string name5, string name6, string options = "")
      { 
          string name;
          if ((type == "1DSuperpRenorm") || (type == "1DStack") || (type == "DataMCComparison") || (type == "1DCutSignificance"))
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

      void SchedulePlots(string plotType, string options = "")
      {
         if ((plotType != "1DSuperpRenorm")
         &&  (plotType != "1DStack")
         &&  (plotType != "1DCutSignificance")
         &&  (plotType != "DataMCComparison")
         &&  (plotType != "2D")
         &&  (plotType != "2DProjectedTo1D")
         &&  (plotType != "3DProjectedTo2D"))
         {
           WARNING_MSG << "Plot-type '" << plotType << "' unknown." << endl;
         }
         else
         {
           scheduledPlots.push_back(plotType);
           scheduledPlotsOptions.push_back(options);
         }
      };

      void MakePlots(vector<Variable>* theVariables,
                     vector<ProcessClass>* theProcessClasses,
                     vector<Region>* theRegions,
                     vector<Channel>* theChannels,
                     HistoScrewdriver* theHistoScrewdriver,
                     float theLumi,
                     string options = "")
      {
         for (unsigned int i = 0 ; i < scheduledPlots.size() ; i++)
         {
            string plotType = scheduledPlots[i];

            if ((plotType == "1DSuperpRenorm") || (plotType == "1DStack") || (plotType == "DataMCComparison") || (plotType == "1DCutSignificance"))
            for (unsigned int r = 0 ; r < theRegions->size()   ; r++)
            {
                for (unsigned int v = 0 ; v < theVariables->size() ; v++)
                for (unsigned int c = 0 ; c < theChannels->size()  ; c++)
                {
                    Variable* theVar     = &((*theVariables)[v]);
                    Region*   theRegion  = &((*theRegions)[r]);
                    Channel*  theChannel = &((*theChannels)[c]);

                    if (plotType == "1DCutSignificance")
                    {
                    	string plotTypeOptions;
                        plotTypeOptions = scheduledPlotsOptions[i];
                        string varTag = OptionsScrewdriver::getStringOption(plotTypeOptions,"var");
                        if (varTag != theVar->getTag()) continue;
                    }

                    Plot*     thePlot    = AddPlot(plotType,theVar->getTag(),theRegion->getTag(),theChannel->getTag(),"","","",options);

                         if (plotType == "1DSuperpRenorm")       Plot1DSuperpRenorm::MakePlot(theVar,theProcessClasses,theRegion,theChannel,theHistoScrewdriver,thePlot,scheduledPlotsOptions[i],options);
                    else if (plotType == "1DStack")                     Plot1DStack::MakePlot(theVar,theProcessClasses,theRegion,theChannel,theHistoScrewdriver,thePlot,scheduledPlotsOptions[i],options);
                    else if (plotType == "DataMCComparison")   PlotDataMCComparison::MakePlot(theVar,theProcessClasses,theRegion,theChannel,theHistoScrewdriver,thePlot,scheduledPlotsOptions[i],options);
                    else if (plotType == "1DCutSignificance") Plot1DCutSignificance::MakePlot(theVar,theProcessClasses,theRegion,theChannel,theHistoScrewdriver,thePlot,scheduledPlotsOptions[i],options);
                }
            }	

            if (plotType == "2D")
            {
                vector<Histo2D>* histo2DList = theHistoScrewdriver->Get2DHistoList();

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
                vector<Histo2D>* histo2DList = theHistoScrewdriver->Get2DHistoList();
                string plotTypeOptions = scheduledPlotsOptions[i];
                string varXTag = OptionsScrewdriver::getStringOption(plotTypeOptions,"varX");
                string varYTag = OptionsScrewdriver::getStringOption(plotTypeOptions,"varY");
                string labelY  = OptionsScrewdriver::getStringOption(plotTypeOptions,"labelY");
 
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

                    Plot2DProjectedTo1D::MakePlot(theVarX,theVarY,theProcessClass,theRegion,theChannel,theHistoScrewdriver,thePlot,plotTypeOptions,options);
                }
            }

            if (plotType == "3DProjectedTo2D")
            {
                vector<Histo3D>* histo3DList = theHistoScrewdriver->Get3DHistoList();
                string plotTypeOptions = scheduledPlotsOptions[i];
                string varXTag = OptionsScrewdriver::getStringOption(plotTypeOptions,"varX");
                string varYTag = OptionsScrewdriver::getStringOption(plotTypeOptions,"varY");
                string varZTag = OptionsScrewdriver::getStringOption(plotTypeOptions,"varZ");
                string labelZ  = OptionsScrewdriver::getStringOption(plotTypeOptions,"labelZ");
 
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

                    Plot3DProjectedTo2D::MakePlot(theVarX,theVarY,theVarZ,theProcessClass,theRegion,theChannel,theHistoScrewdriver,thePlot,plotTypeOptions,options);
                }
            }

         }
      };

      void WritePlots(vector<Channel>* theChannels, vector<Region>* theRegions, string outputFolder, string infoText, string options = "")
      {

        TDirectory* channelDir = 0;
        TDirectory* regionDir  = 0;
        TDirectory* varDir     = 0; 

		int ret;
        ret = system(("rm -f "+outputFolder+"/1DStack.root").c_str());
        ret = system(("rm -f "+outputFolder+"/1DSuperpRenorm.root").c_str());
        ret = system(("rm -f "+outputFolder+"/1DCutSignificance.root").c_str());
        ret = system(("rm -f "+outputFolder+"/DataMCComparison.root").c_str());
        ret = system(("rm -f "+outputFolder+"/2D.root").c_str());
        ret = system(("rm -f "+outputFolder+"/2DProjectedTo1D.root").c_str());
        ret = system(("rm -f "+outputFolder+"/3DProjectedTo2D.root").c_str());
		// Fix "ret not used" warning
		ret = ret + 1;
		
        for (unsigned int i = 0 ; i < scheduledPlots.size() ; i++)
        {
            string plotType = scheduledPlots[i];
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

     private:

      vector<Plot> thePlots;

      vector<string> scheduledPlots;
      vector<string> scheduledPlotsOptions;

    };

}

#endif
