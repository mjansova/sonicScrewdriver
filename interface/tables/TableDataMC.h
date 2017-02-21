#ifndef TableDataMC_h
#define TableDataMC_h

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

    class TableDataMC : public Table
    {

     public:

            TableDataMC()
            {
            }

            TableDataMC(SonicScrewdriver* screwdriver, vector<string> inputRegionTags, string channel, string options = "")
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
                if (inputRegionTags.size() != 0)
                {
                    vector<string> inputRegionLabels;

                    for (unsigned int i = 0 ; i < inputRegionTags.size() ; i++)
                    {
                        bool found = false;
                        for (unsigned int j = 0 ; j < regionsTags.size()  ; j++)
                        {
                            if (inputRegionTags[i] == regionsTags[j]) { inputRegionLabels.push_back(regionsLabels[j]); found = true; break; }
                        }
                        if (!found) WARNING_MSG << "Region " << inputRegionTags[i] << " was not found." << endl;
                    }

                    regionsTags   = inputRegionTags;
                    regionsLabels = inputRegionLabels;

                }

                bool keepNegative = OptionsScrewdriver::GetBoolOption(options,"keepNegative");
                Init(regionsTags,processesTags,keepNegative);
                SetLabels(regionsLabels,processesLabels);

                // Actually fill the table
                for (unsigned int r = 0 ; r < regionsTags.size()          ; r++)
                {
                    Figure tmpTotal(0.0,0.0);
                    for (unsigned int p = 0 ; p < processesTags.size() ; p++)
                    {
                        if (processesTags[p] == "totalSM") continue;
                        string type = screwdriver->GetProcessClassType(processesTags[p]);
                        Figure currentYield = screwdriver->GetYieldAndError(processesTags[p],
                                                                            regionsTags[r],
                                                                            channel);
                        Set(regionsTags[r], processesTags[p], currentYield);

                        if (type == "background" && currentYield.value()>0) tmpTotal += currentYield;
                    }
                    Set(regionsTags[r],"totalSM",tmpTotal);
                }

            };


            ~TableDataMC()
            {
            };

    };

}

#endif
