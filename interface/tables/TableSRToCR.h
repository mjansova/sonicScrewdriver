#ifndef TableSRToCR_h
#define TableSRToCR_h

#include <iostream>
#include <vector>
#include <iomanip>

#include "interface/Figure.h"
#include "interface/Table.h"
#include "interface/SonicScrewdriver.h"
#include "interface/OptionsScrewdriver.h"

using namespace std;

namespace theDoctor
{

    class TableSRToCR : public Table
    {

     public:

            TableSRToCR()
            {
            }

            TableSRToCR(SonicScrewdriver* screwdriver, vector<string> SRTags, string CRTag, string channel, string options = "")
            {
                vector<string> rawProcessesTags;
                vector<string> rawProcessesLabels;
                vector<string> processesTags;
                vector<string> processesLabels;
                vector<string> regionsTags;
                vector<string> regionsLabels;

                bool includeSignal = OptionsScrewdriver::GetBoolOption(options,"includeSignal");

                screwdriver->GetProcessClassTagList  (&rawProcessesTags  );
                screwdriver->GetProcessClassLabelList(&rawProcessesLabels);

                screwdriver->GetRegionTagList  (&regionsTags  );
                screwdriver->GetRegionLabelList(&regionsLabels);

                // Sort backgrounds, data, and add "total background" line
                for (unsigned int i = 0 ; i < rawProcessesTags.size() ; i++)
                {
                    string type = screwdriver->GetProcessClassType(rawProcessesTags[i]);
                    if (type == "background")
                    {
                        processesTags.push_back(rawProcessesTags[i]);
                        processesLabels.push_back(rawProcessesTags[i]);
                    }
                }

                processesTags.push_back("totalSM");
                processesLabels.push_back("total SM");
 
                processesTags.push_back("SRtoCR");
                processesLabels.push_back("SRtoCR");

                for (unsigned int i = 0 ; i < rawProcessesTags.size() ; i++)
                {
                    string type = screwdriver->GetProcessClassType(rawProcessesTags[i]);
                    if (type == "data")
                    {
                        processesTags.push_back(rawProcessesTags[i]);
                        processesLabels.push_back(rawProcessesTags[i]);
                    }
                }

                if (includeSignal)
                {
                    for (unsigned int i = 0 ; i < rawProcessesTags.size() ; i++)
                    {
                        string type = screwdriver->GetProcessClassType(rawProcessesTags[i]);
                        if (type == "signal")
                        {
                            processesTags.push_back(rawProcessesTags[i]);
                            processesLabels.push_back(rawProcessesTags[i]);
                        }
                    }
                }

                // Get labels for input regions
                if (SRTags.size() != 0)
                {
                    vector<string> SRLabels;

                    for (unsigned int i = 0 ; i < SRTags.size() ; i++)
                    {
                        bool found = false;
                        for (unsigned int j = 0 ; j < regionsTags.size()  ; j++)
                        {
                            if (SRTags[i] == regionsTags[j]) { SRLabels.push_back(regionsLabels[j]); found = true; break; }
                        }
                        if (!found) WARNING_MSG << "Region " << SRTags[i] << " was not found." << endl;
                    }
                    if(SRTags.size() != SRLabels.size() )
                        throw std::runtime_error("Not same amount of tags as labels");
                   
                    regionsTags.clear();
                    regionsLabels.clear(); //@MJ@ TODO do this better!!!!
 
                    regionsTags.push_back(CRTag);
                    regionsLabels.push_back(CRTag); //@MJ@ TODO do this better!!!!
 
                    for(uint32_t tl=0; tl<SRTags.size(); tl++)
                    {
                        regionsTags.push_back(SRTags.at(tl));
                        regionsLabels.push_back(SRLabels.at(tl));
                    }

                }

                Init(regionsTags,processesTags);
                SetLabels(regionsLabels,processesLabels);

                // Actually fill the table
                for (unsigned int r = 0 ; r < regionsTags.size()          ; r++)
                {
                    Figure tmpTotal(0.0,0.0);
                    for (unsigned int p = 0 ; p < processesTags.size() ; p++)
                    {
                        if (processesTags[p] == "totalSM") continue;
                        if (processesTags[p] == "SRtoCR") continue;
                        string type = screwdriver->GetProcessClassType(processesTags[p]);
                        Figure currentYield = screwdriver->GetYieldAndError(processesTags[p],
                                                                            regionsTags[r],
                                                                            channel);
                        Set(regionsTags[r], processesTags[p], currentYield);

                        if (type == "background" && currentYield.value()>0 && processesTags[p] != "SRtoCR") tmpTotal += currentYield;
                    }
                    Set(regionsTags[r],"totalSM",tmpTotal);

                    Figure CRYield = Get(CRTag, "totalSM");
                    Figure SRYield = Get(regionsTags[r],"totalSM");
                    //double ratio = SRYield.value()/CRYield.value();
                    //double dratio = SRYield.error()/CRYield.error();
                    Figure ratio = SRYield/CRYield;

                    //Set(regionsTags[r], "SRtoCR", Figure(ratio,dratio));
                    Set(regionsTags[r], "SRtoCR", ratio);
                }

            };


            ~TableSRToCR()
            {
            };

    };

}

#endif
