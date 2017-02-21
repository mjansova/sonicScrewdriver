#ifndef Plot_h
#define Plot_h

#include "interface/Common.h"
#include "interface/PlotDefaultStyles.h"
#include "interface/OptionsScrewdriver.h"

void replaceSubstring(string& str, const string oldStr, const string newStr)
{
    size_t pos = 0;
    while((pos = str.find(oldStr, pos)) != string::npos)
    {
        str.replace(pos, oldStr.length(), newStr);
        pos += newStr.length();
    }
}

using namespace std;

namespace theDoctor
{

    class Plot
    {

        public:

            // Plot constructor
            Plot(string name_, string type_, OptionsScrewdriver theGlobalOptions, string options = "")
            {
                theCanvas = new TCanvas(name_.c_str(),"",theGlobalOptions.GetGlobalFloatOption("Plot","width"),theGlobalOptions.GetGlobalFloatOption("Plot","height"));

                theLegend = new TLegend(0.65,0.70,0.89,0.89); theLegendMaxLabelSize = 0; theLegendNumberOfLines = 0;
                theTopLeftInfo  = new TPaveText(0.06,0.94,0.99,0.95,"NDC");
                theTopRightInfo = new TPaveText(0.06,0.94,0.99,0.95,"NDC");
                theInPlotInfo = new TPaveText(0.13,0.77,0.43,0.87,"NDC");

                PlotDefaultStyles::ApplyDefaultCanvasStyle(theCanvas,         theGlobalOptions);
                PlotDefaultStyles::ApplyDefaultLegendStyle(theLegend,         theGlobalOptions);
                PlotDefaultStyles::ApplyDefaultPaveTextStyle(theTopLeftInfo,  theGlobalOptions);
                PlotDefaultStyles::ApplyDefaultPaveTextStyle(theTopRightInfo, theGlobalOptions);
                PlotDefaultStyles::ApplyDefaultPaveTextStyle(theInPlotInfo,   theGlobalOptions);

                theTopLeftInfo ->SetTextAlign(12);
                theTopRightInfo->SetTextAlign(32);

                string infoTopRight = theGlobalOptions.GetGlobalStringOption("Plot","infoTopRight");
                string infoTopLeft  = theGlobalOptions.GetGlobalStringOption("Plot","infoTopLeft");
                theTopLeftInfo ->AddText(infoTopRight.c_str());
                theTopRightInfo->AddText(infoTopLeft.c_str());

                theInPlotInfo->SetTextAlign(11);

                type = type_;
                hasBeenWritten = false;

                SetActive();
            };

            ~Plot() { };

            // Accessors
            string getType()     { return type;      };
            TCanvas* getCanvas() { return theCanvas; };
            TLegend* getLegend() { return theLegend; };

            void getRangeAxis(double* x_min, double* y_min, double* x_max, double* y_max)
            {
                double x_min_, y_min_, x_max_, y_max_;
                theCanvas->GetRange(x_min_,y_min_,x_max_,y_max_);
                *x_min = x_min_;
                *y_min = y_min_;
                *x_min = x_max_;
                *y_min = y_max_;
            }

            void AddToInPlotInfo(string info)
            {
                string line;
                stringstream stream(info);
                while( getline(stream,line,';') )
                {
                    //if (line == "") continue;
                    theInPlotInfo->AddText(line.c_str());
                }
            }

            // Logarithm scale management
            void SetLogX(bool mode = true)
            {
                if (thePads.size() != 0) thePads[0]->SetLogx(mode);
                else theCanvas->SetLogx(mode);
            }
            void SetLogY(bool mode = true)
            {
                if (thePads.size() != 0) thePads[0]->SetLogy(mode);
                else theCanvas->SetLogy(mode);
            }
            void SetLogZ(bool mode = true)
            {
                if (thePads.size() != 0) thePads[0]->SetLogz(mode);
                else theCanvas->SetLogz(mode);
            }

            void SetActive()   { theCanvas->cd();                            };
            void Update()      { theCanvas->Modified(); theCanvas->Update(); };

            TPad* AddPad(float x_start, float y_start, float x_end, float y_end, OptionsScrewdriver theGlobalOptions, string options = "")
            {
                SetActive();

                TPad* newPad = new TPad("","",x_start,y_start,x_end,y_end);
                PlotDefaultStyles::ApplyDefaultPadStyle(newPad, theGlobalOptions);

                newPad->Draw();
                newPad->cd();

                thePads.push_back(newPad);

                if (OptionsScrewdriver::GetBoolOption(options,"legend") )  legendTarget = newPad;
                if (OptionsScrewdriver::GetBoolOption(options,"topInfo"))  topPad       = newPad;

                return newPad;
            }

