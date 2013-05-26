#ifndef Histo3D_h
#define Histo3D_h

#include "interface/Common.h"
#include "interface/Variable.h"
#include "interface/ProcessClass.h"

namespace theDoctor
{

    class Histo3D 
    {
      
     public:

      Histo3D(Variable* theXVar_,
              Variable* theYVar_,
              Variable* theZVar_,
              ProcessClass* theProcessClass_,
              Region* theRegion_,
              Channel* theChannel_,
              bool autoFillEnabled_ = true,
              int nBinsX_ = -1, float minX_ = -1.0, float maxX_ = -1.0,
              int nBinsY_ = -1, float minY_ = -1.0, float maxY_ = -1.0,
              int nBinsZ_ = -1, float minZ_ = -1.0, float maxZ_ = -1.0)
      {
          theXVar = theXVar_;
          theYVar = theYVar_;
          theZVar = theZVar_;
          theProcessClass = theProcessClass_;
          theRegion       = theRegion_;
          theChannel      = theChannel_;
          autoFillEnabled = autoFillEnabled_;

          string nameHisto =  string("variableX:")+theXVar->getTag()
                                   +"|variableY:"+theYVar->getTag()
                                   +"|variableZ:"+theZVar->getTag()
                                   +"|processclass:"+theProcessClass->getTag()
                                   +"|region:"+theRegion->getTag()
                                   +"|channel:"+theChannel->getTag();
              
          nBinsX = nBinsX_;  minX = minX_;  maxX = maxX_;
          nBinsY = nBinsY_;  minY = minY_;  maxY = maxY_;
          nBinsZ = nBinsZ_;  minZ = minZ_;  maxZ = maxZ_;

          string titleHisto = "";

          if (nBinsX == -1)
          {
            theHisto = new TH3F(nameHisto.c_str(),titleHisto.c_str(),
                                theXVar->getNbins(),theXVar->getMin(),theXVar->getMax(),
                                theYVar->getNbins(),theYVar->getMin(),theYVar->getMax(),
                                theZVar->getNbins(),theZVar->getMin(),theZVar->getMax());
            theBinnedNumberOfEntries = new TH3F((nameHisto+"|entries").c_str(),
                                                titleHisto.c_str(),
                                                theXVar->getNbins(),theXVar->getMin(),theXVar->getMax(),
                                                theYVar->getNbins(),theYVar->getMin(),theYVar->getMax(),
                                                theZVar->getNbins(),theZVar->getMin(),theZVar->getMax());
          }
          else
          {
            theHisto = new TH3F((nameHisto+"|entries").c_str(),
                                titleHisto.c_str(),
                                nBinsX,minX,maxX,
                                nBinsY,minY,maxY,
                                nBinsZ,minZ,maxZ);
            theBinnedNumberOfEntries = new TH3F((nameHisto+"|entries").c_str(),
                                                titleHisto.c_str(),
                                                nBinsX,minX,maxX,
                                                nBinsY,minY,maxY,
                                                nBinsZ,minZ,maxZ);
          }
      }; 

      ~Histo3D() { };

      // Accessors
      TH3F* 		getHisto()			 { return theHisto;                   };
      TH3F* 		getEntriesHisto()	 { return theBinnedNumberOfEntries;   };
      ProcessClass* getProcessClass()	 { return theProcessClass;            };
      string		getProcessClassTag() { return theProcessClass->getTag();  };
      Variable* 	getVariableX()		 { return theXVar; 		    	      };
      string 		getVariableXTag()	 { return theXVar->getTag();          };
      Variable* 	getVariableY()		 { return theYVar; 		 	          };
      string 		getVariableYTag()	 { return theYVar->getTag();          };
      Variable* 	getVariableZ()		 { return theZVar; 		 	          };
      string 		getVariableZTag()	 { return theZVar->getTag();          };
      Region* 	    getRegion()		     { return theRegion; 		          };
      string 		getRegionTag()   	 { return theRegion->getTag();        };
      Channel*      getChannel()		 { return theChannel; 		          };
      string 	 	getChannelTag() 	 { return theChannel->getTag();       };
      bool			doAutoFill()		 { return autoFillEnabled; 	          };  
      
      TH3F*         getClone()           { return (TH3F*) theHisto->Clone();                  };
      TH3F*         getEntriesClone()    { return (TH3F*) theBinnedNumberOfEntries->Clone();  };
      
      // Editors
      void AutoFill(float weight = 1.0)
      {
          Fill(theXVar->getAutoFillValue(),
               theYVar->getAutoFillValue(),
               theZVar->getAutoFillValue(),
               weight);
      }
          
      void Fill(float valueX = 1.0, float valueY = 1.0, float valueZ = 1.0, float weight = 1.0)
      {
          theHisto->Fill(valueX,valueY,valueZ,weight);
          theBinnedNumberOfEntries->Fill(valueX,valueY,valueZ,weight);
      }

     private:

        TH3F*     theHisto;
        TH3F*     theBinnedNumberOfEntries;
        Variable* theXVar;
        Variable* theYVar;
        Variable* theZVar;
        ProcessClass* theProcessClass;
        Region*   theRegion;
        Channel*  theChannel;
        bool autoFillEnabled;

        int nBinsX;  float minX;  float maxX;
        int nBinsY;  float minY;  float maxY;
        int nBinsZ;  float minZ;  float maxZ;
        
    };

}

#endif
