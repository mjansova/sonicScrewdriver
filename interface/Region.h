#ifndef Region_h
#define Region_h

#include "Name.h"
#include "Cut.h"
#include "interface/OptionsScrewdriver.h"

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
            showCutsMode = false;

            parentRegionPointer = parentRegionPointer_;

            WARNING_MSG << "(Creating region " << tag_ << ") Using function pointers to create regions is deprecated. Please move to cut vectors and region inheritance." << endl;

            if (OptionsScrewdriver::GetBoolOption(options_,"showCuts"))
            {
                WARNING_MSG << "Cannot use 'showCuts' option on region '" << tag_ << "' defined with a selector. Try using a vector of cuts." << endl;
            }
      };

      Region(string tag_, string plotLabel_, vector<Cut> setOfCuts_, vector<Variable>* theVariables, string options_ = "", pair<vector<Region>*,int> parentRegionPointer_ = pair<vector<Region>*,int>(0,-1)):
      Name(tag_,plotLabel_, options_)
      {
            setOfCuts = setOfCuts_;
            selector = 0;
            flag = false;
            showCutsMode = false;

            for (unsigned int i = 0 ; i < setOfCuts.size() ; i++)
            {
                setOfCuts[i].checkAndLink(theVariables);
            }

            parentRegionPointer = parentRegionPointer_;

            if (OptionsScrewdriver::GetBoolOption(options_,"showCuts"))
            {
                showCutsMode = true;
            }
      };

      ~Region()
      {
      };

      bool passSelection(string ignoreVariable = "")
      {
          if (selector != 0)
              return selector();
          else
          {
                for (unsigned int i = 0 ; i < setOfCuts.size() ; i++)
                {
                    // If we're looking at a variable that should be ignored, ignore it..
                    if ((ignoreVariable != "") && (setOfCuts[i].getTargetTag() == ignoreVariable)) continue;

                    if (setOfCuts[i].pass() == false) return false;
                }
                return true;
          }
      };


      void updateSelectionFlag() { flag = passSelection(); };

      // Accessors

      bool getShowCutsMode() { return showCutsMode; };

      bool getSelectionFlag(string ignoreVariable = "")
      {
          bool flagToBeUsed = false;

          // If no variable is to be ignored, use the standard flag
          if (ignoreVariable == "") flagToBeUsed = flag;
          // Else we recompute a flag only for this specific check
          else                      flagToBeUsed = passSelection(ignoreVariable);


          // If no parent region, directly return the flag
          if (parentRegionPointer.first == 0) return flagToBeUsed;
          // Else, check parent's flag
          else
          {
              bool flagFromParent = (*parentRegionPointer.first)[ parentRegionPointer.second].getSelectionFlag();
              return (flagToBeUsed && flagFromParent);
          }
      };

      pair<bool,Cut> getCutForVariable(string variableTag)
      {
          if (selector != 0) return pair<bool,Cut>(false,Cut("",' ',-1.0));

          for (unsigned int c = 0 ; c < setOfCuts.size() ; c++)
          {
              if (setOfCuts[c].getTargetTag() == variableTag) return pair<bool,Cut>(true,setOfCuts[c]);
          }

          return pair<bool,Cut>(false,Cut("",' ',-1.0));
      }

     private:

      bool (*selector)(); //@MJ@ TODO I need bool (*selector)(ofstream &outputFile)
      vector<Cut> setOfCuts;
      bool flag;
      bool showCutsMode;

      pair<vector<Region>*,int> parentRegionPointer;

    };

}

#endif