            void DrawTopInfoText()
            {
                TPad* plotTo;

                if (thePads.size() != 0)
                {
                    plotTo = topPad;
                    theTopLeftInfo ->SetY1(1.0-plotTo->GetTopMargin());
                    theTopLeftInfo ->SetY2(1.0);
                    theTopRightInfo->SetY1(1.0-plotTo->GetTopMargin());
                    theTopRightInfo->SetY2(1.0);
                }
                else
                    plotTo = theCanvas;

                plotTo->cd();

                theTopLeftInfo ->SetX1(    plotTo->GetLeftMargin() -0.04);
                theTopLeftInfo ->SetX2(1.0-plotTo->GetRightMargin()+0.04);
                theTopRightInfo->SetX1(    plotTo->GetLeftMargin() -0.04);
                theTopRightInfo->SetX2(1.0-plotTo->GetRightMargin()+0.04);

                theTopLeftInfo->Draw();
                theTopRightInfo->Draw();
            }

            void DrawLegend()
            {
                TPad* plotTo;
                if (thePads.size() != 0)
                    plotTo = legendTarget;
                else
                    plotTo = theCanvas;

                plotTo->cd();

                PlaceLegend(plotTo,"topright",0.2,0.2);
                theLegend->Draw();

                theInPlotInfo->SetX1(    plotTo->GetLeftMargin()+0.03);
                theInPlotInfo->SetY1(1.0-plotTo->GetTopMargin()-0.15-0.025*theInPlotInfo->GetSize());
                theInPlotInfo->SetX2(theInPlotInfo->GetX1() + 0.4);
                theInPlotInfo->SetY2(theInPlotInfo->GetY1() + 0.1 + 0.025*theInPlotInfo->GetSize());

                //theInPlotInfo->Draw(); @MJ@ TODO do not do that, set some global option!!!

            }

            void Write(string outputFolder, string category, OptionsScrewdriver theGlobalOptions)
            {

                if (hasBeenWritten) return;
                DrawTopInfoText();
                DrawLegend();
                SetActive();
                // FIXME : this has nothing to do here, move this to each classes
                if (type == "1DFrom2DProjection")
                {
                    string varX = GetParameter("variableX");
                    string varY = GetParameter("tagY");
                    theCanvas->SetName((varX + "[vs]" + varY).c_str());
                }
                else if ((type == "2D") || (type == "2DFrom3DProjection") || (type == "2DSuperimposed"))
                {
                    string processClassName = GetParameter("processClass");
                    theCanvas->SetName(processClassName.c_str());
                }
                else if (type == "1DDataMCComparisonFigure")
                {
                    string theName = GetParameter("figure");
                    theCanvas->SetName(theName.c_str());
                }
                else if (type == "1DFigure")
                {
                    string theName = GetParameter("name");
                    theCanvas->SetName(theName.c_str());
                }
                else if (type == "custom")
                {
                    string theName = GetParameter("name");
                    theCanvas->SetName(theName.c_str());
                }
                else
                {
                    string varName = GetParameter("variable");
		    theCanvas->SetName(varName.c_str());
                }

                // Write root output
                theCanvas->Write();

                bool exportPng = theGlobalOptions.GetGlobalBoolOption("Plot","exportPng");
                bool exportEps = theGlobalOptions.GetGlobalBoolOption("Plot","exportEps");
                bool exportPdf = theGlobalOptions.GetGlobalBoolOption("Plot","exportPdf");

                // Optionnal export other formats
                if (exportPng || exportEps || exportPdf)
                {
                    if (category.find("[") != string::npos) category.replace(category.find("["),1,"_");
                    if (category.find("]") != string::npos) category.replace(category.find("]"),1,"_");
                    string shortPlotName = theCanvas->GetName();
                    if (shortPlotName.find("[") != string::npos) shortPlotName.replace(shortPlotName.find("["),1,"_");
                    if (shortPlotName.find("]") != string::npos) shortPlotName.replace(shortPlotName.find("]"),1,"_");

                    string epsFolder = "./"+outputFolder+"/eps/"+category+"/";  string epsFile = epsFolder+shortPlotName+".eps";
                    string pngFolder = "./"+outputFolder+"/png/"+category+"/";  string pngFile = pngFolder+shortPlotName+".png";
                    string pdfFolder = "./"+outputFolder+"/pdf/"+category+"/";  string pdfFile = pdfFolder+shortPlotName+".pdf";

                    // FIXME : move this somewhere else, maybe constructor of sonicscrewdriver
                    gErrorIgnoreLevel = kWarning;

                    if (exportEps)
                    {
                        system((string("mkdir -p ")+epsFolder).c_str());
                        string eraseBeforeCreationEps("rm -f "+epsFile);
                        system(eraseBeforeCreationEps.c_str());
                        theCanvas->SaveAs(epsFile.c_str());
                    }

                    if (exportPdf || exportPng)
                    {
                        system((string("mkdir -p ")+pdfFolder).c_str());
                        string eraseBeforeCreationPdf("rm -f "+pdfFile);
                        system(eraseBeforeCreationPdf.c_str());
                        theCanvas->SaveAs(pdfFile.c_str());
                    }

                    if (exportPng)
                    {
                        system((string("mkdir -p ")+pngFolder).c_str());
                        string eraseBeforeCreationPng("rm -f "+pngFile);
                        system(eraseBeforeCreationPng.c_str());
                        string convertCommand("ls "+pdfFile+" > /dev/null && convert -crop 775x665+350+35 -density 100 "+pdfFile+" "+pngFile);
                        system(convertCommand.c_str());
                        if (!exportPdf)
                        {
                            string erasePdf("rm -f "+pdfFile);
                            system(erasePdf.c_str());
                        }
                    }
                }

                hasBeenWritten = true;
            };

