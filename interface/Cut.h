#ifndef Cut_h
#define Cut_h

#include "interface/Common.h"
#include "interface/Variable.h"

using namespace std;

namespace theDoctor
{

    class Cut
    {

     public:

      Cut(string targetTag_, char type_, float value_)
      {
          targetTag = targetTag_;
          type        = type_;
          value       = value_;

          target    = 0;
      };

      ~Cut() { };

      string    getTargetTag() const { return targetTag; };
      Variable* getTarget()    const { return target;    };
      char      getType()      const { return type;      };
      float     getValue()     const { return value;     };

      void checkAndLink(vector<Variable>* theVariables)
      {
          if ((type != '>') && (type != '<') && (type != '=')) { WARNING_MSG << "Cannot compile cut on target " << targetTag << " : '" << type << "' type is not supported." << endl; exit(-1); }

          target = 0;
          for (unsigned int i = 0 ; i < theVariables->size() ; i++)
          {
              Variable* var = &((*theVariables)[i]);
              if (var->getTag() == targetTag)
              {
                  target = var;
              }
          }
          if (target == 0)
          {
              WARNING_MSG << "Cannot compile cut on target " << targetTag << " : '" << targetTag << "' not found." << endl;
          }
      };

      bool pass() const
      {
          if (target == 0) { return false; }

          float targetValue = *target->getAutoFillValue(); //@MJ@ TODO implement array variable, or what should I do here for array variable?????

               if (type == '>') { if (targetValue >  value) return true; else return false; }
          else if (type == '<') { if (targetValue <  value) return true; else return false; }
          else if (type == '=') { if (targetValue == value) return true; else return false; }
          else return false;
      };

     private:

        string    targetTag;
        Variable* target;
        char      type;
        float     value;

    };

}

#endif
