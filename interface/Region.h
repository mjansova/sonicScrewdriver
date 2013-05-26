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
      }; 

      ~Region() 
      { 
      };

      bool passSelection() { return selector(); };

      void AddVariables(vector<string> list, vector<int> listById)
      {
          for (unsigned int i = 0 ; i < list.size() ; i++)
          {
              associatedVariables.push_back(list[i]);
              associatedVariablesById.push_back(listById[i]);
          }
      }
      
      vector<string> AssociatedVariables()     { return associatedVariables;     }
      vector<int>    AssociatedVariablesById() { return associatedVariablesById; }

      // Accessors

     private:

      bool (*selector)();

      vector<string> associatedVariables;
      vector<int>    associatedVariablesById;

    };

}

#endif
