#ifndef PlotDefaultStyles_h
#define PlotDefaultStyles_h

#include "interface/Common.h"


#define PLOTDEFAULTSTYLES_FONT 40		// Helvetica
//#define PLOTDEFAULTSTYLES_FONT 130	// Times


class PlotDefaultStyles
{

 public:

  PlotDefaultStyles();
  ~PlotDefaultStyles();

  // Axis style
  static void ApplyDefaultAxisStyle(TAxis* theAxis, string label)
  {
	 theAxis->SetTitle(label.c_str());
	 theAxis->SetTitleFont(PLOTDEFAULTSTYLES_FONT+2);
	 theAxis->SetTitleSize(0.05);
	 theAxis->SetTitleOffset(0.9);
	 theAxis->SetLabelFont(PLOTDEFAULTSTYLES_FONT+3);
	 theAxis->SetLabelSize(22);
  }

  // PaveText style
  static void ApplyDefaultPaveTextStyle(TPaveText* thePaveText)
  {
	 thePaveText->SetFillColor(kWhite);
	 thePaveText->SetFillStyle(0);
	 thePaveText->SetBorderSize(0);
	 thePaveText->SetTextFont(PLOTDEFAULTSTYLES_FONT+3);
	 thePaveText->SetTextSize(26);
  }

  // Legend style
  static void ApplyDefaultLegendStyle(TLegend* theLegend)
  {
	 theLegend->SetBorderSize(0);
	 theLegend->SetFillColor(kWhite);
	 theLegend->SetFillStyle(0);
	 theLegend->SetTextFont(PLOTDEFAULTSTYLES_FONT+2);
	 theLegend->SetTextSize(0.035);
  }

  // Canvas style
  static void ApplyDefaultCanvasStyle(TCanvas* theCanvas)
  {
    //theCanvas->SetFrameLineWidth(3);
	//theCanvas->SetFixedAspectRatio();
	theCanvas->SetTickx();
	theCanvas->SetTicky();
	theCanvas->SetFillColor(kWhite);
	theCanvas->SetFillStyle(0);
    theCanvas->SetRightMargin(0.05);
    theCanvas->SetTopMargin(0.08);
  }

  // Pad
  static void ApplyDefaultPadStyle(TPad* thePad)
  {
    //theCanvas->SetFrameLineWidth(3);
	//theCanvas->SetFixedAspectRatio();
	thePad->SetTickx();
	thePad->SetTicky();
	thePad->SetFillColor(kWhite);
	thePad->SetFillStyle(0);
    thePad->SetRightMargin(0.05);
    thePad->SetTopMargin(0.05);
  }

  // Data style
  static void ApplyDefaultMarkerStyle(TH1F* histo, Color_t color)
  {
      histo->SetMarkerStyle(8);
      histo->SetMarkerSize(1.25);
      histo->SetLineWidth(2);
      histo->SetMarkerColor(color);
      histo->SetLineColor(color);
      histo->SetFillStyle(0);
  }

};
#endif
