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

      string getTag()   { return tag; };
      string getLabel() { return plotLabel; };
      string getOptions() { return options; };

      const char* getTagC()   { return tag.c_str(); };
      const char* getLabelC() { return plotLabel.c_str(); };
      
     private:

        string tag;	
        string plotLabel;
        string options;	

    };

}

#endif
