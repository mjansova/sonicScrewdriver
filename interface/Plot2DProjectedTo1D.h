#ifndef Plot2DProjectedTo1D_h
#define Plot2DProjectedTo1D_h

#include "interface/Common.h"

#include "interface/Plot.h"
#include "interface/Variable.h"
#include "interface/ProcessClass.h"
#include "interface/Histo1D.h"
#include "interface/Histo2D.h"
#include "interface/PlotDefaultStyles.h"
#include "interface/OptionsScrewdriver.h"

namespace theDoctor
{

    class Plot2DProjectedTo1D
    {
      
     public:
     
      Plot2DProjectedTo1D();
      ~Plot2DProjectedTo1D();


      static void MakePlot(Variable* theVarX, 
                           Variable* theVarY, 
                           ProcessClass* theProcessClass, 
                           Region* theRegion, 
                           Channel* theChannel, 
                           HistoScrewdriver* theHistoScrewdriver, 
                           Plot* thePlot, 
                           string theProjectionInfos,
                           string plotOptions = "")
      {

         // Prepare the labels for x and y axis
         // xlabel = labelDeLaVariable (Unité)
         // ylabel = Normalized entries / largeurDeBin Unité

         string xlabel(theVarX->getLabel());
         string ylabel = OptionsScrewdriver::GetStringOption(theProjectionInfos,"labelY");
         
         // Add the units
         if (theVarX->getUnit() != "")
            xlabel += " [" + theVarX->getUnit() + "]";

     
        // Get clone of the histo
         TH2F* histoClone = theHistoScrewdriver->get2DHistoClone(theVarX->getTag(),
                                                                       theVarY->getTag(),
                                                                       theProcessClass->getTag(),
                                                                       theRegion->getTag(),
                                                                       theChannel->getTag());
    
        int nBinsX = theVarX->getNbins(); float minX = theVarX->getMin(); float maxX = theVarX->getMax();
        int nBinsY = theVarY->getNbins(); float minY = theVarY->getMin(); float maxY = theVarY->getMax();
        
        string theProjectionType = OptionsScrewdriver::GetStringOption(theProjectionInfos,"projectionType");

        TH1F* theProjectedHisto = new TH1F((theProjectionType+"|"+histoClone->GetName()).c_str(),"",
                                               nBinsX,minX,maxX);

        for (int x = 1 ; x <= nBinsX ; x++)
        {
            // Create "local" histo for given bin x
            TH1F localHisto("localHisto","",nBinsY,minY,maxY);  
            for (int y = 1 ; y <= nBinsY ; y++)
            {
                localHisto.SetBinContent(y,histoClone->GetBinContent(x,y));
                localHisto.SetBinError(y,histoClone->GetBinError(x,y));
            }

            // Fill the bin x of the projected histo with the info corresponding to projection type
            if (theProjectionType == "mean") 
            {
                theProjectedHisto->SetBinContent(x,localHisto.GetMean());
                theProjectedHisto->SetBinError(x,localHisto.GetMeanError());
            }
            else 
            {
                theProjectedHisto->SetBinContent(x,0);
                theProjectedHisto->SetBinError(x,0);
            }
        }

        ApplyHistoStyle(thePlot,theProjectedHisto,theProcessClass->getColor(),plotOptions,theProcessClass->getOptions());
        ApplyAxisStyle(thePlot,theProjectedHisto,xlabel,ylabel,plotOptions,theVarX->getOptions());
        theProjectedHisto->Draw("hist E0");
            
      }

      static void GetHistoDependencies(vector<pair<string,string> >& output)
      {
      }

      static void ApplyHistoStyle(Plot* thePlot, TH1F* theHisto, Color_t color, string plotOptions = "", string processClassOptions = "")
      {
         theHisto->SetFillColor(0);
         theHisto->SetLineWidth(6);
         theHisto->SetLineColor(color);
      }

      static void ApplyAxisStyle(Plot* thePlot, TH1F* theHisto, string xlabel, string ylabel, string plotOptions = "", string varOptions = "")
      {	
         PlotDefaultStyles::ApplyDefaultAxisStyle(theHisto->GetXaxis(),xlabel);
         PlotDefaultStyles::ApplyDefaultAxisStyle(theHisto->GetYaxis(),ylabel);
         theHisto->SetTitle("");
         theHisto->SetStats(0);

         if (OptionsScrewdriver::GetBoolOption(varOptions,"logY")) thePlot->SetLogY();
      }


    };
}

#endif
