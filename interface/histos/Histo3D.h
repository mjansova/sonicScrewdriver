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

      Histo3D(Name        theHistoType_,
              Variable*   theXVar_,
              Variable*   theYVar_,
              Variable*   theZVar_,
              Region*     theRegion_,
              Channel*    theChannel_,
              string      theHistoParameters_ = "") :
      theHistoType(theHistoType_)
      {
          theXVar    = theXVar_;
          theYVar    = theYVar_;
          theZVar    = theZVar_;
          theRegion  = theRegion_;
          theChannel = theChannel_;

          theHistoParameters = theHistoParameters_;

          if(!theXVar_->usingCustomBinning() && !theYVar_->usingCustomBinning() && !theZVar_->usingCustomBinning())
              theHisto = new TH3D("","",
                                  theXVar->getNbins(),theXVar->getMin(),theXVar->getMax(),
                                  theYVar->getNbins(),theYVar->getMin(),theYVar->getMax(),
                                  theZVar->getNbins(),theZVar->getMin(),theZVar->getMax());
          else if (theXVar_->usingCustomBinning() && theYVar_->usingCustomBinning() && theZVar_->usingCustomBinning())
              theHisto = new TH3D("","",
                                  theXVar->getNbins(),theXVar->getCustomBinning(),
                                  theYVar->getNbins(),theYVar->getCustomBinning(),
                                  theZVar->getNbins(),theZVar->getCustomBinning());
          else
          {
              WARNING_MSG << "Cannot create 3D histograms with variable using different kind of binning (normal/custom)."
                          << "(varX, varY, varZ) = (" << theXVar->getTag() << "," << theYVar->getTag() << "," << theZVar->getTag() << ")."
                          << "Root doesn't allow it." << endl;
          }
          theHisto->Sumw2();
      };

      ~Histo3D() { };

      // Accessors
      TH3D*     getHisto()           { return theHisto;           };      TH3D*   getClone()        { return (TH3D*) theHisto->Clone(); };
      Variable* getVariableX()       { return theXVar;            };      string  getVariableXTag() { return theXVar->getTag();         };
      Variable* getVariableY()       { return theYVar;            };      string  getVariableYTag() { return theYVar->getTag();         };
      Variable* getVariableZ()       { return theZVar;            };      string  getVariableZTag() { return theZVar->getTag();         };
      Region*   getRegion()          { return theRegion;          };      string  getRegionTag()    { return theRegion->getTag();       };
      Channel*  getChannel()         { return theChannel;         };      string  getChannelTag()   { return theChannel->getTag();      };

      Name      getHistoType()       { return theHistoType;       };      string  getHistoTypeTag() { return theHistoType.getTag();     };
      string    getHistoParameters() { return theHistoParameters; };

     protected:

        TH3D*     theHisto;

        Variable* theXVar;
        Variable* theYVar;
        Variable* theZVar;
        Region*   theRegion;
        Channel*  theChannel;

        Name      theHistoType;
        string    theHistoParameters;

    };

}

#endif
