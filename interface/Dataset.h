#ifndef Dataset_h
#define Dataset_h

#include "Name.h"

namespace theDoctor
{

    class Dataset : public Name
    {

     public:

      // Please note : processClass is stored in the "plotLabel" string of Name,
      //               though it is not meant to be used for plotting it or whatever anyway.

      Dataset(string tag_, string processClass_, int trueNumberOfEvents_, float xsecOrLumi_, string options_ = ""):
      Name(tag_,processClass_)
      {
          xsecOrLumi = xsecOrLumi_;
          trueNumberOfEvents = trueNumberOfEvents_;
          options = options_;
      };

      ~Dataset() { };

      string getProcessClass()            { return getLabel();         };
      float  getXsecOrLumi()              { return xsecOrLumi;         };
      int    getTrueNumberOfEvents()      { return trueNumberOfEvents; };
      void   setTrueNumberOfEvents(int n) { trueNumberOfEvents = n;    };
      string getOptions()                 { return options;            };

     private:

      float xsecOrLumi;
      int trueNumberOfEvents;

      string options;

    };

}

#endif
