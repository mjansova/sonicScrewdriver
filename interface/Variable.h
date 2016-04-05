#ifndef Variable_h
#define Variable_h

#include <iterator>
#include <algorithm>
#include "Name.h"

namespace theDoctor
{

    class Variable : public Name
    {

     public:

      // ######################################################################

      // Float variable, standard binning
      Variable(string tag_, string plotLabel_, string unit_, int nBins_, float min_, float max_, float* autoFillPointer_ = 0, string options_ = ""):
      Name(tag_,plotLabel_, options_)
      {
          m_arrSize = 1;
          unit = unit_;
          mapPointers(autoFillPointer_,0,0,0,0);
          defineBinning(nBins_, min_, max_, 0);
      };

      // Float variable, custom binning
      Variable(string tag_, string plotLabel_, string unit_, int nBins_, float* customBinning_, float* autoFillPointer_ = 0, string options_ = ""):
      Name(tag_,plotLabel_, options_)
      {
          m_arrSize = 1;
          unit = unit_;

          mapPointers(autoFillPointer_,0,0,0,0);

          if (customBinning_ == 0) { WARNING_MSG << "Trying to use a custom binning that has not been defined - null pointer -  (variable " << getTag() << ")" << endl; return; }
          defineBinning(nBins_,customBinning_[0], customBinning_[nBins_], customBinning_);
      };

      // Float variable, standard binning, function
      Variable(string tag_, string plotLabel_, string unit_, int nBins_, float min_, float max_, float* (*autoFillFunction_)() = 0, string options_ = ""):
      Name(tag_,plotLabel_, options_)
      {
          m_arrSize = 1;
          unit = unit_;
          mapPointers(0,0,0,autoFillFunction_,0);
          defineBinning(nBins_, min_, max_, 0);
      };

      // Float variable, custom binning, function
      Variable(string tag_, string plotLabel_, string unit_, int nBins_, float* customBinning_, float* (*autoFillFunction_)() = 0, string options_ = ""):
      Name(tag_,plotLabel_, options_)
      {
          m_arrSize = 1;
          unit = unit_;

          mapPointers(0,0,0,autoFillFunction_,0);

          if (customBinning_ == 0) { WARNING_MSG << "Trying to use a custom binning that has not been defined - null pointer -  (variable " << getTag() << ")" << endl; return; }
          defineBinning(nBins_,customBinning_[0], customBinning_[nBins_], customBinning_);
      };

      // Double variable, standard binning
      Variable(string tag_, string plotLabel_, string unit_, int nBins_, float min_, float max_, double* autoFillPointer_ = 0, string options_ = ""):
      Name(tag_,plotLabel_, options_)
      {
          m_arrSize = 1;
          unit = unit_;

          mapPointers(0,autoFillPointer_,0,0,0);
          defineBinning(nBins_, min_, max_, 0);
      };

      // Double variable, custom binning
      Variable(string tag_, string plotLabel_, string unit_, int nBins_, float* customBinning_, double* autoFillPointer_ = 0, string options_ = ""):
      Name(tag_,plotLabel_, options_)
      {
          m_arrSize = 1;
          unit = unit_;

          mapPointers(0,autoFillPointer_,0,0,0);

          if (customBinning_ == 0) { WARNING_MSG << "Trying to use a custom binning that has not been defined - null pointer -  (variable " << getTag() << ")" << endl; return; }
          defineBinning(nBins_, customBinning_[0], customBinning_[nBins_], customBinning_);
      };

      // Int variable, standard binning
      Variable(string tag_, string plotLabel_, string unit_, int nBins_, int min_, int max_, int* autoFillPointer_ = 0, string options_ = ""):
      Name(tag_,plotLabel_, options_)
      {
          m_arrSize = 1;
          unit = unit_;

          mapPointers(0,0,autoFillPointer_,0,0);
          defineBinning(nBins_, min_ - 0.5, max_ + 0.5, 0);
      };

      // Int variable, custom customBinning
      Variable(string tag_, string plotLabel_, string unit_, int nBins_, float* customBinning_, int* autoFillPointer_ = 0, string options_ = ""):
      Name(tag_,plotLabel_, options_)
      {
          m_arrSize = 1;
          unit = unit_;

          mapPointers(0,0,autoFillPointer_,0,0);

          if (customBinning_ == 0) { WARNING_MSG << "Trying to use a custom binning that has not been defined - null pointer -  (variable " << getTag() << ")" << endl; return; }
          defineBinning(nBins_,customBinning_[0], customBinning_[nBins_], customBinning_);
      };

      // Int variable, standard binning, function
      Variable(string tag_, string plotLabel_, string unit_, int nBins_, int min_, int max_, int* (*autoFillFunction_)() = 0, string options_ = ""):
      Name(tag_,plotLabel_, options_)
      {
          m_arrSize = 1;
          unit = unit_;

          mapPointers(0,0,0,0,autoFillFunction_);
          defineBinning(nBins_, min_ - 0.5, max_ + 0.5, 0);
      };