            // Legend edition
            void AddToLegend(const TObject* obj, string label, Option_t* option)
            {
                // Add entry to label
                theLegend->AddEntry(obj,label.c_str(),option);

                // Estimate length of label
                // (replacing latex stuff to avoid having big length when actual display is short)
                replaceSubstring(label,"#rightarrow","->");
                replaceSubstring(label,"#bar","");
                replaceSubstring(label,"{","");
                replaceSubstring(label,"}","");
                replaceSubstring(label,"#times","x");

                if (theLegendMaxLabelSize < label.size())
                    theLegendMaxLabelSize = label.size();

                theLegendNumberOfLines++;
            };

            void PlaceLegend(TPad* pad, string place, float width, float height)
            {
                if ((type == "1DDataMCComparisonFigure") || (type == "1DDataMCComparison"))
                    theLegend->SetTextSize(0.045);
                //if (height < theLegendNumberOfLines * 0.075)
                height = theLegendNumberOfLines * 0.08;
                if (height > 0.35) height = 0.35;

                float abs_right = 1.0-0.03-pad->GetRightMargin();
                float abs_left  = 1.0-0.03-pad->GetLeftMargin();
                float abs_top   = 1.0-0.03-pad->GetTopMargin();
                float abs_bot   = 1.0-0.03-pad->GetBottomMargin();

                float x_min = 0.0;
                float y_min = 0.0;
                float x_max = 0.0;
                float y_max = 0.0;

                if ((place == string("topleft"))   || (place == string("tl")))
                {
                    x_min = abs_left;x_max = abs_left + width;
                    y_min = abs_top - height;y_max = abs_top;
                }
                else if ((place == string("topright"))  || (place == string("tr")))
                {
                    x_min = abs_right - width;x_max = abs_right;
                    y_min = abs_top - height;y_max = abs_top;
                }
                else if ((place == string("topcenter")) || (place == string("tc")))
                {
                    x_min = 0.5 - width/2.0;x_max = 0.5 + width/2.0;
                    y_min = abs_top - height;y_max = abs_top;
                }
                else if ((place == string("botleft"))   || (place == string("bl")))
                {
                    x_min = abs_left;x_max = abs_left + width;
                    y_min = abs_bot;y_max = abs_bot + height;
                }
                else if ((place == string("botright"))  || (place == string("br")))
                {
                    x_min = abs_right - width;x_max = abs_right;
                    y_min = abs_top;y_max = abs_bot + height;
                }
                else if ((place == string("botcenter")) || (place == string("bc")))
                {
                    x_min = 0.5 - width/2.0;x_max = 0.5 + width/2.0;
                    y_min = abs_bot;y_max = abs_bot + height;
                }

                // FIXME : find a cleverer way instead of putting 0.02 by hand
                float offset = max(0.0,(theLegendMaxLabelSize * 0.017) - width);

                theLegend->SetX1(x_min-offset);
                theLegend->SetY1(y_min);
                theLegend->SetX2(x_max-offset);
                theLegend->SetY2(y_max);

            }

            void SetParameter(string field, string value) { parameters[field] = value; }
            string GetParameter(string field) { return parameters[field]; }

        private:

            TCanvas* theCanvas;
            TLegend* theLegend;
            unsigned int theLegendNumberOfLines;
            unsigned int theLegendMaxLabelSize;
            TPaveText* theTopLeftInfo;
            TPaveText* theTopRightInfo;
            TPaveText* theInPlotInfo;
            vector<TPad*> thePads;

            TPad* legendTarget;
            TPad* topPad;


            string type;
            std::map<string,string> parameters;

            // TODO : might be a dirty solution to the problem
            // Should find another way to do this if possible
            bool hasBeenWritten;

    };

}

#endif
