#ifndef Plot1DFigure_h
#define Plot1DFigure_h

#include "interface/Common.h"

#include "interface/Plot.h"
#include "interface/Variable.h"
#include "interface/ProcessClass.h"
#include "interface/PlotDefaultStyles.h"
#include "interface/OptionsScrewdriver.h"

namespace theDoctor
{

    class Plot1DFigure
    {

     public:

      Plot1DFigure();
      ~Plot1DFigure();


      static void GetHistoDependencies(vector<pair<string,string> >& output, string options = "")
      {
      }

      static vector<Plot> Produce(vector<pair<Name,Map2DFigure> >*  theFigureMap,
                                  vector<Region>*                   theRegions,
                                  vector<Channel>*                  theChannels,
                                  OptionsScrewdriver                theGlobalOptions,
                                  string                            thePlotOptions)
      {
          vector<Plot> theOutput;

          string canvasName = OptionsScrewdriver::GetStringOption(thePlotOptions,"name");

          vector<string>  figureTagsToPlot = OptionsScrewdriver::GetStringListOption(thePlotOptions,"figures");

          // Fill name and map vectors to be given to MakePlot
          vector<Name>          figureNamesToPlot;
          vector<Map2DFigure>   figureMapsToPlot;
          for (unsigned int f = 0 ; f < figureTagsToPlot.size() ; f++)
          {
              string      figureTagToPlot = figureTagsToPlot[f];
              bool        found = false;
              Name        figureName("","");
              Map2DFigure figureMap;

              for (unsigned int f2 = 0 ; f2 < theFigureMap->size() ; f2++)
              {
                  if ((*theFigureMap)[f2].first.getTag() == figureTagToPlot)
                  {
                      found = true;
                      figureName = (*theFigureMap)[f2].first;
                      figureMap  = (*theFigureMap)[f2].second;
                      break;
                  }
              }

              if (found)
              {
                  figureNamesToPlot.push_back(figureName);
                  figureMapsToPlot .push_back(figureMap);
              }
              else
              {
                  WARNING_MSG << "Unable to find figure named " << figureTagToPlot << endl;
              }
          }

          vector<string> channelRestriction = OptionsScrewdriver::GetStringListOption(thePlotOptions,"channel");

          for (unsigned int c = 0 ; c < theChannels->size()  ; c++)
          {
              Channel  theChannel = (*theChannels)[c];

              bool producePlotForThisChannel = false;
              if (channelRestriction.size() == 0) producePlotForThisChannel = true;
              for (unsigned int i = 0 ; i < channelRestriction.size() ; i++)
              {
                  if (channelRestriction[i] == theChannel.getTag()) producePlotForThisChannel = true;
              }

              if (producePlotForThisChannel)
                  theOutput.push_back(
                                    MakePlot(canvasName,
                                             figureNamesToPlot,
                                             figureMapsToPlot,
                                             theRegions,
                                             theChannel,
                                             theGlobalOptions,
                                             thePlotOptions)
                                 );

          }

          return theOutput;
      }

