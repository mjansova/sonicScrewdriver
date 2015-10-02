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

         string ratioPosition = theGlobalOptions.GetGlobalStringOption("DataMCComparison",        "ratioPosition");
         string includeSignal = theGlobalOptions.GetGlobalStringOption("1DDataMCComparisonFigure","includeSignal");
         float  factorSignal  = theGlobalOptions.GetGlobalFloatOption( "1DDataMCComparisonFigure","factorSignal");
         string factorSignalStr = floatToString(factorSignal);

         string xlabel("");
         string ylabel(theFigureName.getLabel());

        vector<TH1D*>  pointersForLegend;
        vector<string> labelsForLegend;
        vector<string> optionsForLegend;

        // ##########################################
        // ## Create invididual per-process histos ##
        // ##########################################

        vector<TH1D*> perProcessHistos;
        for (unsigned int p = 0 ; p < theProcessClasses->size() ; p++)
        {
            ProcessClass* processClass = &((*theProcessClasses)[p]);
            TH1D* newHisto   = new TH1D("","",theRegions->size(), 0, theRegions->size());
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

        TH1D* histoSumBackground = new TH1D("","",theRegions->size(), 0, theRegions->size());
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
            TH1D* histoClone = perProcessHistos[p];

            // Change style of histo and add it to legend
            ApplyHistoStyle(&thePlot,histoClone,processClass->getColor(),theGlobalOptions,processClass->getOptions());

            pointersForLegend.push_back(histoClone);
            optionsForLegend.push_back(string("f"));
			labelsForLegend.push_back(processClass->getLabel());

            // Add histo to stack
            theStack->Add(histoClone);
            histoSumBackground->Add(histoClone);
        }

        TPad* theRatioPad;
        TPad* theStackPad;
        if (ratioPosition == "top")
        {
           theStackPad = thePlot.AddPad(0,0,1,0.8,"legend");
           theRatioPad = thePlot.AddPad(0,0.8,1,1,"topInfo");

           theRatioPad->SetBottomMargin(0.07);
           theRatioPad->SetTopMargin(0.3);
           theStackPad->SetTopMargin(0.0);
        }
        else
        {
           theStackPad = thePlot.AddPad(0,0.25,1,1,"legend,topInfo");
           theRatioPad = thePlot.AddPad(0,0,1,0.25,"");

           theRatioPad->SetTopMargin(0.05);
           theRatioPad->SetBottomMargin(0.35);
           theStackPad->SetBottomMargin(0.02);
           theStackPad->SetTopMargin(0.06);
        }

        theStackPad->cd();

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

                TH1D* histoClone = perProcessHistos[p];
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

        TH1D* histoSumData = new TH1D("","",theRegions->size(), 0, theRegions->size());
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
            TH1D* histoClone = perProcessHistos[p];

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

        theRatioPad->cd();

        // Make the histogram
        TH1D* histoRatio = new TH1D("","",theRegions->size(), 0, theRegions->size());
        nameHisto  = string("f:")+theFigureName.getTag()
                          +"|p:DataMCRatio"
                          +"|c:"+theChannel.getTag()
                          +"|t:1DFigurePerProcess";
        histoRatio->SetName(nameHisto.c_str());
        histoRatio->Add(histoSumData);
        histoRatio->Divide(histoSumBackground);

        // Set labels for ratio histogram
        for (unsigned int r = 0 ; r < theRegions->size() ; r++)
        {
            Region* region = &((*theRegions)[r]);
            histoRatio->GetXaxis()->SetBinLabel(r+1, region->getLabel().c_str());
        }


        TF1* unity = new TF1("unity","1",-10000,10000);
        unity->SetLineColor(kBlack);
        unity->SetLineStyle(1);
        unity->SetLineWidth(1);

        ApplyRatioStyle(&thePlot,histoRatio,theGlobalOptions);
        ApplyRatioAxisStyle(&thePlot,histoRatio,xlabel,theGlobalOptions,theFigureName.getOptions());

        histoRatio->Draw("E");

        // Draw unity
        unity->Draw("SAME");

        thePlot.SetActive();


        return thePlot;
      }

     private:

      static void ApplyHistoStyle(Plot* thePlot, TH1D* theHisto, Color_t color, OptionsScrewdriver theGlobalOptions, string processClassOptions = "")
      {
         theHisto->SetFillColor(color);
         theHisto->SetLineColor(kBlack);
         theHisto->SetLineWidth(2);
      }

	  static void ApplyHistoSignalStyle(Plot* thePlot, TH1D* theHisto, Color_t color, OptionsScrewdriver theGlobalOptions, string processClassOptions = "")
      {
         theHisto->SetFillColor(0);
         theHisto->SetLineWidth(3);
         theHisto->SetLineColor(color);
		 theHisto->SetLineStyle(9);
	  }

      static void ApplyDataStyle(Plot* thePlot, TH1D* theData, OptionsScrewdriver generalOptions)
      {
          theData->SetMarkerStyle(8);
          theData->SetMarkerSize(1);
          theData->SetLineWidth(1);
          theData->SetLineColor(kBlack);
          theData->SetFillStyle(0);
      }

      static void ApplyAxisStyle(Plot* thePlot, THStack* theStack, string xlabel, string ylabel, OptionsScrewdriver generalOptions, string options = "")
      {
          string ratioPosition = generalOptions.GetGlobalStringOption("DataMCComparison","ratioPosition");
          bool   logY = OptionsScrewdriver::GetBoolOption(options,"logY");

          if (ratioPosition == "top")
             PlotDefaultStyles::ApplyDefaultAxisStyle(theStack->GetXaxis(),xlabel);
          else
             theStack->GetXaxis()->SetLabelSize(0.0);

          PlotDefaultStyles::ApplyDefaultAxisStyle(theStack->GetYaxis(),ylabel);

          theStack->SetTitle("");
          if (logY)
          {
              thePlot->SetLogY();
              theStack->SetMaximum(theStack->GetMaximum() * SONICSCREWDRIVER_RANGE_RESCALER_WITHLOG);
          }
          else
          {
              theStack->SetMaximum(theStack->GetMaximum() * SONICSCREWDRIVER_RANGE_RESCALER_NOLOG);
          }
      }

      static void ApplyRatioStyle(Plot* thePlot, TH1D* theRatio, OptionsScrewdriver generalOptions)
      {
          PlotDefaultStyles::ApplyDefaultMarkerStyle(theRatio, kBlack);
      }

      static void ApplyRatioAxisStyle(Plot* thePlot, TH1D* theRatio, string xlabel, OptionsScrewdriver generalOptions, string varOptions = "")
      {
          string ratioPosition = generalOptions.GetGlobalStringOption("DataMCComparison","ratioPosition");

          float  ratioMin      = generalOptions.GetGlobalFloatOption("DataMCRatio","min");
          float  ratioMax      = generalOptions.GetGlobalFloatOption("DataMCRatio","max");

          if (ratioMin == -1) ratioMin = 0.5;
          if (ratioMax == -1) ratioMax = 1.5;

          // Y axis
          PlotDefaultStyles::ApplyDefaultAxisStyle(theRatio->GetYaxis(),string("data/SM"));
          theRatio->GetYaxis()->CenterTitle();
          theRatio->GetYaxis()->SetTickLength(0.015);
          theRatio->GetYaxis()->SetTitleSize(0.17);
          theRatio->GetYaxis()->SetTitleOffset(0.25);
          theRatio->SetMinimum(ratioMin);
          theRatio->SetMaximum(ratioMax);
          theRatio->GetYaxis()->SetNdivisions(4);

          // X axis
          if (ratioPosition == "top")
          {
              PlotDefaultStyles::ApplyDefaultAxisStyle(theRatio->GetXaxis(), "");
              theRatio->GetXaxis()->SetLabelSize(0.0);
          }
          else
          {
              PlotDefaultStyles::ApplyDefaultAxisStyle(theRatio->GetXaxis(), xlabel) ;
              theRatio->GetXaxis()->SetTitleSize(0.22);
              theRatio->GetXaxis()->SetTitleOffset(0.65);
          }

          theRatio->GetXaxis()->SetTickLength(0.1);

          // Misc stuff
          theRatio->SetTitle("");
          theRatio->SetStats(0);

      }

    };

}

#endif
