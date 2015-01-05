#ifndef Name_h
#define Name_h

#include "interface/Common.h"

using namespace std;

namespace theDoctor
{

    class Name
    {

     public:

      Name(string tag_, string plotLabel_, string options_ = "")
      {
          tag = tag_;
          plotLabel = plotLabel_;
          options = options_;
      };

      ~Name() { };

      string getTag()     const { return tag; };
      string getLabel()   const { return plotLabel; };
      string getOptions() const { return options; };

      const char* getTagC()   const { return tag.c_str(); };
      const char* getLabelC() const { return plotLabel.c_str(); };

     private:

        string tag;
        string plotLabel;
        string options;

    };

}

#endif
