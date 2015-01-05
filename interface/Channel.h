#ifndef Channel_h
#define Channel_h

#include "Name.h"

namespace theDoctor
{

    class Channel : public Name
    {

     public:

      Channel(string tag_, string plotLabel_, bool (*selector_)(), string options_ = ""):
      Name(tag_,plotLabel_, options_)
      {
            selector = selector_;
            flag = false;
      };

      ~Channel()
      {
      };

      bool passSelection()       { return selector(); };
      bool getSelectionFlag()    { return flag;       };
      void updateSelectionFlag() { flag = selector(); };

      // Accessors

     private:

      bool (*selector)() ;
      bool flag;


    };

}

#endif
