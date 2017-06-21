#ifndef CombineCardMaker_h
#define CombineCardMaker_h

#include <iostream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <fstream>

#include "interface/Figure.h"
#include "interface/Table.h"
#include "interface/SonicScrewdriver.h"

using namespace std;

namespace theDoctor
{

    class CombineCardMaker : public Table
    {

     public:

            CombineCardMaker(){};

            CombineCardMaker(SonicScrewdriver* screwdriver, vector<string> inputRegionTags, string channel, string SigProcessClass, string sigVarX, string sigVarY)
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

                bool dataFilled = false;
                for (unsigned int i = 0 ; i < rawProcessesTags.size() ; i++)
                {
                    string type2 = screwdriver->GetProcessClassType(rawProcessesTags[i]);
                    if (type2 == "data")
                    {
                        if(dataFilled == true)
                            throw std::runtime_error("data are already present!");
                        processesTags.push_back(rawProcessesTags[i]);
                        processesLabels.push_back(rawProcessesTags[i]);
                        dataFilled = true;
                    }
                }

                /*
		for (unsigned int i = 0 ; i < rawProcessesTags.size() ; i++)
                {
                    string type = screwdriver->GetProcessClassType(rawProcessesTags[i]);
                    if (type == "signal")
                    {
                        processesTags.push_back(rawProcessesTags[i]);
                        processesLabels.push_back(rawProcessesTags[i]);
                    }
                }
		*/
		//Add signal labels by looking to non empty bins (for the moment in the first region only)
		 if(regionsTags.size()==0){
		 	cerr<<"CombinedCardMaker::no regions have been defined !"<<endl;
		 	return;
		 }
 
