#ifndef Plot1DDataMCComparisonFigure_h
#define Plot1DDataMCComparisonFigure_h

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

    class Plot1DDataMCComparisonFigure 
    {
      
     public:
     
      Plot1DDataMCComparisonFigure();
      ~Plot1DDataMCComparisonFigure();


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

      static Plot MakePlot(Name                  theFigureName,
                           Map3DFigure           theFigureMap, 
                           vector<ProcessClass>* theProcessClasses,
                           vector<Region>*       theRegions, 
                           Channel               theChannel,
                           OptionsScrewdriver    theGlobalOptions)
      {

         string plotName = string("t:1DDataMCComparisonFigure|f:")+theFigureName.getTag()
                                                          +"|c:" +theChannel.getTag();

         Plot thePlot(plotName,"1DDataMCComparisonFigure",theGlobalOptions,"");

         thePlot.SetParameter("figure",theFigureName.getTag());
         thePlot.SetParameter("channel",theChannel.getTag());
         thePlot.AddToInPlotInfo(theChannel.getLabel());
         //thePlot.getCanvas()->SetBottomMargin(0.15);
         //thePlot.getCanvas()->SetRightMargin(0.15);
         
         string includeSignal = theGlobalOptions.GetGlobalStringOption("1DDataMCComparisonFigure","includeSignal");
         float  factorSignal  = theGlobalOptions.GetGlobalFloatOption( "1DDataMCComparisonFigure","factorSignal");
         string factorSignalStr = floatToString(factorSignal);
         
         string xlabel("");
         string ylabel(theFigureName.getLabel());

        vector<TH1F*>  pointersForLegend;
        vector<string> labelsForLegend;
        vector<string> optionsForLegend;

        // ##########################################
        // ## Create invididual per-process histos ##
        // ##########################################
        
        vector<TH1F*> perProcessHistos;
        for (unsigned int p = 0 ; p < theProcessClasses->size() ; p++)
        {
            ProcessClass* processClass = &((*theProcessClasses)[p]);
            TH1F* newHisto   = new TH1F("","",theRegions->size(), 0, theRegions->size());
            string nameHisto =  string("f:")+theFigureName.getTag()
                                     +"|p:"+processClass->getTag()
                                     +"|c:"+theChannel.getTag()
                                     +"|t:1DFigurePerProcess";
            newHisto->SetName(nameHisto.c_str());
            newHisto->Sumw2();

            // Fill histogram with data
            for (unsigned int r = 0 ; r < theRegions->size() ; r++)
            {
                Region* region = &((*theRegions)[r]);

                Figure theFigure = theFigureMap[processClass->getTag()]
                                               [region->getTag()]
                                               [theChannel.getTag()];

                newHisto->SetBinContent(r+1, theFigure.value());
                newHisto->SetBinError  (r+1, theFigure.error());
                newHisto->GetXaxis()->SetBinLabel(r+1, region->getLabel().c_str());
            }

            perProcessHistos.push_back(newHisto);
        }

        // ########################
        // ## Create stack histo ##
        // ########################
        
        THStack* theStack   = new THStack("","");

        TH1F* histoSumBackground = new TH1F("","",theRegions->size(), 0, theRegions->size());
        histoSumBackground->Sumw2();
        string nameHisto    = string("f:")+theFigureName.getTag()
                                 +"|p:SumBackground"
                                 +"|c:"+theChannel.getTag()
                                 +"|t:1DFigurePerProcess";
        histoSumBackground->SetName(nameHisto.c_str());
   
        // Now loop on the histos
        for (int p = theProcessClasses->size()-1 ; p >= 0 ; p--)
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

        // Create stack histo + another histo for the error plotting for the background sum
        TPad* theStackPad = thePlot.AddPad(0,0,1,0.8);
        theStackPad->SetTopMargin(0.0);

        // Apply axis style and plot the stack
        theStack->Draw("HIST");
        ApplyAxisStyle(&thePlot,theStack,xlabel,ylabel,theGlobalOptions,theFigureName.getOptions());

        // Display uncertainty
        histoSumBackground->SetFillColor(kBlack);
        histoSumBackground->SetFillStyle(3004);
        histoSumBackground->Draw("SAME E2");

        // ####################
        // ## Signal display ##
        // ####################

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
       
        // ############
        // ##  Data  ##
        // ############

        TH1F* histoSumData = new TH1F("","",theRegions->size(), 0, theRegions->size());
        nameHisto  = string("f:")+theFigureName.getTag()
                          +"|p:SumData"
                          +"|c:"+theChannel.getTag()
                          +"|t:1DFigurePerProcess";
        histoSumData->SetName(nameHisto.c_str());

        // Now loop on the histos
        for (unsigned int p = 0 ; p < theProcessClasses->size() ; p++)
        {
            // Get associated processClass
            ProcessClass* processClass = &((*theProcessClasses)[p]);

            if (processClass->getType() != "data") continue;

            // Get the histo
            TH1F* histoClone = perProcessHistos[p];

            // Add histo to sum
            histoSumData->Add(histoClone);
        }

        // Fill histogram with data
        for (unsigned int r = 0 ; r < theRegions->size() ; r++)
        {
            Region* region = &((*theRegions)[r]);
            histoSumData->GetXaxis()->SetBinLabel(r+1, region->getLabel().c_str());
        }

        // Add it to the legend
        pointersForLegend.push_back(histoSumData);
        labelsForLegend.push_back("data");
        optionsForLegend.push_back("pl");

        // Apply style to data and draw it
        ApplyDataStyle(&thePlot,histoSumData,theGlobalOptions);
        histoSumData->Draw("SAME E");

        // ##############
        // ##  Legend  ##
        // ##############

        // Add stuff to legend in reverse order
        for (unsigned int leg_i = 0 ; leg_i < pointersForLegend.size() ; leg_i++)
        {
            thePlot.AddToLegend(pointersForLegend[pointersForLegend.size()-1-leg_i],
                                labelsForLegend[pointersForLegend.size()-1-leg_i].c_str(),
                                optionsForLegend[pointersForLegend.size()-1-leg_i].c_str());
        }

        // #############
        // ##  Ratio  ##
        // #############

        // Compute and draw the ratio
        TPad* theRatioPad = thePlot.AddPad(0,0.8,1,1);
        theRatioPad->SetBottomMargin(0.07);
        theRatioPad->SetTopMargin(0.3);

        // Make the histogram
        TH1F* histoRatio = new TH1F("","",theRegions->size(), 0, theRegions->size());
        nameHisto  = string("f:")+theFigureName.getTag()
                          +"|p:DataMCRatio"
                          +"|c:"+theChannel.getTag()
                          +"|t:1DFigurePerProcess";
        histoRatio->SetName(nameHisto.c_str());
        histoRatio->Add(histoSumData);
        histoRatio->Divide(histoSumBackground);

        TF1* unity = new TF1("unity","1",-10000,10000);
        unity->SetLineColor(kBlack);
        unity->SetLineStyle(1);
        unity->SetLineWidth(1);

        ApplyRatioStyle(&thePlot,histoRatio,theGlobalOptions);
        ApplyRatioAxisStyle(&thePlot,histoRatio,theGlobalOptions);

        histoRatio->Draw("E");

        // Draw unity
        unity->Draw("SAME");

        thePlot.SetActive();


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

      static void ApplyDataStyle(Plot* thePlot, TH1F* theData, OptionsScrewdriver generalOptions)
      {
          theData->SetMarkerStyle(8);
          theData->SetMarkerSize(1);
          theData->SetLineWidth(1);
          theData->SetLineColor(kBlack);
          theData->SetFillStyle(0);
      }

      static void ApplyAxisStyle(Plot* thePlot, THStack* theStack, string xlabel, string ylabel, OptionsScrewdriver theGlobalOptions, string options = "")
      { 
          PlotDefaultStyles::ApplyDefaultAxisStyle(theStack->GetXaxis(),xlabel);
          PlotDefaultStyles::ApplyDefaultAxisStyle(theStack->GetYaxis(),ylabel);
          theStack->SetTitle("");

          if (OptionsScrewdriver::GetBoolOption(options,"logY"))
          {
              thePlot->SetLogY();
              theStack->SetMaximum(theStack->GetMaximum() * 6.0);
          }
          else
              theStack->SetMaximum(theStack->GetMaximum() * 1.6);
      }

      static void ApplyRatioStyle(Plot* thePlot, TH1F* theRatio, OptionsScrewdriver generalOptions)
      {
          theRatio->SetMarkerStyle(8);
          theRatio->SetMarkerSize(1);
          theRatio->SetLineWidth(1);
          theRatio->SetLineColor(kBlack);
          theRatio->SetFillStyle(0);
      }

      static void ApplyRatioAxisStyle(Plot* thePlot, TH1F* theRatio, OptionsScrewdriver generalOptions)
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
