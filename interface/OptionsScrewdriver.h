#ifndef OptionsScrewdriver_h
#define OptionsScrewdriver_h

#include "interface/Common.h" 

using namespace std;

namespace theDoctor
{

    class OptionsScrewdriver 
    {
      
     public:
     
      OptionsScrewdriver();
      ~OptionsScrewdriver();

      static bool isInOptions(string options, string field)
      {
          if (options.find(field+"=") != string::npos) return true;
          else return false;
      }
      
      static string getStringOption(string options, string field)
      {
        if (!OptionsScrewdriver::isInOptions(options,field)) return "";

        string optionWithValue;
        stringstream stream(options);
        while( getline(stream,optionWithValue,',') )
        {
            stringstream stream2(optionWithValue);
            string option; getline(stream2,option,'=');
            
            if (option != field) continue;
            
            string value; 
            getline(stream2,value,'=');
            return value;
        }

        WARNING_MSG << "Tried to access option '" << field << "' not present in the list, returned default value" << endl;
        
        // TODO fix this : every available field should have a default option
        // set somewhere else
        return "";
      }

      static bool getBoolOption(string options, string field)
      {
          string value = getStringOption(options,field);

               if (value == string("true") ) return true;
          else if (value == string("false")) return false;
          else 
          {
          //    WARNING_MSG << "Tried to access boolean option'" << field << "', but value is neither 'true' or 'false'" << endl;
              return false;
          }
      }

    };

}

#endif
