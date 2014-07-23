
#include "interface/SonicScrewdriver.h"

using namespace theDoctor;

string floatToString(float input)
{
    std::ostringstream ss;
    ss.precision(3);
    ss << input;
    return ss.str();
}

int sonicScrewdriver_DummyUnity = 1;

SonicScrewdriver::SonicScrewdriver():
    theHistoScrewdriver(&theVariables,&theProcessClasses,&theRegions,&theChannels)
{
    AddVariable("yield","Yield","",1,0,0,&sonicScrewdriver_DummyUnity,"");
}

SonicScrewdriver::~SonicScrewdriver()
{ }

// #################################
// #   Variables & processClasses  #
// #          management           #
// #################################

void SonicScrewdriver::AddVariable(string tag, string plotLabel, string unit, int nBins, float min, float max, float*  autoFillPointer, string options)
{    theVariables.push_back(Variable(tag,plotLabel,unit,nBins,min,max,autoFillPointer,options));    }
void SonicScrewdriver::AddVariable(string tag, string plotLabel, string unit, int nBins, float min, float max, double* autoFillPointer, string options)
{    theVariables.push_back(Variable(tag,plotLabel,unit,nBins,min,max,autoFillPointer,options));    }
void SonicScrewdriver::AddVariable(string tag, string plotLabel, string unit, int nBins, int min, int max, int*  autoFillPointer, string options)
{    theVariables.push_back(Variable(tag,plotLabel,unit,nBins,min,max,autoFillPointer,options));    }
void SonicScrewdriver::AddVariable(string tag, string plotLabel, string unit, int nBins, float* binning, float*  autoFillPointer, string options)
{    theVariables.push_back(Variable(tag,plotLabel,unit,nBins,binning,autoFillPointer,options));    }
void SonicScrewdriver::AddVariable(string tag, string plotLabel, string unit, int nBins, float* binning, double* autoFillPointer, string options)
{    theVariables.push_back(Variable(tag,plotLabel,unit,nBins,binning,autoFillPointer,options));    }
void SonicScrewdriver::AddVariable(string tag, string plotLabel, string unit, int nBins, float* binning, int*    autoFillPointer, string options)
{    theVariables.push_back(Variable(tag,plotLabel,unit,nBins,binning,autoFillPointer,options));    }


int SonicScrewdriver::getIndexOfVariable(string tag)
{
    for (unsigned int i = 0 ; i < theVariables.size() ; i++)
        if (theVariables[i].getTag() == tag) return i;

    return -1;
}


void SonicScrewdriver::AddProcessClass(string tag, string plotLabel, string type, Color_t color, string options)
{    theProcessClasses.push_back(ProcessClass(tag,plotLabel,type,color,options));    }

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

void SonicScrewdriver::GetProcessClassTagList(vector<string> *output)
{
    output->clear();
    for (unsigned int i = 0 ; i < theProcessClasses.size() ; i++)
        output->push_back(theProcessClasses[i].getTag());
}

