
#include "interface/SonicScrewdriver.h"

using namespace theDoctor;

SonicScrewdriver::SonicScrewdriver():
theHistoScrewdriver(&theVariables,&theProcessClasses,&theRegions,&theChannels) { }

SonicScrewdriver::~SonicScrewdriver()
{ }

// #################################
// #   Variables & processClasses  #
// #          management           #
// #################################

void SonicScrewdriver::AddVariable(string tag, string plotLabel, string unit, int nBins, float min, float max, float* autoFillPointer, string options)
{	theVariables.push_back(Variable(tag,plotLabel,unit,nBins,min,max,autoFillPointer,options));	}

int SonicScrewdriver::getIndexOfVariable(string tag)
{
	for (unsigned int i = 0 ; i < theVariables.size() ; i++)
		if (theVariables[i].getTag() == tag) return i;
	
	return -1;
}


void SonicScrewdriver::AddProcessClass(string tag, string plotLabel, string type, Color_t color, string options)
{	theProcessClasses.push_back(ProcessClass(tag,plotLabel,type,color,options));	}

void SonicScrewdriver::AddDataset(string tag, string processClass, int trueNumberOfEvents, float xsecOrLumi, string options)
{	
    theDatasets.push_back(Dataset(tag,processClass,trueNumberOfEvents,xsecOrLumi,options));	
    if (GetProcessClassType(processClass) == "data") theLumi += xsecOrLumi;
}


void SonicScrewdriver::SetTrueNumberOfEvents(string dataset, int n)
{
    for (unsigned int i = 0 ; i < theDatasets.size() ; i++)
    {
        if (theDatasets[i].getTag() == dataset)
        {
            theDatasets[i].setTrueNumberOfEvents(n);
            return;
        }
    }
}

void SonicScrewdriver::GetProcessClassList(vector<string> *output)
{
	output->clear();
	for (unsigned int i = 0 ; i < theProcessClasses.size() ; i++)
        output->push_back(theProcessClasses[i].getTag());
}

void SonicScrewdriver::GetDatasetList(vector<string> *output)
{
	output->clear();
	for (unsigned int i = 0 ; i < theDatasets.size() ; i++)
        output->push_back(theDatasets[i].getTag());
}



string SonicScrewdriver::GetProcessClassType(string processClass)
{
	for (unsigned int i = 0 ; i < theProcessClasses.size() ; i++)
    {
        if (theProcessClasses[i].getTag() != processClass) continue;
        return theProcessClasses[i].getType();
    }
    return "";
}


float SonicScrewdriver::GetDatasetLumiWeight(string dataset)
{
	for (unsigned int i = 0 ; i < theDatasets.size() ; i++)
    {
        if (theDatasets[i].getTag() != dataset) continue;
        string type = GetProcessClassType(theDatasets[i].getProcessClass()); 
             if (type == "background")
            return theDatasets[i].getXsecOrLumi() * theLumi / theDatasets[i].getTrueNumberOfEvents();
        else if ((type == "data") || (type == "signal"))
            return 1.0;
    }
    return 0.0;
}

string SonicScrewdriver::GetProcessClass(string dataset)
{
	for (unsigned int i = 0 ; i < theDatasets.size() ; i++)
    {
        if (theDatasets[i].getTag() != dataset) continue;
        return theDatasets[i].getProcessClass(); 
    }
    return "";
}
  
float SonicScrewdriver::GetLumi()
{
    return theLumi;
}

void SonicScrewdriver::SetLumi(float inputLumi)
{
    theLumi = inputLumi;
}

// #####################################
// #   Regions & channels management   #
// #####################################

void  SonicScrewdriver::AddRegion(string tag, string plotLabel, bool (*selector)(), string options)
{	theRegions.push_back(Region(tag,plotLabel,selector,options));	}

void  SonicScrewdriver::ScheduleVariablesForRegion(string region, vector<string> list)
{	
	vector<string> cleanedList;
	vector<int> cleanedIdList;

	for (unsigned int i = 0 ; i < theRegions.size() ; i++)
    {
        if (theRegions[i].getTag() != region) continue;

		// TODO : Add management of a case where list is empty or list = "all" => add all the variables.
		
	    if ((list.size() == 1) && (list[0] == "all"))
        {
            for (unsigned int j = 0 ; j < theVariables.size() ; j++)
            {
				cleanedList.push_back(theVariables[j].getTag());
				cleanedIdList.push_back(j);
            }
        }
        else for (unsigned int j = 0 ; j < list.size() ; j++)
		{
            int tmpIndex = getIndexOfVariable(list[j]);
			if (tmpIndex != -1)
			{
				cleanedList.push_back(list[j]);
				cleanedIdList.push_back(tmpIndex);
			}
			else
				WARNING_MSG << "Did not find any variable named '" << list[j] << "'." << endl;
		}

		theRegions[i].AddVariables(cleanedList, cleanedIdList);

        return;
    }

	WARNING_MSG << "Did not found region named " << region << endl;
    return;
}



