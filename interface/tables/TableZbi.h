#ifndef TableZbi_h
#define TableZbi_h

#include <iostream>
#include <vector>
#include <iomanip>

#include "interface/Figure.h"
#include "interface/Table.h"
#include "interface/SonicScrewdriver.h"

using namespace std;

namespace theDoctor
{

    class TableZbi : public Table
    {

     public:

            TableZbi();

            TableZbi(SonicScrewdriver* screwdriver, vector<string> inputRegionTags, string channel)
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
                    Figure tmpZero(0.0,0.0);
                    for (unsigned int p = 0 ; p < processesTags.size() ; p++)
                    {
                        if (processesTags[p] == "totalSM") continue;
                        string type = screwdriver->GetProcessClassType(processesTags[p]);
                        if (type != "background") continue;
                        Figure currentYield = screwdriver->GetYieldAndError(processesTags[p],
                                                                            regionsTags[r],
                                                                            channel);
                       
                        tmpTotal += currentYield;

                    }
                    for (unsigned int p = 0 ; p < processesTags.size() ; p++)
                    {
                        if (processesTags[p] == "totalSM") continue;
                        string type = screwdriver->GetProcessClassType(processesTags[p]);
                        Figure currentYield = screwdriver->GetYieldAndError(processesTags[p],
                                                                            regionsTags[r],
                                                                            channel);
                        Figure zbi = Zbi(currentYield.value(), tmpTotal.value());

                        if (type == "background") 
                            Set(regionsTags[r], processesTags[p], tmpZero);
                        else 
                            Set(regionsTags[r], processesTags[p], zbi);
                    }
                    Set(regionsTags[r],"totalSM", tmpZero);
                }

            };

            double Zbi(double n_sig, double n_b, double rel_uncert_b = 0.3 )
            {
                double n_on = n_sig+n_b;
                double mu_b_hat = n_b;
                double sigma_b = rel_uncert_b*n_b;
                double tau = mu_b_hat/(sigma_b*sigma_b);
                double n_off = tau*mu_b_hat;
                double P_Bi = TMath::BetaIncomplete(1./(1.+tau),n_on,n_off+1);
                double Z_Bi = sqrt(2)*TMath::ErfInverse(1 - 2*P_Bi);
                return Z_Bi;
            }
            ~TableZbi()
            {
            };

    };

}

#endif
