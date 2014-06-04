#ifndef OptionsScrewdriver_h
#define OptionsScrewdriver_h

#include "interface/Common.h" 

using namespace std;

namespace theDoctor
{

    class OptionsScrewdriver 
    {

        public:

            OptionsScrewdriver() { }
            ~OptionsScrewdriver() { }

            static bool IsInOptions(string options, string field)
            {
                if (options.find(field+"=") != string::npos) return true;
                else return false;
            }

            static string GetStringOption(string options, string field)
            {
                if (!OptionsScrewdriver::IsInOptions(options,field)) return "";

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

                return "";

            }

            static float GetFloatOption(string options, string field)
            {
                if (!OptionsScrewdriver::IsInOptions(options,field)) return -1.0;

                string optionWithValue;
                stringstream stream(options);
                while( getline(stream,optionWithValue,',') )
                {
                    stringstream stream2(optionWithValue);
                    string option; getline(stream2,option,'=');

                    if (option != field) continue;

                    string value; 
                    getline(stream2,value,'=');
                    return atof(value.c_str());
                }
                
                return -1.0;
            }

            static bool GetBoolOption(string options, string field)
            {
                string option;
                stringstream stream(options);
                while( getline(stream,option,',') )
                {
                    if (option == field) return true;
                }
                
                return false;
            }

            void SetGlobalStringOption(string category, string field, string value)
            {  pair<string,string> name(category,field); theGlobalOptions_string[name] = value; }

            void SetGlobalFloatOption(string category, string field, float value)
            {  pair<string,string> name(category,field); theGlobalOptions_float[name] = value; }

            void SetGlobalBoolOption(string category, string field, bool value)
            {  pair<string,string> name(category,field); theGlobalOptions_bool[name] = value; }

            void SetGlobalIntOption(string category, string field, int value)
            { pair<string,string> name(category,field); theGlobalOptions_int[name] = value; }

            string GetGlobalStringOption(string category, string field)
            { pair<string,string> name(category,field); return theGlobalOptions_string[name]; }

            float  GetGlobalFloatOption(string category, string field)
            { pair<string,string> name(category,field); return theGlobalOptions_float[name]; }

            bool   GetGlobalBoolOption(string category, string field)
            { pair<string,string> name(category,field); return theGlobalOptions_bool[name]; }

            int    GetGlobalIntOption(string category, string field)
            { pair<string,string> name(category,field); return theGlobalOptions_int[name]; }

        private:

            std::map<pair<string,string>,float>  theGlobalOptions_float;
            std::map<pair<string,string>,string> theGlobalOptions_string;
            std::map<pair<string,string>,bool>   theGlobalOptions_bool;
            std::map<pair<string,string>,int>    theGlobalOptions_int;

    };

}

#endif
