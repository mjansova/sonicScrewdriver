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
      Plot(string name_, string type_, string options = "")
      {
          theCanvas = new TCanvas(name_.c_str(),"",850,750);
          theLegend = new TLegend(0.65,0.70,0.89,0.89);
          theTopInfo = new TPaveText(0.1,0.915,0.8,0.95,"NDC");
      
          PlaceLegend("topright",0.2,0.3);

          PlotDefaultStyles::ApplyDefaultCanvasStyle(theCanvas);
          PlotDefaultStyles::ApplyDefaultLegendStyle(theLegend);
          PlotDefaultStyles::ApplyDefaultPaveTextStyle(theTopInfo);

          type = type_;
      
          SetActive();
      };

      ~Plot() { };

      // Accessors
      string getType()     { return type; 		};
      TCanvas* getCanvas() { return theCanvas; 	};
      TLegend* getLegend() { return theLegend; 	}; 

      string infoFromCanvasName(string field)
      {
        string fieldTest;
        stringstream stream(theCanvas->GetName());
        while( getline(stream,fieldTest,'|') )
        {
            stringstream stream2(fieldTest);
            string name; getline(stream2,name,':');
            
            if (name != field) continue;
            
            string value; getline(stream2,value,':');
            return value;
        }

        return "";

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

      void SetActive()	   { theCanvas->cd(); 	};
     
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
      
      void Write(string outputFolder = "", string infoText = "", string options = "")
      {
        
        // Draw the legend and top info before writing

        SetActive();
        if (thePads.size() != 0) thePads[0]->cd();

        theLegend->Draw();
        AddToTopInfo(infoText);
        theTopInfo->Draw();
     
        if ((type == "2D") || (type == "2DProjectedTo1D"))
        {
            string processClassName = infoFromCanvasName("processClass");
            theCanvas->SetName(processClassName.c_str());
        }
        else if  (type == "3DProjectedTo2D")
        {
            string processClassName = infoFromCanvasName("processClass");
            string varZ = infoFromCanvasName("varZ");
            theCanvas->SetName((varZ+"|"+processClassName).c_str());
        }
        else
        {
            string varName = infoFromCanvasName("var");
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

      // Info text edition
      void AddToTopInfo(string text)
      { theTopInfo->AddText(text.c_str()); };

      // Legend edition
      void AddToLegend(const TObject* obj, const char* label,Option_t* option)
      { theLegend->AddEntry(obj,label,option); };

      void PlaceLegend(string place, float width, float height)
      {
          float abs_right = 0.875;
          float abs_left = 0.125;
          float abs_top = 0.875;
          float abs_bot = 0.125;

          float x_min = 0.0;
          float y_min = 0.0;
          float x_max = 0.0;
          float y_max = 0.0;

               if ((place == string("topleft"))   || (place == string("tl")))
          {
                x_min = abs_left;				x_max = abs_left + width;
                y_min = abs_top - height;		y_max = abs_top;
          }
          else if ((place == string("topright"))  || (place == string("tr")))
          {
                x_min = abs_right - width;		x_max = abs_right;
                y_min = abs_top - height;		y_max = abs_top;
          }
          else if ((place == string("topcenter")) || (place == string("tc")))
          {
                x_min = 0.5 - width/2.0;		x_max = 0.5 + width/2.0;
                y_min = abs_top - height;		y_max = abs_top;
          }
          else if ((place == string("botleft"))   || (place == string("bl")))
          {
                x_min = abs_left;				x_max = abs_left + width;
                y_min = abs_bot;				y_max = abs_bot + height;
          }
          else if ((place == string("botright"))  || (place == string("br")))
          {
                x_min = abs_right - width;		x_max = abs_right;
                y_min = abs_top;				y_max = abs_bot + height;
          }
          else if ((place == string("botcenter")) || (place == string("bc")))
          {
                x_min = 0.5 - width/2.0;		x_max = 0.5 + width/2.0;
                y_min = abs_bot;				y_max = abs_bot + height;
          }
        
          theLegend->SetX1(x_min);
          theLegend->SetY1(y_min);
          theLegend->SetX2(x_max);
          theLegend->SetY2(y_max);

      }

     private:

      TCanvas* theCanvas;	
      TLegend* theLegend;
      TPaveText* theTopInfo;
      vector<TPad*> thePads;

      string type;

    };

}

#endif