      static Plot MakePlot(string              canvasName,
                           vector<Name>        theFigureNames,
                           vector<Map2DFigure> theFigureMaps,
                           vector<Region>*     theRegions,
                           Channel             theChannel,
                           OptionsScrewdriver  theGlobalOptions,
                           string              thePlotOptions)
      {

         string plotName = string("t:1DFigure|n:")+canvasName
                                           +"|c:" +theChannel.getTag();
         Plot thePlot(plotName,"1DFigure",theGlobalOptions,"");

         thePlot.SetParameter("name",   canvasName);
         thePlot.SetParameter("channel",theChannel.getTag());
         thePlot.AddToInPlotInfo(theChannel.getLabel());
         //thePlot.getCanvas()->SetBottomMargin(0.15);
         //thePlot.getCanvas()->SetRightMargin(0.15);

         string includeSignal = theGlobalOptions.GetGlobalStringOption("1DFigure","includeSignal");
         float  factorSignal  = theGlobalOptions.GetGlobalFloatOption( "1DFigure","factorSignal");
         string factorSignalStr = floatToString(factorSignal);

         string xlabel("");
         string ylabel("");

        vector<TH1F*>  pointersForLegend;
        vector<string> labelsForLegend;
        vector<string> optionsForLegend;

        // ##########################
        // ##  Prepare histograms  ##
        // ##########################

        vector<TH1F*> perFigureHistos;
        for (unsigned int f = 0 ; f < theFigureNames.size() ; f++)
        {
            Name theFigureName       = theFigureNames[f];
            Map2DFigure theFigureMap = theFigureMaps[f];

            TH1F* newHisto     = new TH1F("","",theRegions->size(), 0, theRegions->size());
            string nameHisto =  string("f:")+theFigureName.getTag()
                                     +"|c:"+theChannel.getTag()
                                     +"|t:1DFigure";
            newHisto->SetName(nameHisto.c_str());

            // Fill the histogram
            for (unsigned int r = 0 ; r < theRegions->size() ; r++)
            {
                Region* region = &((*theRegions)[r]);

                Figure theFigure = theFigureMap[region->getTag()]
                                               [theChannel.getTag()];

                newHisto->SetBinContent(r+1, theFigure.value());
                newHisto->SetBinError  (r+1, theFigure.error());
                newHisto->GetXaxis()->SetBinLabel(r+1, region->getLabel().c_str());
            }

            perFigureHistos.push_back(newHisto);
        }

        // #####################
        // ##  Draw the plot  ##
        // #####################

        // FIXME, should be configurable per figure from the front-end
        vector<Color_t> colors = { COLORPLOT_BLACK, COLORPLOT_BLUE, COLORPLOT_GREEN, COLORPLOT_RED, COLORPLOT_CYAN, COLORPLOT_ORANGE, COLORPLOT_MAGENTA };

        for (unsigned int f = 0 ; f < theFigureNames.size() ; f++)
        {
            Name theFigureName  = theFigureNames[f];
            TH1F* theHisto      = perFigureHistos[f];

            ApplyHistoFigureStyle(&thePlot, theHisto, colors[f], theGlobalOptions, thePlotOptions);

            if (f == 0)
            {
                float rangeMinOption = OptionsScrewdriver::GetFloatOption(thePlotOptions,"min");
                float rangeMaxOption = OptionsScrewdriver::GetFloatOption(thePlotOptions,"max");

                if (rangeMinOption != -1) theHisto->SetMinimum(rangeMinOption);
                if (rangeMaxOption != -1) theHisto->SetMaximum(rangeMaxOption);

                ApplyAxisStyle(&thePlot, theHisto, theGlobalOptions, thePlotOptions);
                theHisto->Draw("E");
            }
            else
            {
                theHisto->Draw("SAME E");
            }

			// Add to legend
		    pointersForLegend.insert(pointersForLegend.begin(),theHisto   );
		    optionsForLegend. insert(optionsForLegend .begin(),string("pl"));
            labelsForLegend.  insert(labelsForLegend  .begin(),theFigureName.getLabel());

        }

        // #######################
        // ##  Additional line  ##
        // #######################

        float lineValue       = OptionsScrewdriver::GetFloatOption(thePlotOptions,"lineValue");
        float lineUncertainty = OptionsScrewdriver::GetFloatOption(thePlotOptions,"lineUncert");

        if (lineValue != -1.0)
        {

            TH1F* histLineValue       = new TH1F("","",theRegions->size(), 0, theRegions->size());
            TH1F* histLinePlus1Sigma  = new TH1F("","",theRegions->size(), 0, theRegions->size());
            TH1F* histLineMinus1Sigma = new TH1F("","",theRegions->size(), 0, theRegions->size());

            for (unsigned int i = 0 ; i < theRegions->size() ; i++)
            {
                histLineValue      ->SetBinContent(i+1,lineValue);
                histLinePlus1Sigma ->SetBinContent(i+1,lineValue+lineUncertainty);
                histLineMinus1Sigma->SetBinContent(i+1,lineValue-lineUncertainty);
            }

            histLineValue->SetLineColor(COLORPLOT_RED);
            histLineValue->SetLineStyle(1);
            histLineValue->SetLineWidth(2);

            histLinePlus1Sigma->SetLineColor(COLORPLOT_BLUE);
            histLinePlus1Sigma->SetLineStyle(2);
            histLinePlus1Sigma->SetLineWidth(2);

            histLineMinus1Sigma->SetLineColor(COLORPLOT_BLUE);
            histLineMinus1Sigma->SetLineStyle(2);
            histLineMinus1Sigma->SetLineWidth(2);

            histLineValue      ->Draw("SAME");
            histLinePlus1Sigma ->Draw("SAME");
            histLineMinus1Sigma->Draw("SAME");
        }

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

        return thePlot;
      }

     private:

	  static void ApplyHistoFigureStyle(Plot* thePlot, TH1F* theHisto, Color_t color, OptionsScrewdriver theGlobalOptions, string options = "")
      {
          theHisto->SetMarkerStyle(8);
          theHisto->SetMarkerSize(1);
          theHisto->SetMarkerColor(color);
          theHisto->SetLineWidth(1);
          theHisto->SetLineColor(color);
          theHisto->SetFillStyle(0);
	  }

      static void ApplyAxisStyle(Plot* thePlot, TH1F* theHisto, OptionsScrewdriver theGlobalOptions, string options = "")
      {
          PlotDefaultStyles::ApplyDefaultAxisStyle(theHisto->GetXaxis(),"");
          PlotDefaultStyles::ApplyDefaultAxisStyle(theHisto->GetYaxis(),"");
          theHisto->SetStats(0);
          theHisto->SetTitle("");
      }

    };

}

#endif
