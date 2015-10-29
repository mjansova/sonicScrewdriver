#ifndef OptionsScrewdriver_h
#define OptionsScrewdriver_h

#include "interface/Common.h"

using namespace std;

#include "picojson/picojson.h"


namespace theDoctor
{

class OptionsScrewdriver
{

    public:

        OptionsScrewdriver()
        {
            string defaultConfig
            (
#include "DefaultConfig.json"
            );

            ReadJsonConfig(defaultConfig);

        }

        void LoadJsonConfig(string fileName)
        {
            ifstream f(fileName);
            stringstream buffer;
            buffer << f.rdbuf();
            ReadJsonConfig(buffer.str());
        }

        void ReadJsonConfig(string configString)
        {
            picojson::value configJson;

            string err = picojson::parse(configJson, configString);
            if (!err.empty()) { cerr << "Error while loading json file : " << err << endl; exit(-1); }

            const picojson::value::object& level0 = configJson.get<picojson::object>();

            for (picojson::value::object::const_iterator level1 = level0.begin() ; level1 != level0.end() ; ++level1)
            {
                string level1Name                      = level1->first;
                const picojson::value::object& level1_ = level1->second.get<picojson::object>();

                for (picojson::value::object::const_iterator level2 = level1_.begin() ; level2 != level1_.end() ; ++level2)
                {
                    string level2Name             = level2->first;
                    const picojson::value level2_ = level2->second;

                    if (level2_.is<double>()) SetGlobalFloatOption (level1Name,level2Name,level2_.get<double>());
                    if (level2_.is<bool>  ()) SetGlobalBoolOption  (level1Name,level2Name,level2_.get<bool>  ());
                    if (level2_.is<string>()) SetGlobalStringOption(level1Name,level2Name,level2_.get<string>());
                }
            }

        }

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

        static vector<string> GetStringListOption(string options, string field)
        {
            vector<string> parsedList;
            if (!OptionsScrewdriver::IsInOptions(options,field)) return parsedList;

            string rawList = GetStringOption(options, field);
            string parsedListElement;
            stringstream stream(rawList);
            while( getline(stream,parsedListElement,':') )
            {
                parsedList.push_back(parsedListElement);
            }

            return parsedList;
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

        string GetGlobalStringOption(string category, string field)
        { pair<string,string> name(category,field); return theGlobalOptions_string[name]; }

        float  GetGlobalFloatOption(string category, string field)
        { pair<string,string> name(category,field); return theGlobalOptions_float[name]; }

        bool   GetGlobalBoolOption(string category, string field)
        { pair<string,string> name(category,field); return theGlobalOptions_bool[name]; }

    private:

        std::map<pair<string,string>,float>  theGlobalOptions_float;
        std::map<pair<string,string>,string> theGlobalOptions_string;
        std::map<pair<string,string>,bool>   theGlobalOptions_bool;

};

}

#endif
