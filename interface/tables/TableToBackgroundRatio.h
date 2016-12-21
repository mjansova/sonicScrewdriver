#ifndef TableToBackgroundRatio_h
#define TableToBackgroundRatio_h

#include <iostream>
#include <vector>
#include <iomanip>

#include "interface/Figure.h"
#include "interface/Table.h"
#include "interface/SonicScrewdriver.h"

using namespace std;

namespace theDoctor
{

    class TableToBackgroundRatio : public Table
    {

     public:

            TableToBackgroundRatio();

            TableToBackgroundRatio(SonicScrewdriver* screwdriver, vector<string> inputRegionTags, string channel)
            {
                vector<string> rawProcessesTags;
                vector<string> rawProcessesLabels;
                vector<string> processesTags;
                vector<string> processesLabels;
                vector<string> regionsTags;
                vector<string> regionsLabels;

                screwdriver->GetProcessClassTagList  (&rawProcessesTags  );
                screwdriver->GetProcessClassLabelList(&rawProcessesLabels);

                screwdriver->GetRegionTagList  (&regionsTags  );
                screwdriver->GetRegionLabelList(&regionsLabels);

                // Sort backgrounds, signal, and add "total background" line
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
                    if (type == "signal")
                    {
                        processesTags.push_back(rawProcessesTags[i]);
                        processesLabels.push_back(rawProcessesTags[i]);
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

                Init(regionsTags,processesTags);
                SetLabels(regionsLabels,processesLabels);

                // Actually fill the table signal/bkg and bkg/bkg
                for (unsigned int r = 0 ; r < regionsTags.size()          ; r++)
                {
                    Figure tmpTotal(0.0,0.0);
                    for (unsigned int p = 0 ; p < processesTags.size() ; p++)
                    {
                        if (processesTags[p] == "totalSM") continue;
                        string type = screwdriver->GetProcessClassType(processesTags[p]);
                        if (type != "background") continue;
                        Figure currentYield = screwdriver->GetYieldAndError(processesTags[p],
                                                                            regionsTags[r],
                                                                            channel);
                       
                        if(currentYield.value()>0) tmpTotal += currentYield;

                    }
                    for (unsigned int p = 0 ; p < processesTags.size() ; p++)
                    {
                        if (processesTags[p] == "totalSM") continue;
                        Figure currentYield = screwdriver->GetYieldAndError(processesTags[p],
                                                                            regionsTags[r],
                                                                            channel);
                        Figure currentSOverB = currentYield/tmpTotal;
                        Set(regionsTags[r], processesTags[p], currentSOverB);

                    }
                    Set(regionsTags[r],"totalSM",tmpTotal/tmpTotal);
                }

            };


            ~TableToBackgroundRatio()
            {
            };

    };

}

#endif
