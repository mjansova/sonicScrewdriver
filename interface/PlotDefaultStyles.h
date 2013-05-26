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
	 theAxis->SetTitleSize(0.06);
	 theAxis->SetTitleOffset(0.7);
	 theAxis->SetLabelFont(PLOTDEFAULTSTYLES_FONT+2);
  }

  // PaveText style
  static void ApplyDefaultPaveTextStyle(TPaveText* thePaveText)
  {
	 thePaveText->SetFillColor(kWhite);
	 thePaveText->SetBorderSize(0);
	 thePaveText->SetTextFont(PLOTDEFAULTSTYLES_FONT+2);
	 thePaveText->SetTextAlign(12);
	 thePaveText->SetTextSize(0.05);
  }

  // Legend style
  static void ApplyDefaultLegendStyle(TLegend* theLegend)
  {
	 theLegend->SetBorderSize(0);
	 theLegend->SetFillColor(kWhite);
	 theLegend->SetFillStyle(0);
	 theLegend->SetTextFont(PLOTDEFAULTSTYLES_FONT+2);
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
  }


};
#endif
