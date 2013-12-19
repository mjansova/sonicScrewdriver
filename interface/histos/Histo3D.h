#ifndef Histo3D_h
#define Histo3D_h

#include "interface/Common.h"
#include "interface/Variable.h"
#include "interface/Region.h"
#include "interface/Channel.h"

namespace theDoctor
{

    class Histo3D 
    {
      
     public:

      Histo3D(Variable*   theXVar_,
              Variable*   theYVar_,
              Variable*   theZVar_,
              Region*     theRegion_,
              Channel*    theChannel_,
              Name        theHistoType_) :
      theHistoType(theHistoType_)
      {
          theXVar = theXVar_;
          theYVar = theYVar_;
          theYVar = theZVar_;
          theRegion       = theRegion_;
          theChannel      = theChannel_;

          theHisto          = new TH3F("","",
                                       theXVar->getNbins(),theXVar->getMin(),theXVar->getMax(),
                                       theYVar->getNbins(),theYVar->getMin(),theYVar->getMax(),
                                       theZVar->getNbins(),theZVar->getMin(),theZVar->getMax());

          theHisto->Sumw2();
      }; 

      ~Histo3D() { };

      // Accessors
      TH3F*         getHisto()           { return theHisto;                  };
      TH3F*         getClone()           { return (TH3F*) theHisto->Clone(); };
      Variable*     getVariableX()       { return theXVar;                   };
      string        getVariableXTag()    { return theXVar->getTag();         };
      Variable*     getVariableY()       { return theYVar;                   };
      string        getVariableYTag()    { return theYVar->getTag();         };
      Variable*     getVariableZ()       { return theZVar;                   };
      string        getVariableZTag()    { return theZVar->getTag();         };
      Region*       getRegion()          { return theRegion;                 };
      string        getRegionTag()       { return theRegion->getTag();       };
      Channel*      getChannel()         { return theChannel;                };
      string        getChannelTag()      { return theChannel->getTag();      };
      
     protected:

        TH3F*     theHisto;
        Variable* theXVar;
        Variable* theYVar;
        Variable* theZVar;
        Region*   theRegion;
        Channel*  theChannel;
        Name      theHistoType;

    };

}

#endif