vector<string> Common_SplitString(string input,char sep)
{
	vector<string> output;

	string item;
	stringstream stream(input);
	while( getline(stream,item,sep) )
	{
		output.push_back(item);
	}

	return output;
}

void SonicScrewdriver::ScheduleVariablesForRegion(string region, string list)
{
	vector<string> splitedList = Common_SplitString(list,',');
	return ScheduleVariablesForRegion(region,splitedList);
}


void SonicScrewdriver::AddChannel(string tag, string plotLabel, bool (*selector)(), string options)
{	theChannels.push_back(Channel(tag,plotLabel,selector,options));	}


// ###########################
// #    Histo management     #
// ###########################

void SonicScrewdriver::AutoFillProcessClass(string processClass, float weight)
{	theHistoScrewdriver.AutoFillProcessClass(processClass,weight);	}

		// ###########################
		// #   1D histo management   #
		// ###########################

void SonicScrewdriver::Create1DHistos()
{	theHistoScrewdriver.Create1DHistos();	}

void SonicScrewdriver::Fill(string var,	string processClass, float value, float weight)
{	theHistoScrewdriver.Fill(var,processClass,value,weight); }

void SonicScrewdriver::ApplyScaleFactor(string var, string processClass, string region, string channel, Figure scaleFactor)
{	theHistoScrewdriver.ApplyScaleFactor(var,processClass,region, channel, scaleFactor);	}

		// ###########################
		// #   2D histo management   #
		// ###########################
 
void SonicScrewdriver::Add2DHisto(string varX, string varY, 
                                  bool autoFill, 
                                  int nBinsX, float minX, float maxX, 
                                  int nBinsY, float minY, float maxY)
{
  theHistoScrewdriver.Add2DHisto(varX,varY,
                                 autoFill,
                                 nBinsX,minX,maxX,
                                 nBinsY,minY,maxY);
}
  
void SonicScrewdriver::Fill(string varX, string varY, string processClass, float valueX, float valueY, float weight)
{ 	theHistoScrewdriver.Fill(varX,varY,processClass,valueX,valueY,weight);	}

		// ###########################
		// #   3D histo management   #
		// ###########################
 
void SonicScrewdriver::Add3DHisto(string varX, string varY, string varZ, 
                                  bool autoFill, 
                                  int nBinsX, float minX, float maxX, 
                                  int nBinsY, float minY, float maxY,
                                  int nBinsZ, float minZ, float maxZ)
{
  theHistoScrewdriver.Add3DHisto(varX,varY,varZ,
                                 autoFill,
                                 nBinsX,minX,maxX,
                                 nBinsY,minY,maxY,
                                 nBinsZ,minZ,maxZ);
}
  
void SonicScrewdriver::Fill(string varX, string varY, string varZ, string processClass, float valueX, float valueY, float valueZ, float weight)
{ 	theHistoScrewdriver.Fill(varX,varY,varZ,processClass,valueX,valueY,valueZ,weight);	}


// ###########################
// #     Plots management    #
// ###########################
void SonicScrewdriver::SchedulePlots(string plotType, string options)
{
    thePlotScrewdriver.SchedulePlots(plotType,options);
}

void SonicScrewdriver::MakePlots(string options)
{
    thePlotScrewdriver.MakePlots(&theVariables,&theProcessClasses,&theRegions,&theChannels,&theHistoScrewdriver,theLumi,options);
}

void SonicScrewdriver::WritePlots(string outputFolder, string infoText, string options)
{
	thePlotScrewdriver.WritePlots(&theChannels, &theRegions, outputFolder,infoText,options);
}



Figure SonicScrewdriver::GetYieldAndError(string var, string processClass,string region,string channel)
{
    return theHistoScrewdriver.GetYieldAndError(var,processClass,region,channel);
}

vector<Histo2D>* SonicScrewdriver::Get2DHistoList()
{
    return theHistoScrewdriver.Get2DHistoList();
}

