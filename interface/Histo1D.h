#ifndef Histo1D_h
#define Histo1D_h

#include "interface/Common.h"
#include "interface/Name.h"
#include "interface/Variable.h"
#include "interface/Channel.h"
#include "interface/Region.h"

namespace theDoctor
{

    class Histo1D 
    {
      
     public:

      Histo1D(Name theHistoType_,
              Variable* theVar_, 
              Region* theRegion_, 
              Channel* theChannel_,
              string theHistoParameters_ = "") :
      theHistoType(theHistoType_)
      {
          theVar          = theVar_;
          theRegion       = theRegion_;
          theChannel      = theChannel_;
          theHistoParameters = theHistoParameters_;
          
          theHisto        = new TH1F("","",theVar->getNbins(),theVar->getMin(),theVar->getMax());
          theHisto->Sumw2();
      }; 

      virtual ~Histo1D() { };

      // Accessors
      TH1F*     getHisto()           { return theHisto;                  };
      TH1F*     getClone()           { return (TH1F*) theHisto->Clone(); };
      
      Variable* getVariable()        { return theVar;             };  string  getVariableTag()  { return theVar->getTag();      };
      Region*   getRegion()          { return theRegion;          };  string  getRegionTag()    { return theRegion->getTag();   };
      Channel*  getChannel()         { return theChannel;         };  string  getChannelTag()   { return theChannel->getTag();  };
      Name      getHistoType()       { return theHistoType;       };  string  getHistoTypeTag() { return theHistoType.getTag(); };
      string    getHistoParameters() { return theHistoParameters; };

     protected:

      TH1F* theHisto;

      Variable*     theVar;
      Region*       theRegion;
      Channel*      theChannel;
      Name          theHistoType;
      
      string        theHistoParameters;

    };

}

#endif
