#ifndef Histo2D_h
#define Histo2D_h

#include "interface/Common.h"
#include "interface/Variable.h"
#include "interface/Region.h"
#include "interface/Channel.h"

namespace theDoctor
{

    class Histo2D
    {

     public:

      Histo2D(Name        theHistoType_,
              Variable*   theXVar_,
              Variable*   theYVar_,
              Region*     theRegion_,
              Channel*    theChannel_,
              string      theHistoParameters_ = "") :
      theHistoType(theHistoType_)
      {
          theXVar = theXVar_;
          theYVar = theYVar_;
          theRegion       = theRegion_;
          theChannel      = theChannel_;

          theHistoParameters = theHistoParameters_;

          if (theXVar_->usingCustomBinning() && theYVar_->usingCustomBinning())
              theHisto = new TH2D("","",
                                  theXVar->getNbins(),theXVar->getCustomBinning(),
                                  theYVar->getNbins(),theYVar->getCustomBinning());

          else if (theXVar_->usingCustomBinning())
              theHisto = new TH2D("","",
                                  theXVar->getNbins(),theXVar->getCustomBinning(),
                                  theYVar->getNbins(),theYVar->getMin(),theYVar->getMax());
          else if (theXVar_->usingCustomBinning())
              theHisto = new TH2D("","",
                                  theXVar->getNbins(),theXVar->getMin(),theXVar->getMax(),
                                  theYVar->getNbins(),theYVar->getCustomBinning());
          else
              theHisto = new TH2D("","",
                                  theXVar->getNbins(),theXVar->getMin(),theXVar->getMax(),
                                  theYVar->getNbins(),theYVar->getMin(),theYVar->getMax());


          theHisto->Sumw2();
      };

      ~Histo2D() { };

      // Accessors
      TH2D*     getHisto()           { return theHisto;           };      TH2D*   getClone()        { return (TH2D*) theHisto->Clone(); };
      Variable* getVariableX()       { return theXVar;            };      string  getVariableXTag() { return theXVar->getTag();         };
      Variable* getVariableY()       { return theYVar;            };      string  getVariableYTag() { return theYVar->getTag();         };
      Region*   getRegion()          { return theRegion;          };      string  getRegionTag()    { return theRegion->getTag();       };
      Channel*  getChannel()         { return theChannel;         };      string  getChannelTag()   { return theChannel->getTag();      };

      Name      getHistoType()       { return theHistoType;       };      string  getHistoTypeTag() { return theHistoType.getTag();     };
      string    getHistoParameters() { return theHistoParameters; };

     protected:

        TH2D*     theHisto;

        Variable* theXVar;
        Variable* theYVar;
        Region*   theRegion;
        Channel*  theChannel;

        Name      theHistoType;
        string    theHistoParameters;

    };

}

#endif
