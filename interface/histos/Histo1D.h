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

      Histo1D(Name      theHistoType_,
              Variable* theVar_, 
              Region*   theRegion_, 
              Channel*  theChannel_,
              string    theHistoParameters_ = "") :
      theHistoType(theHistoType_)
      {
          theVar             = theVar_;
          theRegion          = theRegion_;
          theChannel         = theChannel_;
          theHistoParameters = theHistoParameters_;
          
          if (theVar_->usingCustomBinning())
              theHisto        = new TH1F("","",theVar->getNbins(),theVar->getCustomBinning());
          else
              theHisto        = new TH1F("","",theVar->getNbins(),theVar->getMin(),theVar->getMax());
          
          theHisto->Sumw2();
      }; 

      virtual ~Histo1D() { };

      // Accessors
      TH1F*     getHisto()           const { return theHisto;                  };
      TH1F*     getClone()           const { return (TH1F*) theHisto->Clone(); };
      
      Variable* getVariable()        const { return theVar;             };  string  getVariableTag()  const { return theVar->getTag();      };
      Region*   getRegion()          const { return theRegion;          };  string  getRegionTag()    const { return theRegion->getTag();   };
      Channel*  getChannel()         const { return theChannel;         };  string  getChannelTag()   const { return theChannel->getTag();  };
      Name      getHistoType()       const { return theHistoType;       };  string  getHistoTypeTag() const { return theHistoType.getTag(); };
      string    getHistoParameters() const { return theHistoParameters; };

      void dump() const
      {
          cout 
          << "theHisto = "           <<   theHisto            << endl
          << "theVar = "             <<   theVar              << endl
          << "theRegion = "          <<   theRegion           << endl
          << "theChannel = "         <<   theChannel          << endl
          << "theHistoParameters = " <<   theHistoParameters  << endl;

      }

     protected:

      TH1F*         theHisto;

      Variable*     theVar;
      Region*       theRegion;
      Channel*      theChannel;

      Name          theHistoType;
      string        theHistoParameters;

    };

}

#endif
