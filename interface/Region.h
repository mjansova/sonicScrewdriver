#ifndef Region_h
#define Region_h

#include "Name.h" 
#include "Cut.h"

namespace theDoctor
{

    class Region : public Name 
    {
      
     public:
     
      Region(string tag_, string plotLabel_, bool (*selector_)(), string options_ = "", pair<vector<Region>*,int> parentRegionPointer_ = pair<vector<Region>*,int>(0,-1)):
      Name(tag_,plotLabel_, options_)
      {
            selector = selector_;
            setOfCuts.clear();
            flag = false;
            parentRegionPointer = parentRegionPointer_;
      }; 

      Region(string tag_, string plotLabel_, vector<Cut> setOfCuts_, vector<Variable>* theVariables, string options_ = "", pair<vector<Region>*,int> parentRegionPointer_ = pair<vector<Region>*,int>(0,-1)):
      Name(tag_,plotLabel_, options_)
      {
            setOfCuts = setOfCuts_;
            selector = 0;
            flag = false;

            for (unsigned int i = 0 ; i < setOfCuts.size() ; i++)
            {
                setOfCuts[i].checkAndLink(theVariables);
            }

            parentRegionPointer = parentRegionPointer_;
      }; 
      
      ~Region() 
      { 
      };

      bool passSelection()       
      { 
          if (selector != 0) 
              return selector(); 
          else
          {
              for (unsigned int i = 0 ; i < setOfCuts.size() ; i++)
              {
                  if (setOfCuts[i].pass() == false) return false; 
              }
              return true;
          }
      };
      
      bool getSelectionFlag()    
      {
          if (parentRegionPointer.first == 0) 
          { 
              return  flag; 
          }
          else                                
          { 
              bool flagFromParent = (*parentRegionPointer.first)[ parentRegionPointer.second].getSelectionFlag();
              //DEBUG_MSG << flagFromParent << endl;
              return (flag && flagFromParent);
          }
      };

      void updateSelectionFlag() { flag = passSelection(); };

      // Accessors

     private:

      bool (*selector)();
      vector<Cut> setOfCuts;
      bool flag;

      pair<vector<Region>*,int> parentRegionPointer;

    };

}

#endif