                 if(SigProcessClass != "none")
                 {
		 TH2D* histo2Dtmp = screwdriver-> Get2DHistoClone(sigVarX, sigVarY, SigProcessClass, regionsTags[0], channel);
		 for(int binX = 1 ; binX<histo2Dtmp->GetNbinsX(); binX++){

			for(int binY = 1 ; binY<histo2Dtmp->GetNbinsX(); binY++){
				if( histo2Dtmp->GetBinContent(binX, binY) !=0 ){
                        		stringstream ss;
					//rouding the masses by step of 5 GeV

					ss<<"("<<(int)histo2Dtmp->GetXaxis()->GetBinCenter(binX)<<","<<(int)histo2Dtmp->GetYaxis()->GetBinCenter(binY)<<")";
					processesTags.push_back(ss.str());
                        		processesLabels.push_back(ss.str());
				}
                        	

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

                Init(regionsTags,processesTags);
                SetLabels(regionsLabels,processesLabels);

                // Actually fill the table signal/bkg and bkg/bkg
                for (unsigned int r = 0 ; r < regionsTags.size()          ; r++)
                {
      		
		    //retrive signal yield from 2D plot 
		    TH2D* histo2DSigYields = screwdriver-> Get2DHistoClone(sigVarX, sigVarY, SigProcessClass, regionsTags[r], channel);
                    
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
                       
			Set(regionsTags[r], processesTags[p], currentYield);
                        tmpTotal += currentYield;

                    }

                    if(SigProcessClass != "none")
                    {
		    //Scan the Signal 2D plan
		    for(int binX = 1 ; binX<histo2DSigYields->GetNbinsX(); binX++){
			for(int binY = 1 ; binY<histo2DSigYields->GetNbinsX(); binY++){
				Figure sigYield (histo2DSigYields->GetBinContent(binX, binY), histo2DSigYields->GetBinError(binX, binY));
				Figure zbi = Zbi(sigYield.value(), tmpTotal.value());
                        	
				stringstream ss;
				ss<<"("<<(int)histo2DSigYields->GetXaxis()->GetBinCenter(binX)<<","<<(int)histo2DSigYields->GetYaxis()->GetBinCenter(binY)<<")";
				string label = ss.str();
                        	cout<<"yield: "<<ss.str()<<" "<<histo2DSigYields->GetBinContent(binX, binY);//<<endl;
				cout<<" "<<sigYield.value()<<" "<<tmpTotal.value();
				cout<<" "<<zbi.value()<<endl;

                            	//Set(regionsTags[r], label, zbi);
                            	Set(regionsTags[r], label, sigYield);
			}

		    }
                    }
		    ///*
                    for (unsigned int p = 0 ; p < processesTags.size() ; p++)
                    {
                        if (processesTags[p] == "totalSM"){
                        	/*
				Figure currentYield = screwdriver->GetYieldAndError(processesTags[p],
                                                                            regionsTags[r],
                                                                            channel);
                            	*/
				//put the sum of bkg
				cout<<"I found totalSM - "<<tmpTotal.value()<<endl;
				Set(regionsTags[r], processesTags[p], tmpTotal);
			
			}
		    	/*
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
                    	*/
		    }
		    //*/
		    //Loop over signal points


                    //Set(regionsTags[r],"totalSM", tmpZero);
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
            ~CombineCardMaker()
            {
            };

	    void UpdateCardTable(string inputTab){

                Table inTable(inputTab);
                vector<string> processList = inTable.rowTags; 
                vector<string> regionsList = inTable.colTags;

		for (unsigned int i = 0 ; i < nRow ; i++){
			//cout<<rowLabels[i]<<endl;
			if( !(rowLabels[i].find("total SM")!=std::string::npos || rowLabels[i].find("totalSM")!=std::string::npos) ){
				//cout<<"new process !"<<endl;
                                if(std::find(processList.begin(), processList.end(), rowLabels[i]) == processList.end() )
                                    throw std::runtime_error("The processes names do not agree between the two tables");

				for(unsigned int j = 0; j < nCol ; j++){ 
                                    if(std::find(regionsList.begin(), regionsList.end(), colLabels[j]) == regionsList.end() )
                                        throw std::runtime_error("The regions names do not agree between the two tables");
				    Figure y = Get(j,i);
                                    inTable.Set(colLabels[j], rowLabels[i], y); //@MJ@ TODO there can be mismatch between labels and tags
                                    //cout << "table set with" << colLabels[j]<< rowLabels[i] << y.value() << endl;
                                }
                        }
                }
                inTable.Print(inputTab,4);
            }; //@MJ@ TODO is the updated table really saved?! 

	    void ProduceCard(string inputTab, string outputdir,float sigRelUncert = 1.1, float bkgRelUncert = 1.2){

                Table inTable(inputTab);
                vector<string> processList = inTable.rowTags; 
                vector<string> regionsList = inTable.colTags; 
		vector<int> iBkgLine;
		for (unsigned int i = 0 ; i < processList.size() ; i++){
			cout<<processList.at(i)<<endl;
			if(processList.at(i).find("bkg")!=std::string::npos) iBkgLine.push_back(i);
		}
		if(iBkgLine.size()==0){
			cerr<<"CombinedCardMaker::backgrounds not found !"<<endl;
			return;
		}
		int iDataLine=-1;
		for (unsigned int i = 0 ; i < processList.size() ; i++){
			if(processList.at(i).find("data")!=std::string::npos) iDataLine =i;
		}
		if(iDataLine==-1){
			cout<<"CombinedCardMaker::no data"<<endl;
		}

                ofstream shfile;
                shfile.open (outputdir+"/combineCommands.sh");
		//looking for signal
		for (unsigned int i = 0 ; i < processList.size() ; i++){
			cout<<processList.at(i)<<endl;
			if(processList.at(i).find("(")!=std::string::npos && processList.at(i).find(")")!=std::string::npos){
				cout<<"selected !"<<endl;
				string filename;
				size_t found = processList.at(i).find(",");
				string val1 = processList.at(i).substr(processList.at(i).find("(")+1,found-1);
				string val2 = processList.at(i).substr(found+1,processList.at(i).find(")")-found-1);
				filename = outputdir+string("/")+val1+string("_")+val2+string(".tab");
                                shfile << "combine -M Asymptotic " << val1+string("_")+val2+string(".tab") << " -n " << val1+string("_")+val2 << endl;

				ofstream ofile(filename.c_str());
				ofile<<"imax "<<regionsList.size()<<" number of bins"<<endl;
				ofile<<"jmax "<< iBkgLine.size() <<" number of processes"<<endl; // bkg + signal only
				ofile<<"kmax 1 number of nuisance parameters"<<endl; // only a systematic on the bkg
				ofile<<"----------------------------------------------------"<<endl;
				ofile<<"bin\t";
				for(unsigned int j = 0; j < regionsList.size() ; j++){ ofile<<"ch"<<j<<"\t"; }
				ofile<<endl;
				ofile<<"----------------------------------------------------"<<endl;
				ofile<<"observation\t";
				for(unsigned int j = 0; j < regionsList.size() ; j++){ ofile<<inTable.Get(j,iDataLine).value()<<"\t"; }
				ofile<<endl;
				ofile<<"----------------------------------------------------"<<endl;
				ofile<<"bin\t";
				for(unsigned int j = 0; j < regionsList.size() ; j++)
                                {
                                    ofile<<"ch"<<j<<"\t"; 
                                    for(uint32_t l=0; l<iBkgLine.size(); l++ )
                                    {
                                        ofile<<"ch"<<j<<"\t"; 
                                    }
                                };
				ofile<<endl;
				ofile<<"process\t";
				//for(unsigned int j = 0; j < nCol ; j++){ ofile<<"bkg\tsignal\t"; }
				for(unsigned int j = 0; j  < regionsList.size() ; j++)
                                { 
                                    ofile<<"sig\t";
                                    for(uint32_t l=0; l<iBkgLine.size(); l++ )
                                    {
                                        ofile<<processList.at(l) <<"\t"; 
                                    }
                                }
				ofile<<endl;
				ofile<<"process\t";
				for(unsigned int j = 0; j  < regionsList.size() ; j++)
                                { 
                                    ofile<<"0\t";
                                    for(uint32_t l=0; l<iBkgLine.size(); l++ )
                                    {
                                        ofile<<l+1<<"\t"; 
                                    }
                                }
				ofile<<endl;
				ofile<<"rate\t";
				for(unsigned int j = 0; j < regionsList.size() ; j++){ 
					//signal yield (current one)
			            ofile<<inTable.Get(j,i).value()<<"\t";
					//background yield
                                    for(uint32_t l=0; l<iBkgLine.size(); l++ )
                                    {
					ofile<<inTable.Get(j,iBkgLine.at(l)).value()<<"\t";
                                    }
				}
				ofile<<endl;
				ofile<<"----------------------------------------------------"<<endl;
				ofile<<"uncert_bkg\t lnN \t";
				for(unsigned int j = 0; j < nCol ; j++)
                                { 
                                    ofile<<sigRelUncert<<"\t ";
                                    for(uint32_t l=0; l<iBkgLine.size(); l++ )
                                    {
                                        ofile<<bkgRelUncert<<" \t";
                                    }
                                }
				ofile<<endl;
				ofile.close();

			}
		}
                shfile << "hadd allLimits.root *.root" << endl;
                shfile.close();
	    	
	    }

    };

}

#endif
