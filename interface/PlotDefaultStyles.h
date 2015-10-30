#ifndef PlotDefaultStyles_h
#define PlotDefaultStyles_h

#include "interface/Common.h"
#include "interface/OptionsScrewdriver.h"

namespace theDoctor
{

class PlotDefaultStyles
{

  public:

  PlotDefaultStyles();
  ~PlotDefaultStyles();

  // Axis style
  static void ApplyDefaultAxisStyle(TAxis* theAxis, string label, OptionsScrewdriver globalOptions)
  {
	 theAxis->SetTitle      (label.c_str());
	 theAxis->SetTitleFont  (globalOptions.GetGlobalFloatOption("General","textFont")*10+2 );
	 theAxis->SetTitleSize  (globalOptions.GetGlobalFloatOption("Axis","titleSize")        );
	 theAxis->SetTitleOffset(globalOptions.GetGlobalFloatOption("Axis","titleOffset")      );
	 theAxis->SetLabelFont  (globalOptions.GetGlobalFloatOption("General","textFont")*10+3 );
	 theAxis->SetLabelSize  (globalOptions.GetGlobalFloatOption("Axis","labelSize")        );
  }

  // PaveText style
  static void ApplyDefaultPaveTextStyle(TPaveText* thePaveText, OptionsScrewdriver globalOptions)
  {
	 thePaveText->SetFillColor(kWhite);
	 thePaveText->SetFillStyle(0);
	 thePaveText->SetBorderSize(0);
	 thePaveText->SetTextFont(globalOptions.GetGlobalFloatOption("General","textFont")*10+3);
	 thePaveText->SetTextSize(26);
  }

  // Legend style
  static void ApplyDefaultLegendStyle(TLegend* theLegend, OptionsScrewdriver globalOptions)
  {
	 theLegend->SetBorderSize(globalOptions.GetGlobalFloatOption("Legend","borderSize")     );
	 theLegend->SetFillColor (globalOptions.GetGlobalFloatOption("Legend","fillColor")      );
	 theLegend->SetTextFont  (globalOptions.GetGlobalFloatOption("General","textFont")*10+2 );
	 theLegend->SetTextSize  (globalOptions.GetGlobalFloatOption("Legend","textSize")       );
	 theLegend->SetFillStyle (0);
  }

  // Canvas style
  static void ApplyDefaultCanvasStyle(TCanvas* theCanvas, OptionsScrewdriver globalOptions)
  {
	theCanvas->SetTickx();
	theCanvas->SetTicky();
	theCanvas->SetFillColor(kWhite);
	theCanvas->SetFillStyle(0);
    theCanvas->SetRightMargin(0.05);
    theCanvas->SetTopMargin(0.08);
  }

  // Pad
  static void ApplyDefaultPadStyle(TPad* thePad, OptionsScrewdriver globalOptions)
  {
	thePad->SetTickx();
	thePad->SetTicky();
	thePad->SetFillColor(kWhite);
	thePad->SetFillStyle(0);
    thePad->SetRightMargin(0.05);
    thePad->SetTopMargin(0.05);
  }

  // Data style
  static void ApplyDefaultMarkerStyle(TH1D* histo, Color_t color, OptionsScrewdriver globalOptions)
  {
      histo->SetMarkerStyle(8);
      histo->SetMarkerSize(1.25);
      histo->SetLineWidth(2);
      histo->SetMarkerColor(color);
      histo->SetLineColor(color);
      histo->SetFillStyle(0);
  }

  static void ApplyDefaultStylePalette(TH2D* histo, TCanvas* theCanvas, OptionsScrewdriver globalOptions)
  {
      theCanvas->Modified();
      theCanvas->Update();
      TPaletteAxis* pal = (TPaletteAxis*) histo->GetListOfFunctions()->FindObject("palette");
      if (pal != 0)
      {
          pal->SetX1NDC(0.901);
          pal->SetY1NDC(0.1);
          pal->SetX2NDC(0.93);
          pal->SetY2NDC(1.0-theCanvas->GetTopMargin());
      }
      else
      {
          // Disabling this warning message - palette not found usually
          // mean that the 2D plot is empty, which might not be a bug (i.e. for a given process
          // its empty because variables are not defined for it)
          //WARNING_MSG << "Palette not found for 2D histo " << histo->GetName() << endl;
      }
  }

  static void Set2DPalette(OptionsScrewdriver globalOptions, string option = "standard")
  {
      const int NRGBs = 5;
      const int NCont = 999;

      if (option == "soft")
      {
          double stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00  };
          double red[NRGBs]   = { 0.25, 0.25, 0.95, 1.00, 1.00  };
          double green[NRGBs] = { 0.25, 0.90, 1.00, 0.40, 0.25  };
          double blue[NRGBs]  = { 0.75, 1.00, 0.30, 0.20, 0.25  };
          TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
          gStyle->SetNumberContours(NCont);
      }
      else
      {
          double stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
          double red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
          double green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
          double blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
          TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
          gStyle->SetNumberContours(NCont);
      }
  }

};

}

#endif
