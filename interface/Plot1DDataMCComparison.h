#ifndef Plot1DDataMCComparison_h
#define Plot1DDataMCComparison_h

#include "interface/Common.h"

#include "interface/Plot.h"
#include "interface/Variable.h"
#include "interface/ProcessClass.h"
#include "interface/Histo1D.h"
#include "interface/Histo1DDataMCRatio.h"
#include "interface/Histo2D.h"
#include "interface/PlotDefaultStyles.h"
#include "interface/OptionsScrewdriver.h"

namespace theDoctor
{

    class Plot1DDataMCComparison 
    {
      
     public:
     
      Plot1DDataMCComparison();
      ~Plot1DDataMCComparison();
     
      static void GetHistoDependencies(vector<pair<string,string> >& output, string options = "")
      {
          Histo1DDataMCRatio::GetHistoDependencies(output); 
          output.push_back(pair<string,string>("1DDataMCRatio",options));
          output.push_back(pair<string,string>("1DSumData",options));
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

          // Browse the (var x reg x chan) space
          for (unsigned int v = 0 ; v < theVariables->size() ; v++)
          for (unsigned int r = 0 ; r < theRegions->size()   ; r++)
          for (unsigned int c = 0 ; c < theChannels->size()  ; c++)
          {
          
              vector<Histo1DEntries*> theBackgrounds;
              vector<Histo1DEntries*> theSignals;

              Variable* theVar     = &((*theVariables)[v]);
              Region*   theRegion  = &((*theRegions)[r]);
              Channel*  theChannel = &((*theChannels)[c]);

              // Now loop on the histos
              for (unsigned int i = 0 ; i < theProcessClasses->size() ; i++)
              {

                  ProcessClass thisProcess = (*theProcessClasses)[i];

                  // If it it, we add it to the relevant backgrounds
                  Histo1DEntries* thisHisto = theHistoScrewdriver->get1DHistoEntriesPointer(theVar->getTag(),
                                                                                            thisProcess.getTag(),
                                                                                            theRegion->getTag(),
                                                                                            theChannel->getTag());

                  if (thisProcess.getType() == "background") theBackgrounds.push_back(thisHisto);
                  if (thisProcess.getType() == "signal")     theSignals.push_back(thisHisto);
              }

              Histo1D* theSumBackground = theHistoScrewdriver->get1DHistoForPlotPointer("1DSumBackground",
                                                                                        theVar->getTag(),
                                                                                        theRegion->getTag(),
                                                                                        theChannel->getTag(),
                                                                                        "");
              Histo1D* theSumData       = theHistoScrewdriver->get1DHistoForPlotPointer("1DSumData",
                                                                                        theVar->getTag(),
                                                                                        theRegion->getTag(),
                                                                                        theChannel->getTag(),
                                                                                        "");
              Histo1D* theDataMCRatio   = theHistoScrewdriver->get1DHistoForPlotPointer("1DDataMCRatio",
                                                                                        theVar->getTag(),
                                                                                        theRegion->getTag(),
                                                                                        theChannel->getTag(),
                                                                                        "");
              theOutput.push_back(
                                  MakePlot(theVar,theRegion,theChannel,
                                           theBackgrounds,theSignals,
                                           theSumBackground,theSumData,theDataMCRatio,
                                           theGlobalOptions)
                                 );

          }

          return theOutput;
      }

