#ifndef Plot1DStackFigurePerProcess_h
#define Plot1DStackFigurePerProcess_h

#include "interface/Common.h"

#include "interface/Plot.h"
#include "interface/Variable.h"
#include "interface/ProcessClass.h"
#include "interface/histos/Histo1D.h"
#include "interface/histos/Histo2D.h"
#include "interface/PlotDefaultStyles.h"
#include "interface/OptionsScrewdriver.h"

namespace theDoctor
{

    class Plot1DStackFigurePerProcess 
    {
      
     public:
     
      Plot1DStackFigurePerProcess();
      ~Plot1DStackFigurePerProcess();


      static void GetHistoDependencies(vector<pair<string,string> >& output, string options = "")
      {
      }

      static vector<Plot> Produce(vector<pair<Name,Map3DFigure> >* theFigurePerProcessMap,
                                  vector<ProcessClass>* theProcessClasses,
                                  vector<Region>* theRegions,
                                  vector<Channel>* theChannels,
                                  OptionsScrewdriver theGlobalOptions,
                                  string histoOptions)
      {
          vector<Plot> theOutput;
          
          for (unsigned int f = 0 ; f < theFigurePerProcessMap->size() ; f++)
          for (unsigned int c = 0 ; c < theChannels->size()  ; c++)
          {
              Channel  theChannel = (*theChannels)[c];
              
              Name          theFigureName  =  (*theFigurePerProcessMap)[f].first;
              Map3DFigure   theFigureMap   =  (*theFigurePerProcessMap)[f].second;
      
              theOutput.push_back(
                                    MakePlot(theFigureName,theFigureMap,
                                             theProcessClasses, theRegions, theChannel,
                                             theGlobalOptions)
                                 );
   
          }

          return theOutput;
      }