      // Int variable, custom customBinning, function
      Variable(string tag_, string plotLabel_, string unit_, int nBins_, float* customBinning_, int* (*autoFillFunction_)() = 0, string options_ = ""):
      Name(tag_,plotLabel_, options_)
      {
          m_arrSize = 1;
          unit = unit_;

          mapPointers(0,0,0,0,autoFillFunction_);

          if (customBinning_ == 0) { WARNING_MSG << "Trying to use a custom binning that has not been defined - null pointer -  (variable " << getTag() << ")" << endl; return; }
          defineBinning(nBins_,customBinning_[0], customBinning_[nBins_], customBinning_);
      };
      // ######################################################################

      ~Variable() {};

      // Accessors
      double* getAutoFillValue()//@MJ@ TODO returns pointer now!!
      {
          if ((autoFillPointerFloat == 0) && (autoFillPointerDouble == 0) && (autoFillPointerInt == 0) && (autoFillFunctionFloat == 0) && (autoFillFunctionInt == 0))
          {
              WARNING_MSG << "Trying to access autoFillValue with null pointer (variable " << getTag() << ")" << endl;
              return NULL;
          }
          else if (autoFillPointerFloat  != 0)
          {
              float arrFloat[m_arrSize];
              double arrDouble[m_arrSize];
              memcpy(arrFloat, autoFillPointerFloat, m_arrSize*sizeof(float));
              for(uint32_t m = 0; m < m_arrSize; m++ )
              {
                  arrDouble[m] = static_cast<double>(arrFloat[m]);
              }
              doublePtrArr = arrDouble;
              return doublePtrArr;
          }
          else if (autoFillPointerInt    != 0)
          {
              int arrInt[m_arrSize];
              double arrDouble[m_arrSize];
              memcpy(arrInt, autoFillPointerInt, m_arrSize*sizeof(int));
              for(uint32_t m = 0; m < m_arrSize; m++ )
              {
                  arrDouble[m] = static_cast<double>(arrInt[m]);
                  //std::cout << "arrDouble[m]: " << arrDouble[m] << std::endl;
              }
              doublePtrArr = arrDouble;
              return doublePtrArr;
          }
          else if (autoFillFunctionFloat != 0)
          {
              float arrFloat[m_arrSize];
              double arrDouble[m_arrSize];
              
              float* tmpPtr = NULL;
              tmpPtr = autoFillFunctionFloat();
              memcpy(arrFloat, tmpPtr, m_arrSize*sizeof(float));
              for(uint32_t m = 0; m < m_arrSize; m++ )
              {
                  arrDouble[m] = static_cast<double>(arrFloat[m]);
              }
              doublePtrArr = arrDouble;
              return doublePtrArr;
          }
          else if (autoFillFunctionInt   != 0)
          {
              int arrInt[m_arrSize];
              double arrDouble[m_arrSize];
              int* tmpPtr = NULL;
              tmpPtr = autoFillFunctionInt();
              memcpy(arrInt, tmpPtr, m_arrSize*sizeof(int));
              for(uint32_t m = 0; m < m_arrSize; m++ )
              {
                  arrDouble[m] = static_cast<double>(arrInt[m]);
                  //std::cout << "arrDouble[m]: " << arrDouble[m] << std::endl;
              }
              doublePtrArr = arrDouble;
              return doublePtrArr;
          }
          else if (autoFillPointerDouble != 0) return (autoFillPointerDouble);

          return NULL;
      };

      string getUnit()            { return unit;    };
      void     setArrSize(uint32_t arrSize)  { m_arrSize = arrSize; };
      uint32_t getArrSize()       { return m_arrSize; };
      int    getNbins()           { return nBins;   };
      float  getMin()             { return min;     };
      float  getMax()             { return max;     };
      float  getBinWidth()        { return (max - min) / (float) nBins; };

      double* getCustomBinning()   { return customBinning;        };
      bool    usingCustomBinning() { return (customBinning != 0); };

     private:


      void defineBinning(int nBins_, float min_, float max_, float* customBinning_)
      {
          nBins = nBins_;
          min = min_;
          max = max_;

          if (customBinning_ == 0)
          {
              customBinning = 0;
          }
          else
          {
              customBinning = new double[nBins+1];
              for (int i = 0 ; i <= nBins ; i++)
              {
                  customBinning[i] = customBinning_[i];
              }
          }
      }

      void mapPointers(float* pointerFloat, double* pointerDouble, int* pointerInt, float* (*functionFloat)(), int* (*functionInt)())
      {
          autoFillPointerFloat  = pointerFloat;
          autoFillPointerDouble = pointerDouble;
          autoFillPointerInt    = pointerInt;
          autoFillFunctionFloat = functionFloat;
          autoFillFunctionInt   = functionInt;
      }

      string unit;
      uint32_t m_arrSize;
      double* doublePtrArr; 

      float*  autoFillPointerFloat;
      double* autoFillPointerDouble;
      int*    autoFillPointerInt;
      float*   (*autoFillFunctionFloat)();
      int*     (*autoFillFunctionInt  )();

      int    nBins;
      float  min;
      float  max;

      double* customBinning;

    };

}

#endif
