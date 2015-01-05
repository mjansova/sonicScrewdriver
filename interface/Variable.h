#ifndef Variable_h
#define Variable_h

#include "Name.h"

namespace theDoctor
{

    class Variable : public Name
    {

     public:

      // Float variable, standard binning
      Variable(string tag_, string plotLabel_, string unit_, int nBins_, float min_, float max_, float* autoFillPointer_ = 0, string options_ = ""):
      Name(tag_,plotLabel_, options_)
      {
          unit = unit_;
          autoFillPointerFloat  = autoFillPointer_;
          autoFillPointerDouble = 0;
          autoFillPointerInt    = 0;

          // Binning definition
          nBins = nBins_;
          min = min_;
          max = max_;
          customBinning = 0;
      };

      // Float variable, custom binning
      Variable(string tag_, string plotLabel_, string unit_, int nBins_, float* customBinning_, float* autoFillPointer_ = 0, string options_ = ""):
      Name(tag_,plotLabel_, options_)
      {
          unit = unit_;
          autoFillPointerFloat  = autoFillPointer_;
          autoFillPointerDouble = 0;
          autoFillPointerInt    = 0;

          // Binning definition
          nBins = nBins_;
          if (customBinning_ == 0)
          {
              WARNING_MSG << "Trying to use a custom binning that has not been defined - null pointer -  (variable " << getTag() << ")" << endl;
              return;
          }

          min = customBinning_[0];
          max = customBinning_[nBins];
          customBinning = new double[nBins+1];
          for (int i = 0 ; i <= nBins ; i++)
          {
              customBinning[i] = customBinning_[i];
          }
      };

      // Double variable, standard binning
      Variable(string tag_, string plotLabel_, string unit_, int nBins_, float min_, float max_, double* autoFillPointer_ = 0, string options_ = ""):
      Name(tag_,plotLabel_, options_)
      {
          unit = unit_;
          autoFillPointerFloat  = 0;
          autoFillPointerDouble = autoFillPointer_;
          autoFillPointerInt    = 0;

          // Binning definition
          nBins = nBins_;
          min = min_;
          max = max_;
          customBinning = 0;
      };

      // Double variable, custom binning
      Variable(string tag_, string plotLabel_, string unit_, int nBins_, float* customBinning_, double* autoFillPointer_ = 0, string options_ = ""):
      Name(tag_,plotLabel_, options_)
      {
          unit = unit_;
          autoFillPointerFloat  = 0;
          autoFillPointerDouble = autoFillPointer_;
          autoFillPointerInt    = 0;

          // Binning definition
          nBins = nBins_;
          if (customBinning_ == 0)
          {
              WARNING_MSG << "Trying to use a custom binning that has not been defined - null pointer -  (variable " << getTag() << ")" << endl;
              return;
          }

          min = customBinning_[0];
          max = customBinning_[nBins];
          customBinning = new double[nBins+1];
          for (int i = 0 ; i <= nBins ; i++)
          {
              customBinning[i] = customBinning_[i];
          }
      };

      // Int variable, standard customBinning
      Variable(string tag_, string plotLabel_, string unit_, int nBins_, int min_, int max_, int* autoFillPointer_ = 0, string options_ = ""):
      Name(tag_,plotLabel_, options_)
      {
          unit = unit_;
          autoFillPointerInt    = autoFillPointer_;
          autoFillPointerDouble = 0;
          autoFillPointerFloat  = 0;

          // Binning definition
          nBins = nBins_;
          min = min_ - 0.5;
          max = max_ + 0.5;
          customBinning = 0;
      };

      // Int variable, custom customBinning
      Variable(string tag_, string plotLabel_, string unit_, int nBins_, float* customBinning_, int* autoFillPointer_ = 0, string options_ = ""):
      Name(tag_,plotLabel_, options_)
      {
          unit = unit_;
          autoFillPointerInt    = autoFillPointer_;
          autoFillPointerDouble = 0;
          autoFillPointerFloat  = 0;

          // Binning definition
          nBins = nBins_;
          if (customBinning_ == 0)
          {
              WARNING_MSG << "Trying to use a custom binning that has not been defined - null pointer -  (variable " << getTag() << ")" << endl;
              return;
          }

          min = customBinning_[0];
          max = customBinning_[nBins];
          customBinning = new double[nBins+1];
          for (int i = 0 ; i <= nBins ; i++) customBinning[i] = customBinning_[i];
      };

      ~Variable() { };

      // Accessors
      float getAutoFillValue()
      {
          if ((autoFillPointerFloat == 0) && (autoFillPointerDouble == 0) && (autoFillPointerInt == 0))
          {
              WARNING_MSG << "Trying to access autoFillValue with null pointer (variable " << getTag() << ")" << endl;
              return -1.0;
          }
          else if (autoFillPointerFloat  != 0) return *(autoFillPointerFloat);
          else if (autoFillPointerDouble != 0) return ((float) *(autoFillPointerDouble));
          else                                 return ((float) *(autoFillPointerInt));

      };

      string getUnit()            { return unit;    };
      int    getNbins()           { return nBins;   };
      float  getMin()             { return min;     };
      float  getMax()             { return max;     };
      float  getBinWidth()        { return (max - min) / (float) nBins; };

      double* getCustomBinning()   { return customBinning;        };
      bool    usingCustomBinning() { return (customBinning != 0); };

     private:

      string unit;

      float*  autoFillPointerFloat;
      double* autoFillPointerDouble;
      int*    autoFillPointerInt;

      int    nBins;
      float  min;
      float  max;

      double* customBinning;

    };

}

#endif
