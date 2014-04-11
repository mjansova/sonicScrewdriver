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
          autoFillPointerFloat = autoFillPointer_;
          autoFillPointerInt = 0;
          nBins = nBins_;
          min = min_;
          max = max_;
      }; 

      Variable(string tag_, string plotLabel_, string unit_, int nBins_, int min_, int max_, int* autoFillPointer_ = 0, string options_ = ""):
      Name(tag_,plotLabel_, options_)
      {
          unit = unit_;
          autoFillPointerInt = autoFillPointer_;
          autoFillPointerFloat = 0;
          nBins = nBins_;
          min = min_ - 0.5;
          max = max_ + 0.5;
      }; 


      ~Variable() { };

      // Accessors
      float getAutoFillValue()  
      {
          if ((autoFillPointerFloat == 0) && (autoFillPointerInt == 0))
          {
              WARNING_MSG << "Trying to access autoFillValue with null pointer (variable " << getTag() << ")" << endl;
              return -1.0;
          }
          if (autoFillPointerFloat != 0) return *(autoFillPointerFloat); 
          if (autoFillPointerInt   != 0) return ((int) *(autoFillPointerInt)); 

      };

      string getUnit() 	  { return unit;  }; 
      int getNbins() 	  { return nBins; }; 
      float getMin() 	  { return min; 	}; 
      float getMax() 	  { return max; 	}; 
      float getBinWidth() { return (max - min) / (float) nBins; }

     private:

      string unit;

      float* autoFillPointerFloat;
      int* autoFillPointerInt;

      int nBins;
      float min;
      float max;
      
    };

}

#endif