      static Plot MakePlot(Variable* theVar, 
                           Region* theRegion, 
                           Channel* theChannel,
                           vector<Histo1DEntries*> theBackgrounds,
                           vector<Histo1DEntries*> theSignals,
                           Histo1D* theSumBackground,
                           Histo1D* theSumData,
                           Histo1D* theDataMCRatio,
                           OptionsScrewdriver theGlobalOptions)
      {

          string plotName = string("t:1DDataMCComparison|v:")+theVar->getTag()
                                                      +"|r:"+theRegion->getTag()
                                                      +"|c:"+theChannel->getTag();

          Plot thePlot(plotName,"1DDataMCComparison",theGlobalOptions,"");
          thePlot.SetParameter("variable",theVar->getTag());
          thePlot.SetParameter("region",theRegion->getTag());
          thePlot.SetParameter("channel",theChannel->getTag());
         
          thePlot.AddToInPlotInfo(theChannel->getLabel());
          thePlot.AddToInPlotInfo(theRegion->getLabel());

          string includeSignal = theGlobalOptions.GetGlobalStringOption("DataMCComparison","includeSignal");
          float  factorSignal  = theGlobalOptions.GetGlobalFloatOption( "DataMCComparison","factorSignal");
          string factorSignalStr = floatToString(factorSignal);

          // Prepare the labels for x and y axis
          // xlabel = labelDeLaVariable (Unité)
          // ylabel = Normalized entries / largeurDeBin Unité

          string xlabel(theVar->getLabel());
          string ylabel("Entries / ");
          ylabel += floatToString(theVar->getBinWidth());

          // Add the unit
          if (theVar->getUnit() != "")
          {
              xlabel += string(" [") + theVar->getUnit() + string("]");
              ylabel += string(" ") + theVar->getUnit();
          }

          vector<TH1F*>  pointersForLegend;
          vector<string> labelsForLegend;
          vector<string> optionsForLegend;

          // #################################
          // #####                       #####
          // ###       SM / Background     ###
          // #####                       #####
          // #################################

          // Create stack histo + another histo for the error plotting for the background sum
          TPad* theStackPad = thePlot.AddPad(0,0,1,0.8);
          theStackPad->SetTopMargin(0.0);

          THStack* stackBackground = new THStack("","");

          // Now loop on the histos
          for (int i = theBackgrounds.size()-1 ; i >= 0 ; i--)
          {
              // Get associated processClass
              ProcessClass* processClass = theBackgrounds[i]->getProcessClass();

              // Get the histo
              TH1F* histoClone = theBackgrounds[i]->getClone();

              // Change style of histo and add it to legend
              ApplyHistoStyle(&thePlot,histoClone,processClass->getColor(),theGlobalOptions,processClass->getOptions());

              pointersForLegend.push_back(histoClone);
              optionsForLegend.push_back(string("f"));
              labelsForLegend.push_back(processClass->getLabel());

              // Add histo to stack
              stackBackground->Add(histoClone);
          }

          // Apply axis style and plot the stack
          stackBackground->Draw("HIST");
          ApplyAxisStyle(&thePlot,stackBackground,xlabel,ylabel,theGlobalOptions,theVar->getOptions());

          // #################################
          // #####                       #####
          // ###          Signal           ###
          // #####                       #####
          // #################################

		  // Add signal if specified in the options of the plot type
          if ((includeSignal != "") && (includeSignal != "no") && (includeSignal != "false"))
          {
            
              TH1F* histoSumBackground = theSumBackground->getClone();
              
              for (unsigned int i = 0 ; i < theSignals.size() ; i++)
              {
                // Get associated processClass
                ProcessClass* processClass = theSignals[i]->getProcessClass();
                    
                TH1F* histoClone = theSignals[i]->getClone();
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
        

          // #################################
          // #####                       #####
          // ###           Data            ###
          // #####                       #####
          // #################################

          // Get the histo for data
          TH1F* dataHisto = theSumData->getClone();
          
          // Add it to the legend
          pointersForLegend.push_back(dataHisto);
          labelsForLegend.push_back("data");
          optionsForLegend.push_back("pl");

          // Apply style to data and draw it
          ApplyDataStyle(&thePlot,dataHisto,theGlobalOptions);
          dataHisto->Draw("SAME E");

          // #################################
          // #####                       #####
          // ###          Legend           ###
          // #####                       #####
          // #################################

          // Add stuff to legend in reverse order
          for (unsigned int leg_i = 0 ; leg_i < pointersForLegend.size() ; leg_i++)
          {
              thePlot.AddToLegend(pointersForLegend[pointersForLegend.size()-1-leg_i],
                                  labelsForLegend[pointersForLegend.size()-1-leg_i].c_str(),
                                  optionsForLegend[pointersForLegend.size()-1-leg_i].c_str());
          }

          // #################################
          // #####                       #####
          // ###      Ratio data/SM        ###
          // #####                       #####
          // #################################   

          // Compute and draw the ratio

          TPad* theRatioPad = thePlot.AddPad(0,0.8,1,1);
          theRatioPad->SetBottomMargin(0.07);
          theRatioPad->SetTopMargin(0.3);

          TH1F* ratio = theDataMCRatio->getClone();

          TF1* unity = new TF1("unity","1",-10000,10000);
          unity->SetLineColor(kBlack);
          unity->SetLineStyle(1);
          unity->SetLineWidth(1);

          ApplyRatioStyle(&thePlot,ratio,theGlobalOptions);
          ApplyRatioAxisStyle(&thePlot,ratio,theGlobalOptions,theVar->getOptions());

          ratio->Draw("E");

          // Add signal if specified in the options of the plotType
          if ((includeSignal == "stack") || (includeSignal == "superimpose"))
          {
              TH1F* histoSumBackground = theSumBackground->getClone();
              for (unsigned int i = 0 ; i < theSignals.size() ; i++)
              {
                  // Get associated processClass
                  ProcessClass* processClass = theSignals[i]->getProcessClass();
                    
                  TH1F* histoClone = theSignals[i]->getClone();
                  ApplyHistoSignalStyle(&thePlot,histoClone,processClass->getColor(),theGlobalOptions,processClass->getOptions());
                  histoClone->Scale(factorSignal);
                  histoClone->Add(histoSumBackground);		
                  histoClone->Divide(histoSumBackground);
                  histoClone->Draw("hist same");
              }
          }

          // Draw unity
          unity->Draw("SAME");

          thePlot.SetActive();

          return thePlot;
      }

     private:

      static void ApplyHistoStyle(Plot* thePlot, TH1F* theHisto, Color_t color, OptionsScrewdriver generalOptions, string processClassOptions = "")
      {
          theHisto->SetFillColor(color);
          theHisto->SetLineColor(kBlack);
          theHisto->SetLineWidth(2);
      }

      static void ApplyHistoSignalStyle(Plot* thePlot, TH1F* theHisto, Color_t color, OptionsScrewdriver generalOptions, string processClassOptions = "")
      {
          theHisto->SetFillColor(0);
          theHisto->SetLineWidth(3);
          theHisto->SetLineColor(color);
          theHisto->SetLineStyle(9);
      }

      static void ApplyErrorStyle(Plot* thePlot, TH1F* theError, OptionsScrewdriver generalOptions)
      {
          theError->SetMarkerSize(0);
          theError->SetLineWidth(2);
          theError->SetFillStyle(0);
      }

      static void ApplyDataStyle(Plot* thePlot, TH1F* theData, OptionsScrewdriver generalOptions)
      {
          theData->SetMarkerStyle(8);
          theData->SetMarkerSize(1);
          theData->SetLineWidth(1);
          theData->SetLineColor(kBlack);
          theData->SetFillStyle(0);
      }

      static void ApplyAxisStyle(Plot* thePlot, THStack* theStack, string xlabel, string ylabel, OptionsScrewdriver generalOptions, string varOptions = "")
      { 
          theStack->SetMaximum(theStack->GetMaximum() * 1.3);
          PlotDefaultStyles::ApplyDefaultAxisStyle(theStack->GetXaxis(),xlabel);
          PlotDefaultStyles::ApplyDefaultAxisStyle(theStack->GetYaxis(),ylabel);
          theStack->SetTitle("");
          if (OptionsScrewdriver::GetBoolOption(varOptions,"logY")) thePlot->SetLogY();
      }

      static void ApplyRatioStyle(Plot* thePlot, TH1F* theRatio, OptionsScrewdriver generalOptions)
      {
          theRatio->SetMarkerStyle(8);
          theRatio->SetMarkerSize(1);
          theRatio->SetLineWidth(1);
          theRatio->SetLineColor(kBlack);
          theRatio->SetFillStyle(0);
      }

      static void ApplyRatioAxisStyle(Plot* thePlot, TH1F* theRatio, OptionsScrewdriver generalOptions, string varOptions = "")
      { 
          // Y axis
          PlotDefaultStyles::ApplyDefaultAxisStyle(theRatio->GetYaxis(),string("data/SM"));
          theRatio->GetYaxis()->CenterTitle();
          theRatio->GetYaxis()->SetTickLength(0.015);
          theRatio->GetYaxis()->SetTitleSize(0.17);
          theRatio->GetYaxis()->SetTitleOffset(0.25);
          theRatio->SetMaximum(1.5);
          theRatio->SetMinimum(0.5);
          theRatio->GetYaxis()->SetNdivisions(4);

          // X axis
          PlotDefaultStyles::ApplyDefaultAxisStyle(theRatio->GetXaxis(),string(""));
          theRatio->GetXaxis()->SetLabelSize(0.0);
          theRatio->GetXaxis()->SetTickLength(0.1);

          // Misc stuff
          theRatio->SetTitle("");
          theRatio->SetStats(0);

      }
    };

}
#endif