void SonicScrewdriver::GetProcessClassLabelList(vector<string> *output)
{
    output->clear();
    for (unsigned int i = 0 ; i < theProcessClasses.size() ; i++)
        output->push_back(theProcessClasses[i].getLabel());
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
        if ((type == "background") || (type == "signal"))
            return theDatasets[i].getXsecOrLumi() * theLumi / theDatasets[i].getTrueNumberOfEvents();
        else if (type == "data")
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
{ return theLumi; }

void SonicScrewdriver::SetLumi(float inputLumi)
{ theLumi = inputLumi; }

// #####################################
// #   Regions & channels management   #
// #####################################

void  SonicScrewdriver::AddRegion(string tag, string plotLabel, bool (*selector)(), string options)
{    theRegions.push_back(Region(tag,plotLabel,selector,options));    }

void  SonicScrewdriver::AddRegion(string tag, string plotLabel, string parentRegionTag, bool (*selector)(), string options)
{    
    int parentRegionIndex = -1;
    for (unsigned int r = 0 ; r < theRegions.size() ; r++)
    {
        if (theRegions[r].getTag() == parentRegionTag) { parentRegionIndex = r; break; }
    }
    if (parentRegionIndex == -1)
    {
        WARNING_MSG << "Cannot find parent region '" << parentRegionTag << "' for region '" << tag << "'." << endl;
        return;
    }

    theRegions.push_back(Region(tag,plotLabel,selector,options,pair<vector<Region>*,int>(&theRegions,parentRegionIndex)));    
}

void  SonicScrewdriver::AddRegion(string tag, string plotLabel, vector<Cut> setOfCuts, string options)
{    theRegions.push_back(Region(tag,plotLabel,setOfCuts,&theVariables,options));    }

void  SonicScrewdriver::AddRegion(string tag, string plotLabel, string parentRegionTag, vector<Cut> setOfCuts, string options)
{ 
    int parentRegionIndex = -1;
    for (unsigned int r = 0 ; r < theRegions.size() ; r++)
    {
        if (theRegions[r].getTag() == parentRegionTag) { parentRegionIndex = r; break; }
    }
    if (parentRegionIndex == -1)
    {
        WARNING_MSG << "Cannot find parent region '" << parentRegionTag << "' for region '" << tag << "'." << endl;
        return;
    }

    theRegions.push_back(Region(tag,plotLabel,setOfCuts,&theVariables,options,pair<vector<Region>*,int>(&theRegions,parentRegionIndex)));    
}

void SonicScrewdriver::GetRegionTagList(vector<string> *output)
{
    output->clear();
    for (unsigned int i = 0 ; i < theRegions.size() ; i++)
        output->push_back(theRegions[i].getTag());
}

void SonicScrewdriver::GetRegionLabelList(vector<string> *output)
{
    output->clear();
    for (unsigned int i = 0 ; i < theRegions.size() ; i++)
        output->push_back(theRegions[i].getLabel());
}

void SonicScrewdriver::AddChannel(string tag, string plotLabel, bool (*selector)(), string options)
{    theChannels.push_back(Channel(tag,plotLabel,selector,options));    }

// ###########################
// #    Histo management     #
// ###########################

void SonicScrewdriver::AutoFillProcessClass(string processClass, float weight)
{    theHistoScrewdriver.AutoFillProcessClass(processClass,weight);    }

// ###########################
// #   1D histo management   #
// ###########################

void SonicScrewdriver::Create1DHistos()
{    theHistoScrewdriver.Create1DHistosEntries();    }

void SonicScrewdriver::ApplyScaleFactor(string var, string processClass, string region, string channel, Figure scaleFactor)
{    theHistoScrewdriver.ApplyScaleFactor(var,processClass,region, channel, scaleFactor);    }

void SonicScrewdriver::ApplyScaleFactor(string processClass, string region, string channel, Figure scaleFactor)
{   
    for (unsigned int i = 0 ; i < theVariables.size() ; i++)
        theHistoScrewdriver.ApplyScaleFactor(theVariables[i].getTag(),processClass,region, channel, scaleFactor);    
}

// ###########################
// #   2D histo management   #
// ###########################

void SonicScrewdriver::Add2DHisto(string varX, string varY)
{ theHistoScrewdriver.Add2DHistoEntries(varX,varY); }

// ###########################
// #   3D histo management   #
// ###########################

void SonicScrewdriver::Add3DHisto(string varX, string varY, string varZ) 
{ theHistoScrewdriver.Add3DHistoEntries(varX,varY,varZ); }

// ###########################
// #     Plots management    #
// ###########################
void SonicScrewdriver::SchedulePlots(string plotType, string options)
{ thePlotScrewdriver.SchedulePlots(plotType,options); }

void SonicScrewdriver::MakePlots(string options)
{ thePlotScrewdriver.MakePlots(&theVariables,&theProcessClasses,&theRegions,&theChannels,&theFiguresPerProcessMap,&theFiguresMap,&theHistoScrewdriver,options); }

void SonicScrewdriver::WritePlots(string outputFolder)
{ thePlotScrewdriver.WritePlots(&theChannels, &theRegions, outputFolder); }

Figure SonicScrewdriver::GetYieldAndError(string processClass,string region,string channel)
{ return theHistoScrewdriver.GetYieldAndError(processClass,region,channel); }

vector<Histo1DEntries>* SonicScrewdriver::Get1DHistosEntries()
{ return theHistoScrewdriver.Get1DHistosEntries(); }

vector<Histo2DEntries>* SonicScrewdriver::Get2DHistosEntries()
{ return theHistoScrewdriver.Get2DHistosEntries(); }

vector<Histo3DEntries>* SonicScrewdriver::Get3DHistosEntries()
{ return theHistoScrewdriver.Get3DHistosEntries(); }

TH1F* SonicScrewdriver::get1DHistoClone(string var, string processClass, string region, string channel)
{ return theHistoScrewdriver.get1DHistoClone(var,processClass,region,channel); }

TH1F* SonicScrewdriver::get1DCompositeHistoClone(string var, string type, string region, string channel, string otherParameters)
{ return theHistoScrewdriver.get1DCompositeHistoClone(var,type,region,channel,otherParameters); }

TH2F* SonicScrewdriver::get2DHistoClone(string varX, string varY, string processClass, string region, string channel)
{ return theHistoScrewdriver.get2DHistoClone(varX,varY,processClass,region,channel); }

TH2F* SonicScrewdriver::get2DCompositeHistoClone(string varX, string varY, string type, string region, string channel, string otherParameters)
{ return theHistoScrewdriver.get2DCompositeHistoClone(varX,varY,type,region,channel,otherParameters); }





      // #########################
      // #   Figure management   #
      // #########################
      
void SonicScrewdriver::AddFigurePerProcess(string tag, string label, string options)
{
    theFiguresPerProcessMap.push_back(pair<Name,Map3DFigure>(Name(tag, label, options), Map3DFigure()));
}

void SonicScrewdriver::AddFigure          (string tag, string label, string options)
{
    theFiguresMap.push_back(pair<Name,Map2DFigure>(Name(tag, label, options), Map2DFigure()));
}

void SonicScrewdriver::SetFigure(string tag, string process, string region, string channel, Figure figureValue)
{
   for (unsigned int f = 0 ; f < theFiguresPerProcessMap.size() ; f++)
   {
      if (theFiguresPerProcessMap[f].first.getTag() != tag) continue;
      
      Map3DFigure* theMap = &(theFiguresPerProcessMap[f].second);
      (*theMap)[process][region][channel] = figureValue;

      break;
   }
}

void SonicScrewdriver::SetFigure(string tag, string region, string channel, Figure figureValue)
{
   for (unsigned int f = 0 ; f < theFiguresMap.size() ; f++)
   {
      if (theFiguresMap[f].first.getTag() != tag) continue;

      Map2DFigure* theMap = &(theFiguresMap[f].second);
      (*theMap)[region][channel] = figureValue;

      break;
   }
}

