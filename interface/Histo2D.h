#ifndef Histo2D_h
#define Histo2D_h

#include "interface/Common.h"
#include "interface/Variable.h"
#include "interface/ProcessClass.h"

namespace theDoctor
{

    class Histo2D 
    {
      
     public:

    /*	Histo2D newHisto(
                              &((*theVariables)[indexVarX]),
                              &((*theVariables)[indexVarY]),
                              &((*theProcessClasses)[i]),
                              &((*theRegions)[r]),
                              &((*theChannels)[c]),
                              autoFill,
                              nBinsX,minX,maxX,
                              nBinsY,minY,maxY);
    */

      Histo2D(Variable* theXVar_,
                   Variable* theYVar_,
                   ProcessClass* theProcessClass_,
                   Region* theRegion_,
                   Channel* theChannel_,
                   bool autoFillEnabled_ = true,
                   int nBinsX = -1, float minX = -1.0, float maxX = -1.0,
                   int nBinsY = -1, float minY = -1.0, float maxY = -1.0)
      {
          theXVar = theXVar_;
          theYVar = theYVar_;
          theProcessClass = theProcessClass_;
          theRegion       = theRegion_;
          theChannel      = theChannel_;
          autoFillEnabled = autoFillEnabled_;

          string nameHisto =  string("variableX:")+theXVar->getTag()
                                   +"|variableY:"+theYVar->getTag()
                                   +"|processclass:"+theProcessClass->getTag()
                                   +"|region:"+theRegion->getTag()
                                   +"|channel:"+theChannel->getTag();

          string titleHisto = "";

          if (nBinsX == -1)
          {
            theHisto                 = new TH2F(nameHisto.c_str(),titleHisto.c_str(),
                                                theXVar->getNbins(),theXVar->getMin(),theXVar->getMax(),
                                                theYVar->getNbins(),theYVar->getMin(),theYVar->getMax());
            theBinnedNumberOfEntries = new TH2F((nameHisto+"|entries").c_str(),titleHisto.c_str(),
                                                theXVar->getNbins(),theXVar->getMin(),theXVar->getMax(),
                                                theYVar->getNbins(),theYVar->getMin(),theYVar->getMax());
          }
          else
          {
            theHisto                 = new TH2F(nameHisto.c_str(),titleHisto.c_str(),
                                                nBinsX,minX,maxX,
                                                nBinsY,minY,maxY);
            theBinnedNumberOfEntries = new TH2F((nameHisto+"|entries").c_str(),titleHisto.c_str(),
                                                nBinsX,minX,maxX,
                                                nBinsY,minY,maxY);
          }

          theHisto->Sumw2();
          theBinnedNumberOfEntries->Sumw2();
      }; 

      ~Histo2D() { };

      // Accessors
      TH2F* 		getHisto()			 { return theHisto;	 	 	         };
      TH2F* 		getEntriesHisto()	 { return theBinnedNumberOfEntries;  };
      ProcessClass* getProcessClass()	 { return theProcessClass;  	     };
      string		getProcessClassTag() { return theProcessClass->getTag(); };
      Variable* 	getVariableX()		 { return theXVar; 		    	};
      string 		getVariableXTag()	 { return theXVar->getTag();    };
      Variable* 	getVariableY()		 { return theYVar; 		 	    };
      string 		getVariableYTag()	 { return theYVar->getTag();    };
      Region* 	    getRegion()		     { return theRegion; 		    };
      string 		getRegionTag()   	 { return theRegion->getTag();  };
      Channel*      getChannel()		 { return theChannel; 		    };
      string 	 	getChannelTag() 	 { return theChannel->getTag(); };
      bool			doAutoFill()		 { return autoFillEnabled; 	    };  
      
      TH2F*         getClone()           { return (TH2F*) theHisto->Clone();                 };
      TH2F*         getEntriesClone()    { return (TH2F*) theBinnedNumberOfEntries->Clone(); };
      
      // Editors
      void AutoFill(float weight = 1.0)
      {
          Fill(theXVar->getAutoFillValue(),theYVar->getAutoFillValue(),weight);
      }
          
      void Fill(float valueX = 1.0, float valueY = 1.0, float weight = 1.0)
      {
          theHisto->Fill(valueX,valueY,weight);
          theBinnedNumberOfEntries->Fill(valueX,valueY);
      }

     private:

        TH2F* theHisto;
        TH2F* theBinnedNumberOfEntries;
        Variable* theXVar;
        Variable* theYVar;
        ProcessClass* theProcessClass;
        Region*  theRegion;
        Channel* theChannel;
        bool autoFillEnabled;

    };

}

#endif