      static Plot MakePlot(Name        theFigureName,
                           Map3DFigure theFigureMap, 
                           vector<ProcessClass>* theProcessClasses,
                           vector<Region>* theRegions, 
                           Channel theChannel,
                           OptionsScrewdriver theGlobalOptions)
      {

         string plotName = string("t:1DStackFigurePerProcess|f:")+theFigureName.getTag()
                                                          +"|c:" +theChannel.getTag();

         Plot thePlot(plotName,"1DStackFigurePerProcess",theGlobalOptions,"");
         thePlot.SetParameter("figure",theFigureName.getTag());
         thePlot.SetParameter("channel",theChannel.getTag());
         thePlot.AddToInPlotInfo(theChannel.getLabel());
         
         string includeSignal = theGlobalOptions.GetGlobalStringOption("1DStackFigurePerProcess","includeSignal");
         float  factorSignal  = theGlobalOptions.GetGlobalFloatOption( "1DStackFigurePerProcess","factorSignal");
         string factorSignalStr = floatToString(factorSignal);
         
         string xlabel("");
         string ylabel(theFigureName.getLabel());

        vector<TH1F*>  pointersForLegend;
        vector<string> labelsForLegend;
        vector<string> optionsForLegend;

        // Create invididual per-process histo
        vector<TH1F*> perProcessHistos;
        for (unsigned int p = 0 ; p < theProcessClasses->size() ; p++)
        {
            TH1F* newHisto   = new TH1F("","",theRegions->size(), 0, theRegions->size());
            string nameHisto =  string("f:")+theFigureName.getTag()
                                     +"|p:"+(*theProcessClasses)[p].getTag()
                                     +"|c:"+theChannel.getTag()
                                     +"|t:1DFigurePerProcess";
            newHisto->SetName(nameHisto.c_str());
            perProcessHistos.push_back(newHisto);
        }

        // Create stack histo
        THStack* theStack   = new THStack("","");

        TH1F* histoSumBackground = new TH1F("","",theRegions->size(), 0, theRegions->size());
        string nameHisto    = string("f:")+theFigureName.getTag()
                                 +"|p:SumBackground"
                                 +"|c:"+theChannel.getTag()
                                 +"|t:1DFigurePerProcess";
        histoSumBackground->SetName(nameHisto.c_str());

        // Now loop on the histos
        for (unsigned int p = 0 ; p < theProcessClasses->size() ; p++)
        {
            // Get associated processClass
            ProcessClass* processClass = &((*theProcessClasses)[p]);

            if (processClass->getType() != "background") continue;

            // Get the histo
            TH1F* histoClone = perProcessHistos[p];

            // Change style of histo and add it to legend
            ApplyHistoStyle(&thePlot,histoClone,processClass->getColor(),theGlobalOptions,processClass->getOptions());
         
            pointersForLegend.push_back(histoClone);
            optionsForLegend.push_back(string("f"));
			labelsForLegend.push_back(processClass->getLabel());

            // Add histo to stack
            theStack->Add(histoClone);
            histoSumBackground->Add(histoClone);
        }

        // Apply axis style and plot the stack
        theStack->Draw("HIST");
        ApplyAxisStyle(&thePlot,theStack,xlabel,ylabel,theGlobalOptions);

		// Add signal if specified in the options of the plot type
        if ((includeSignal != "") && (includeSignal != "no") && (includeSignal != "false"))
        {
            for (unsigned int p = 0 ; p < theProcessClasses->size() ; p++)
            {
                // Get associated processClass
                ProcessClass* processClass = &((*theProcessClasses)[p]);
                if (processClass->getType() != "signal") continue;
                    
                TH1F* histoClone = perProcessHistos[p];
                ApplyHistoSignalStyle(&thePlot,histoClone,processClass->getColor(),theGlobalOptions,processClass->getOptions());
                histoClone->Scale(factorSignal);
                
                if (includeSignal == "stack") 
                    histoClone->Add(histoSumBackground);
               
                histoClone->Draw("hist same");
                
				// Add to legend
		        pointersForLegend.insert(pointersForLegend.begin(),histoClone);
		        optionsForLegend.insert(optionsForLegend.begin(),string("l"));
                if (factorSignal == 1.0)
        		    labelsForLegend.insert(labelsForLegend.begin(),processClass->getLabel());
                else
        		    labelsForLegend.insert(labelsForLegend.begin(),processClass->getLabel()+"(#times"+factorSignalStr+")");
            }
        }
        
        // Add stuff to legend in reverse order
        for (unsigned int leg_i = 0 ; leg_i < pointersForLegend.size() ; leg_i++)
        {
            thePlot.AddToLegend(pointersForLegend[pointersForLegend.size()-1-leg_i],
                                labelsForLegend[pointersForLegend.size()-1-leg_i].c_str(),
                                optionsForLegend[pointersForLegend.size()-1-leg_i].c_str());
        }

        return thePlot;
      }

     private:

      static void ApplyHistoStyle(Plot* thePlot, TH1F* theHisto, Color_t color, OptionsScrewdriver theGlobalOptions, string processClassOptions = "")
      {
         theHisto->SetFillColor(color);
         theHisto->SetLineColor(kBlack);
         theHisto->SetLineWidth(2);
      }
	  
	  static void ApplyHistoSignalStyle(Plot* thePlot, TH1F* theHisto, Color_t color, OptionsScrewdriver theGlobalOptions, string processClassOptions = "")
      {
         theHisto->SetFillColor(0);
         theHisto->SetLineWidth(3);
         theHisto->SetLineColor(color);
		 theHisto->SetLineStyle(9);
	  }

      static void ApplyAxisStyle(Plot* thePlot, THStack* theStack, string xlabel, string ylabel, OptionsScrewdriver theGlobalOptions, string varOptions = "")
      { 
         PlotDefaultStyles::ApplyDefaultAxisStyle(theStack->GetXaxis(),xlabel);
         PlotDefaultStyles::ApplyDefaultAxisStyle(theStack->GetYaxis(),ylabel);
         theStack->SetTitle("");
         if (OptionsScrewdriver::GetBoolOption(varOptions,"logY"))
         {
             thePlot->SetLogY();
             theStack->SetMaximum(theStack->GetMaximum() * 6.0);
         }
         else
            theStack->SetMaximum(theStack->GetMaximum() * 1.3);
      }

    };

}

#endif
