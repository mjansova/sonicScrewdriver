#ifndef Region_h
#define Region_h

#include "Name.h" 

namespace theDoctor
{

    class Region : public Name 
    {
      
     public:
     
      Region(string tag_, string plotLabel_, bool (*selector_)(), string options_ = ""):
      Name(tag_,plotLabel_, options_)
      {
            selector = selector_;
            flag = false;
      }; 

      ~Region() 
      { 
      };

      bool passSelection()       { return selector(); };
      bool getSelectionFlag()    { return flag;       };
      void updateSelectionFlag() { flag = selector(); };

      // Accessors

     private:

      bool (*selector)();
      bool flag;

    };

}

#endif
