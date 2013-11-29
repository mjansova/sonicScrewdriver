#ifndef Variable_h
#define Variable_h

#include "Name.h" 

namespace theDoctor 
{

    class Variable : public Name 
    {
      
     public:
     
      Variable(string tag_, string plotLabel_, string unit_, int nBins_, float min_, float max_, float* autoFillPointer_ = 0, string options_ = ""):
      Name(tag_,plotLabel_, options_)
      {
          unit = unit_;
          autoFillPointer = autoFillPointer_;
          nBins = nBins_;
          min = min_;
          max = max_;
      }; 

      ~Variable() { };

      // Accessors
      float getAutoFillValue()  
      { 
          if (autoFillPointer == 0)
          {
              WARNING_MSG << "Trying to access autoFillValue with null pointer (variable " << getTag() << ")" << endl;
              return -1.0;
          }
          else return *(autoFillPointer); 
      };

      string getUnit() 	{ return unit; }; 
      int getNbins() 	{ return nBins; }; 
      float getMin() 	{ return min; 	}; 
      float getMax() 	{ return max; 	}; 

     private:

      string unit;

      float* autoFillPointer;

      int nBins;
      float min;
      float max;
      
    };

}

#endif
