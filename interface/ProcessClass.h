#ifndef ProcessClass_h
#define ProcessClass_h

#include "Name.h"

namespace theDoctor
{

    class ProcessClass : public Name
    {

     public:

      ProcessClass(string tag_, string plotLabel_, string type_, Color_t color_, string options_ = ""):
      Name(tag_,plotLabel_, options_)
      {
          type = type_;
          color = color_;
      };

      ~ProcessClass() { };

      string getType()  { return type; };
      Color_t getColor() { return color; };

     private:

      string type;
      Color_t color;

    };

}

#endif
