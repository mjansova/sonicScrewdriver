#ifndef Histo1D_h
#define Histo1D_h

#include "interface/Common.h"
#include "interface/Variable.h"
#include "interface/Channel.h"
#include "interface/Region.h"
#include "interface/ProcessClass.h"
#include "interface/Figure.h"

namespace theDoctor
{

    class Histo1D 
    {
      
     public:

      Histo1D(Variable* theVar_, ProcessClass* theProcessClass_, Region* theRegion_, Channel* theChannel_, 
                   bool autoFillEnabled_ = true, int nBins = -1, float min = -1.0, float max = -1.0)
      {
          theVar          = theVar_;
          theProcessClass = theProcessClass_;
          theRegion       = theRegion_;
          theChannel      = theChannel_;

          autoFillEnabled = autoFillEnabled_;
          
          string nameHisto =  string("variable:")+theVar->getTag()
                                   +"|processclass:"+theProcessClass->getTag()
                                   +"|region:"+theRegion->getTag()
                                   +"|channel:"+theChannel->getTag();

          string titleHisto = "";

          if (nBins == -1)
          {
            theHisto                 = new TH1F(nameHisto.c_str(),titleHisto.c_str(),theVar->getNbins(),theVar->getMin(),theVar->getMax());
            theBinnedNumberOfEntries = new TH1F((nameHisto+"|entries").c_str(),titleHisto.c_str(),theVar->getNbins(),theVar->getMin(),theVar->getMax());
          }
          else
          {
            theHisto                 = new TH1F(nameHisto.c_str(),titleHisto.c_str(),nBins,min,max);
            theBinnedNumberOfEntries = new TH1F((nameHisto+"|entries").c_str(),titleHisto.c_str(),nBins,min,max);
          }

          theHisto->Sumw2();
          theBinnedNumberOfEntries->Sumw2();
      }; 

      ~Histo1D() { };

      // Accessors
      TH1F* 		 getHisto()            { return theHisto;	 	 	        };
      TH1F* 		 getEntriesHisto()     { return theBinnedNumberOfEntries;   };
      
      ProcessClass*  getProcessClass()     { return theProcessClass;  	 	    };
      string  		 getProcessClassTag()  { return theProcessClass->getTag();  };
      Variable*      getVariable()		   { return theVar; 		 			};
      string 		 getVariableTag()	   { return theVar->getTag(); 			};
      Region*        getRegion()		   { return theRegion; 		 		    };
      string 		 getRegionTag()	       { return theRegion->getTag(); 		};
      Channel*       getChannel()		   { return theChannel; 		 		};
      string 		 getChannelTag()	   { return theChannel->getTag(); 		};

      bool			 doAutoFill()		   { return autoFillEnabled; 	        };

      TH1F*          getClone()            { return (TH1F*) theHisto->Clone();  };
      TH1F*          getEntriesClone()     { return (TH1F*) theBinnedNumberOfEntries->Clone();  };

      Figure getYieldAndError()
      {
          double error;
          double yield = theHisto->IntegralAndError(0,theHisto->GetNbinsX()+1,error);

          return Figure(yield,error);
      }

      // Editors
      void AutoFill(float weight = 1.0)
      {
          Fill(theVar->getAutoFillValue(),weight);
      }
          
      void Fill(float value = 1.0, float weight = 1.0)
      {
          theHisto->Fill(value,weight);
          theBinnedNumberOfEntries->Fill(value);
      }
      
      void ApplyScaleFactor(Figure scaleFactor)
      { 
            TH1F histoScaleFactor("histoScaleFactor","",theVar->getNbins(),theVar->getMin(),theVar->getMax());
        
            for (int i = 0 ; i <= histoScaleFactor.GetNbinsX()+1 ; i++)
            {
                histoScaleFactor.SetBinContent(i,scaleFactor.value());
                histoScaleFactor.SetBinError(i,scaleFactor.error());
            }

            theHisto->Multiply(&histoScaleFactor);
      }

     private:

      TH1F* theHisto;
      TH1F* theBinnedNumberOfEntries;
      Variable*     theVar;
      ProcessClass* theProcessClass;
      Region*       theRegion;
      Channel*      theChannel;
      bool autoFillEnabled;

    };

}

#endif
