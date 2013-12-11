#ifndef Plot_h
#define Plot_h

#include "interface/Common.h" 
#include "interface/PlotDefaultStyles.h"
#include "interface/OptionsScrewdriver.h"

using namespace std;

namespace theDoctor
{

    class Plot 
    {

        public:

            // Plot constructor
            Plot(string name_, string type_, OptionsScrewdriver theGlobalOptions, string options = "")
            {
                theCanvas = new TCanvas(name_.c_str(),"",850,750);
                theLegend = new TLegend(0.65,0.70,0.89,0.89);
                theTopLeftInfo  = new TPaveText(0.06,0.93,0.99,0.97,"NDC");
                theTopRightInfo = new TPaveText(0.06,0.93,0.99,0.97,"NDC");
                theInPlotInfo = new TPaveText(0.13,0.77,0.43,0.87,"NDC");

                PlotDefaultStyles::ApplyDefaultCanvasStyle(theCanvas);
                PlotDefaultStyles::ApplyDefaultLegendStyle(theLegend);
                PlotDefaultStyles::ApplyDefaultPaveTextStyle(theTopLeftInfo);
                PlotDefaultStyles::ApplyDefaultPaveTextStyle(theTopRightInfo);
                PlotDefaultStyles::ApplyDefaultPaveTextStyle(theInPlotInfo);

                theTopLeftInfo ->SetTextAlign(12);
                theTopRightInfo->SetTextAlign(32);

                string infoTopRight = theGlobalOptions.GetGlobalStringOption("Plot","infoTopRight");
                string infoTopLeft  = theGlobalOptions.GetGlobalStringOption("Plot","infoTopLeft");
                theTopLeftInfo ->AddText(infoTopRight.c_str());
                theTopRightInfo->AddText(infoTopLeft.c_str());

                theInPlotInfo->SetTextAlign(11);

                type = type_;

                SetActive();
            };

            ~Plot() { };

            // Accessors
            string getType()     { return type;      };
            TCanvas* getCanvas() { return theCanvas; };
            TLegend* getLegend() { return theLegend; }; 

            void AddToInPlotInfo(string info)
            {
                if (info != "") theInPlotInfo->AddText(info.c_str());
            }

            // Logarithm scale management
            void SetLogX() 
            { 
                if (thePads.size() != 0) thePads[0]->SetLogx(); 
                else theCanvas->SetLogx(); 
            }
            void SetLogY() 
            { 
                if (thePads.size() != 0) thePads[0]->SetLogy(); 
                else theCanvas->SetLogy(); 
            }
            void SetLogZ() 
            { 
                if (thePads.size() != 0) thePads[0]->SetLogz(); 
                else theCanvas->SetLogz(); 
            } 

            void SetActive()   { theCanvas->cd();                            };
            void Update()      { theCanvas->Modified(); theCanvas->Update(); };

            TPad* AddPad(float x_start, float y_start, float x_end, float y_end) 
            {
                SetActive();

                TPad* newPad = new TPad("","",x_start,y_start,x_end,y_end);
                PlotDefaultStyles::ApplyDefaultPadStyle(newPad);

                newPad->Draw();
                newPad->cd();

                thePads.push_back(newPad);
                return newPad;
            }

            void DrawInfoText()
            {
                TPad* plotTo;

                if (thePads.size() != 0)
                {
                    // TODO : translate / generalize this in term of top margin and paveText height
                    theTopLeftInfo ->SetY1(0.75);
                    theTopLeftInfo ->SetY2(0.9);
                    theTopRightInfo->SetY1(0.75);
                    theTopRightInfo->SetY2(0.9);
                    plotTo = thePads[thePads.size()-1];
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
                    plotTo = thePads[0];
                else
                    plotTo = theCanvas;

                plotTo->cd();
                                       
                PlaceLegend(plotTo,"topright",0.2,0.3);
                theLegend->Draw();

                theInPlotInfo->SetX1(    plotTo->GetLeftMargin()+0.03);
                theInPlotInfo->SetY1(1.0-plotTo->GetTopMargin()-0.15);
                theInPlotInfo->SetX2(theInPlotInfo->GetX1() + 0.4);
                theInPlotInfo->SetY2(theInPlotInfo->GetY1() + 0.1);

                theInPlotInfo->Draw();

            }
                
            void Write(string outputFolder = "", string infoText = "", string options = "")
            {

                DrawInfoText();
                DrawLegend();
                SetActive();

                if ((type == "2D") || (type == "2DProjectedTo1D"))
                {
                    string processClassName = GetParameter("processClass");
                    theCanvas->SetName(processClassName.c_str());
                }
                else if  (type == "3DProjectedTo2D")
                {
                    string processClassName = GetParameter("processClass");
                    string varZ = GetParameter("variableZ");
                    theCanvas->SetName((varZ+"|"+processClassName).c_str());
                }
                else
                {
                    string varName = GetParameter("variable");
                    theCanvas->SetName(varName.c_str());
                }

                // Write root output
                theCanvas->Write();

                // Optionnal export in png and eps 
                if (OptionsScrewdriver::GetBoolOption(options,"exportPngAndEps"))
                {
                    if (outputFolder.find("[") != string::npos) outputFolder.replace(outputFolder.find("["),1,"_");
                    if (outputFolder.find("]") != string::npos) outputFolder.replace(outputFolder.find("]"),1,"_");
                    string shortPlotName = theCanvas->GetName();

                    // TODO Should do something clean about the output directory
                    int ret;
                    ret = system((string("mkdir -p ")+outputFolder).c_str());
                    string epsOutput = outputFolder+"/"+shortPlotName+".eps";  
                    string pngOutput = outputFolder+"/"+shortPlotName+".png";  

                    // Save eps
                    // TODO move this line elsewhere
                    // (remove useless info message from root)
                    gErrorIgnoreLevel = kWarning;

                    string eraseBeforeCreationEps("rm -f "+epsOutput);
                    ret = system(eraseBeforeCreationEps.c_str());
                    theCanvas->SaveAs(epsOutput.c_str());

                    // Convert to png
                    string eraseBeforeCreationPng("rm -f "+pngOutput);
                    ret = system(eraseBeforeCreationPng.c_str());
                    string convertCommand("convert -density 130 "+epsOutput+" "+pngOutput+" 2> /dev/null");
                    ret = system(convertCommand.c_str());

                    // Remove "ret not used" warning
                    ret = ret + 1;
                }  
            };

            // Legend edition
            void AddToLegend(const TObject* obj, const char* label,Option_t* option)
            { theLegend->AddEntry(obj,label,option); };

            void PlaceLegend(TPad* pad, string place, float width, float height)
            {

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

                theLegend->SetX1(x_min);
                theLegend->SetY1(y_min);
                theLegend->SetX2(x_max);
                theLegend->SetY2(y_max);

            }

            void SetParameter(string field, string value) { parameters[field] = value; }
            string GetParameter(string field) { return parameters[field]; }

        private:

            TCanvas* theCanvas;
            TLegend* theLegend;
            TPaveText* theTopLeftInfo;
            TPaveText* theTopRightInfo;
            TPaveText* theInPlotInfo;
            vector<TPad*> thePads;

            string type;
            std::map<string,string> parameters;

    };

}

#endif
