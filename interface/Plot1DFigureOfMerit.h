#ifndef Plot1DFigureOfMerit_h
#define Plot1DFigureOfMerit_h

#include <math.h>

#include "interface/Histo1DFigureOfMerit.h"
#include "interface/PlotDefaultStyles.h"
#include "interface/OptionsScrewdriver.h"

namespace theDoctor
{

    class Plot1DFigureOfMerit 
    {
      
     public:
     
      Plot1DFigureOfMerit();
      ~Plot1DFigureOfMerit();

      static void MakePlot(Histo1DFigureOfMerit* theHisto_,
                           Plot* thePlot,
                           string plotTypeOptions = "",
                           string plotOptions = "")
      {
          Variable* theVar = theHisto_->getVariable();

          // Prepare the labels for x and y axis
          // xlabel = labelDeLaVariable (Unité)
          // ylabel = Normalized entries / largeurDeBin Unité

          string xlabel("Cut on "+theVar->getLabel());
          string ylabel("Figure of merit");

          // Add the unit
          if (theVar->getUnit() != "")
              xlabel += string(" [") + theVar->getUnit() + string("]");

          // Get the actual histo
          TH1F* theHisto = theHisto_->getClone();

          // Apply style and draw the ratio
          ApplyAxisStyle(thePlot,theHisto,xlabel,ylabel,plotOptions);
          ApplyHistoStyle(thePlot,theHisto,COLORPLOT_AZURE,plotOptions);
          theHisto->Draw("hist E0");


      }

      static void GetHistoDependencies(vector<pair<string,string> >& output, string options = "")
      {
         Histo1DSumBackground::GetHistoDependencies(output); 
         output.push_back(pair<string,string>("1DSumBackground",""));
         output.push_back(pair<string,string>("1DFigureOfMerit",options));
      }

     private:

      static void ApplyHistoStyle(Plot* thePlot, TH1F* theHisto, Color_t color, string plotOptions = "")
      {
         theHisto->SetFillColor(0);
         theHisto->SetLineWidth(6);
         theHisto->SetLineColor(color);
         
      }

      static void ApplyAxisStyle(Plot* thePlot, TH1F* theHisto, string xlabel, string ylabel, string plotOptions = "")
      {	
         PlotDefaultStyles::ApplyDefaultAxisStyle(theHisto->GetXaxis(),xlabel);
         PlotDefaultStyles::ApplyDefaultAxisStyle(theHisto->GetYaxis(),ylabel);
         theHisto->SetTitle("");
         theHisto->SetStats(0);
      }

    };

}
#endif
