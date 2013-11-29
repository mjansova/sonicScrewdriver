#ifndef Plot3DProjectedTo2D_h
#define Plot3DProjectedTo2D_h

#include "interface/Common.h"

#include "interface/Plot.h"
#include "interface/Variable.h"
#include "interface/ProcessClass.h"
#include "interface/Histo1D.h"
#include "interface/Histo2D.h"
#include "interface/Histo3D.h"
#include "interface/PlotDefaultStyles.h"
#include "interface/OptionsScrewdriver.h"

namespace theDoctor
{

    class Plot3DProjectedTo2D
    {
      
     public:
     
      Plot3DProjectedTo2D();
      ~Plot3DProjectedTo2D();

      static void MakePlot(Variable* theVarX, 
                           Variable* theVarY, 
                           Variable* theVarZ, 
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
         string ylabel(theVarY->getLabel());
         string zlabel = OptionsScrewdriver::GetStringOption(theProjectionInfos,"labelZ");
         
         // Add the units
         if (theVarX->getUnit() != "")
            xlabel += " [" + theVarX->getUnit() + "]";
         if (theVarY->getUnit() != "")
            ylabel += " [" + theVarY->getUnit() + "]";

     
        // Get clone of the histo
         TH3F* histoClone = theHistoScrewdriver->get3DHistoClone(theVarX->getTag(),
                                                                 theVarY->getTag(),
                                                                 theVarZ->getTag(),
                                                                 theProcessClass->getTag(),
                                                                 theRegion->getTag(),
                                                                 theChannel->getTag());
 
        int nBinsX = theVarX->getNbins(); float minX = theVarX->getMin(); float maxX = theVarX->getMax();
        int nBinsY = theVarY->getNbins(); float minY = theVarY->getMin(); float maxY = theVarY->getMax();
        int nBinsZ = theVarZ->getNbins(); float minZ = theVarZ->getMin(); float maxZ = theVarZ->getMax();
        
        string theProjectionType = OptionsScrewdriver::GetStringOption(theProjectionInfos,"projectionType");

        TH2F* theProjectedHisto = new TH2F((theProjectionType+"|"+histoClone->GetName()).c_str(),"",
                                               nBinsX,minX,maxX,nBinsY,minY,maxY);
        

        for (int x = 1 ; x <= nBinsX ; x++)
        for (int y = 1 ; y <= nBinsY ; y++)
        {
            TH1F localHisto("localHisto","",nBinsZ,minZ,maxZ);
            for (int z = 1 ; z <= nBinsZ ; z++)
            {
                localHisto.SetBinContent(z,histoClone->GetBinContent(x,y,z));
                localHisto.SetBinError(z,histoClone->GetBinError(x,y,z));
            }

            if (theProjectionType == "mean") 
            {
                theProjectedHisto->SetBinContent(x,y,localHisto.GetMean());
                theProjectedHisto->SetBinError(x,y,localHisto.GetMeanError());
            }
            else 
            {
                theProjectedHisto->SetBinContent(x,y,0);
                theProjectedHisto->SetBinError(x,y,0);
            }
        }

        Plot2D::ApplyHistoStyle(thePlot,theProjectedHisto,theProcessClass->getColor(),plotOptions,theProcessClass->getOptions());
        Plot2D::ApplyAxisStyle(thePlot,theProjectedHisto,xlabel,ylabel,zlabel+" for "+theProcessClass->getTag(),plotOptions,theVarX->getOptions(),theVarY->getOptions());
        theProjectedHisto->Draw("COLZ");
     }

     static void GetHistoDependencies(vector<pair<string,string> >& output)
     {
     }

    };

}

#endif
