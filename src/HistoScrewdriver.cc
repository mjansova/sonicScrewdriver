
#include "interface/HistoScrewdriver.h"

using namespace theDoctor;

HistoScrewdriver::HistoScrewdriver(vector<Variable>* variables, 
                               vector<ProcessClass>* processClasses, 
                   vector<Region>* regions, 
                   vector<Channel>* channels)
{
  theVariables = variables;
  theProcessClasses = processClasses;
  theRegions = regions;
  theChannels = channels;
}


HistoScrewdriver:: ~HistoScrewdriver() {}

int HistoScrewdriver::getIndexOfVariable(string tag)
{
  for (unsigned int i = 0 ; i < theVariables->size() ; i++)
    if ((*theVariables)[i].getTag() == tag) return i;

  WARNING_MSG << "Didn't find index of variable named '" << tag << "'." << endl;
  return -1;
}

int HistoScrewdriver::getIndexOfProcessClass(string tag)
{
  for (unsigned int i = 0 ; i < theProcessClasses->size() ; i++)
    if ((*theProcessClasses)[i].getTag() == tag) return i;
  WARNING_MSG << "Didn't find index of process class named '" << tag << "'." << endl;
  return -1;
}

int HistoScrewdriver::getIndexOfRegion(string tag)
{
  for (unsigned int i = 0 ; i < theRegions->size() ; i++)
    if ((*theRegions)[i].getTag() == tag) return i;
  WARNING_MSG << "Didn't find index of region named '" << tag << "'." << endl;
  return -1;
}

int HistoScrewdriver::getIndexOfChannel(string tag)
{
  for (unsigned int i = 0 ; i < theChannels->size() ; i++)
    if ((*theChannels)[i].getTag() == tag) return i;
  WARNING_MSG << "Didn't find index of channel named '" << tag << "'." << endl;
  return -1;
}


void HistoScrewdriver::AutoFillProcessClass(string processClass, float weight)
{
    UpdateRegionsAndChannels();
    AutoFill1DProcessClass(processClass,weight);
    AutoFill2DProcessClass(processClass,weight);
    AutoFill3DProcessClass(processClass,weight);
}

void HistoScrewdriver::UpdateRegionsAndChannels()
{
    for (int i = 0 ; i < theRegions->size() ; i++)
        (*theRegions)[i].updateSelectionFlag();

    for (int i = 0 ; i < theChannels->size() ; i++)
        (*theChannels)[i].updateSelectionFlag();
}

// ###########################
// #   1D histo management   #
// ###########################

int HistoScrewdriver::getIndexOfHisto1DEntries(string tagVar, string tagProcessClass, string tagRegion, string tagChannel)
{
  for (unsigned int i = 0 ; i < the1DHistosEntries.size() ; i++)
  {
    if (the1DHistosEntries[i].getChannelTag()      != tagChannel     ) continue;
    if (the1DHistosEntries[i].getRegionTag()       != tagRegion      ) continue;
    if (the1DHistosEntries[i].getProcessClassTag() != tagProcessClass) continue;
    if (the1DHistosEntries[i].getVariableTag()     != tagVar         ) continue;
    return i;
  }
  WARNING_MSG << "Unable to find histo for (var,processClass,region,channel) "
                                   << "= (" << tagVar << "," 
                     << tagProcessClass << "," 
                     << tagRegion       << ","
                     << tagChannel      << ")" << endl;
  return -1;
}

int HistoScrewdriver::getIndexOfHisto1DForPlot(string tagType, string tagVar, string tagRegion, string tagChannel, string otherParameters)
{
  for (unsigned int i = 0 ; i < the1DHistosForPlots.size() ; i++)
  {
    /*
    DEBUG_MSG <<  "t = " << the1DHistosForPlots[i].getHistoTypeTag()
              << " c = " << the1DHistosForPlots[i].getChannelTag()  
              << " r = " << the1DHistosForPlots[i].getRegionTag()   
              << " v = " << the1DHistosForPlots[i].getVariableTag() 
              << " param = " << the1DHistosForPlots[i].getHistoParameters() 
              << endl;
    */

    if (the1DHistosForPlots[i].getHistoTypeTag()    != tagType         ) continue;
    if (the1DHistosForPlots[i].getChannelTag()      != tagChannel      ) continue;
    if (the1DHistosForPlots[i].getRegionTag()       != tagRegion       ) continue;
    if (the1DHistosForPlots[i].getVariableTag()     != tagVar          ) continue;
    if (the1DHistosForPlots[i].getHistoParameters() != otherParameters ) continue;
    
    return i;
  }
  WARNING_MSG << "Unable to find histo for (type,var,region,channel,parameters) "
              << "= (" << tagType    << ","
                       << tagVar     << "," 
                       << tagRegion  << ","
                       << tagChannel << "," 
                       << otherParameters << ")" << endl;
  return -1;
}

void HistoScrewdriver::Create1DHistosEntries()
{
    // #########################
    // #  Loop over variables  #
    // #########################
    for (unsigned int v = 0 ; v < theVariables->size()     ; v++)
    {
      
       // ############################################
       // #  Loop over channels and process classes  #
       // ############################################
       for (unsigned int r = 0 ; r < theRegions->size()        ; r++)
       for (unsigned int c = 0 ; c < theChannels->size()       ; c++)
       for (unsigned int p = 0 ; p < theProcessClasses->size() ; p++)
       {
            Histo1DEntries newHisto(
                  &((*theVariables)[v]),
                  &((*theProcessClasses)[p]),
                  &((*theRegions)[r]),
                  &((*theChannels)[c])
                 );
            the1DHistosEntries.push_back(newHisto);
       }
    }
}

void HistoScrewdriver::AutoFill1DProcessClass(string processClass, float weight)
{
  for (unsigned int i = 0 ; i < the1DHistosEntries.size() ; i++)
  {
    if (the1DHistosEntries[i].getProcessClassTag() != processClass) continue;

    // Check region and channel selection
    if (!the1DHistosEntries[i].getRegion()->getSelectionFlag()) continue;
    if (!the1DHistosEntries[i].getChannel()->getSelectionFlag()) continue;

    the1DHistosEntries[i].AutoFill(weight);
  }
}

Histo1DEntries* HistoScrewdriver::get1DHistoEntriesPointer(string var, string processClass, string region, string channel)
{
    int indexHisto = getIndexOfHisto1DEntries(var,processClass,region,channel);

    if (indexHisto < 0) return 0;
    else return &(the1DHistosEntries[indexHisto]);
}

Histo1D* HistoScrewdriver::get1DHistoForPlotPointer(string type, string var, string region, string channel, string otherParameters)
{
    int indexHisto = getIndexOfHisto1DForPlot(type,var,region,channel,otherParameters);
    if (indexHisto < 0) return 0;
    else return &(the1DHistosForPlots[indexHisto]);
}

TH1F* HistoScrewdriver::get1DHistoClone(string var, string processClass, string region, string channel)
{
    int indexHisto = getIndexOfHisto1DEntries(var,processClass,region,channel);
    if (indexHisto < 0) return 0;
    else return the1DHistosEntries[indexHisto].getClone();
}

TH1F* HistoScrewdriver::get1DHistoEntriesClone(string var, string processClass, string region, string channel)
{
  int indexHisto = getIndexOfHisto1DEntries(var,processClass,region,channel);
    if (indexHisto < 0) return 0;
  else         return the1DHistosEntries[indexHisto].getEntriesClone();
}

void HistoScrewdriver::ApplyScaleFactor(string var, string processClass, string region, string channel, Figure scaleFactor)
{
  int indexHisto = getIndexOfHisto1DEntries(var,processClass,region,channel);
  if (indexHisto < 0) return;
  else                return the1DHistosEntries[indexHisto].ApplyScaleFactor(scaleFactor);
}

vector<Histo1DEntries>* HistoScrewdriver::Get1DHistosEntries()
{
    return &(the1DHistosEntries);
}

// ###########################
// #   2D histo management   #
// ###########################

int HistoScrewdriver::getIndexOfHisto2DEntries(string tagVarX, string tagVarY, string tagProcessClass, string tagRegion, string tagChannel)
{
  for (unsigned int i = 0 ; i < the2DHistosEntries.size() ; i++)
  {
    if (the2DHistosEntries[i].getVariableXTag()    != tagVarX   ) continue;
    if (the2DHistosEntries[i].getVariableYTag()    != tagVarY   ) continue;
    if (the2DHistosEntries[i].getProcessClassTag() != tagProcessClass) continue;
    if (the2DHistosEntries[i].getRegionTag()       != tagRegion ) continue;
    if (the2DHistosEntries[i].getChannelTag()      != tagChannel) continue;
    return i;
  }
  WARNING_MSG << "Unable to find histo for (varX,varY,process,region,channel) "
              << "= (" << tagVarX         << ","
                       << tagVarY         << "," 
                       << tagProcessClass << "," 
                       << tagRegion       << ","
                       << tagChannel      << ")" << endl;
  return -1;
}

int HistoScrewdriver::getIndexOfHisto2DForPlot(string type, string tagVarX, string tagVarY, string tagRegion, string tagChannel, string otherParameters)
{
  for (unsigned int i = 0 ; i < the2DHistosForPlots.size() ; i++)
  {
/*
      DEBUG_MSG <<  "t = " << the2DHistosForPlots[i].getHistoTypeTag()
              << " c = " << the2DHistosForPlots[i].getChannelTag()  
              << " r = " << the2DHistosForPlots[i].getRegionTag()   
              << " vX = " << the2DHistosForPlots[i].getVariableXTag() 
              << " vY = " << the2DHistosForPlots[i].getVariableYTag() 
              << " p = " << the2DHistosForPlots[i].getHistoParameters() 
              << endl;
*/
    if (the2DHistosForPlots[i].getHistoTypeTag()    != type            ) continue;
    if (the2DHistosForPlots[i].getChannelTag()      != tagChannel      ) continue;
    if (the2DHistosForPlots[i].getRegionTag()       != tagRegion       ) continue;


    if (the2DHistosForPlots[i].getVariableXTag()    != tagVarX         ) continue;
    if (the2DHistosForPlots[i].getVariableYTag()    != tagVarY         ) continue;
    if (the2DHistosForPlots[i].getHistoParameters() != otherParameters ) continue;
    
    return i;
  }
  WARNING_MSG << "Unable to find histo for (type,varX,varY,region,channel,parameters) "
              << "= (" << type       << ","
                       << tagVarX    << "," 
                       << tagVarY    << "," 
                       << tagRegion  << ","
                       << tagChannel << "," 
                       << otherParameters << ")" << endl;
  return -1;
}

 
void HistoScrewdriver::Add2DHistoEntries(string varX, string varY)
{

   int indexVarX = getIndexOfVariable(varX);
   int indexVarY = getIndexOfVariable(varY);

   // ############################################
   // #  Loop over channels and process classes  #
   // ############################################
   for (unsigned int r = 0 ; r < theRegions->size()        ; r++)
   for (unsigned int c = 0 ; c < theChannels->size()       ; c++)
   for (unsigned int p = 0 ; p < theProcessClasses->size() ; p++)
   {
        Histo2DEntries newHisto(
          &((*theVariables)[indexVarX]),
          &((*theVariables)[indexVarY]),
          &((*theProcessClasses)[p]),
          &((*theRegions)[r]),
          &((*theChannels)[c]));

        the2DHistosEntries.push_back(newHisto);
    }
}

void HistoScrewdriver::AutoFill2DProcessClass(string processClass, float weight)
{
  for (unsigned int i = 0 ; i < the2DHistosEntries.size() ; i++)
  {
    if (the2DHistosEntries[i].getProcessClassTag() != processClass) continue;

    // Check region and channel selection
    if (!the2DHistosEntries[i].getRegion()->getSelectionFlag()) continue;
    if (!the2DHistosEntries[i].getChannel()->getSelectionFlag()) continue;

    the2DHistosEntries[i].AutoFill(weight);
  }
}

Histo2DEntries* HistoScrewdriver::get2DHistoEntriesPointer(string varX, string varY, string processClass, string region, string channel)
{
    int indexHisto = getIndexOfHisto2DEntries(varX,varY,processClass,region,channel);
    if (indexHisto < 0) return 0; 
    else return &(the2DHistosEntries[indexHisto]);
}

Histo2D* HistoScrewdriver::get2DHistoForPlotPointer(string type, string varX, string varY, string region, string channel, string otherParameters)
{
    int indexHisto = getIndexOfHisto2DForPlot(type,varX,varY,region,channel,otherParameters);
    if (indexHisto < 0) return 0;
    else return &(the2DHistosForPlots[indexHisto]);
}


TH2F* HistoScrewdriver::get2DHistoClone(string varX, string varY, string processClass, string region, string channel)
{
    int indexHisto = getIndexOfHisto2DEntries(varX,varY,processClass,region,channel);
    if (indexHisto < 0) return 0;
    else return the2DHistosEntries[indexHisto].getClone();
}

vector<Histo2DEntries>* HistoScrewdriver::Get2DHistosEntries()
{
    return &(the2DHistosEntries);
}



// ###########################
// #   3D histo management   #
// ###########################

int HistoScrewdriver::getIndexOfHisto3DEntries(string tagVarX, string tagVarY, string tagVarZ,
                                        string tagProcessClass, string tagRegion, string tagChannel)
{
  for (unsigned int i = 0 ; i < the3DHistosEntries.size() ; i++)
  {
    if (the3DHistosEntries[i].getVariableXTag()    != tagVarX   ) continue;
    if (the3DHistosEntries[i].getVariableYTag()    != tagVarY   ) continue;
    if (the3DHistosEntries[i].getVariableZTag()    != tagVarZ   ) continue;
    if (the3DHistosEntries[i].getProcessClassTag() != tagProcessClass) continue;
    if (the3DHistosEntries[i].getRegionTag()       != tagRegion ) continue;
    if (the3DHistosEntries[i].getChannelTag()      != tagChannel) continue;
    return i;
  }
  return -1;
}

int HistoScrewdriver::getIndexOfHisto3DForPlot(string type, string tagVarX, string tagVarY, string tagVarZ, string tagRegion, string tagChannel, string otherParameters)
{
  for (unsigned int i = 0 ; i < the3DHistosForPlots.size() ; i++)
  {
/*
      DEBUG_MSG <<  "t = " << the3DHistosForPlots[i].getHistoTypeTag()
              << " c = "   << the3DHistosForPlots[i].getChannelTag()  
              << " r = "   << the3DHistosForPlots[i].getRegionTag()   
              << " vX = "  << the3DHistosForPlots[i].getVariableXTag() 
              << " vY = "  << the3DHistosForPlots[i].getVariableYTag() 
              << " vZ = "  << the3DHistosForPlots[i].getVariableZTag() 
              << " p = "   << the3DHistosForPlots[i].getHistoParameters() 
              << endl;
*/
    if (the3DHistosForPlots[i].getHistoTypeTag()    != type            ) continue;
    if (the3DHistosForPlots[i].getChannelTag()      != tagChannel      ) continue;
    if (the3DHistosForPlots[i].getRegionTag()       != tagRegion       ) continue;
    if (the3DHistosForPlots[i].getVariableXTag()    != tagVarX         ) continue;
    if (the3DHistosForPlots[i].getVariableYTag()    != tagVarY         ) continue;
    if (the3DHistosForPlots[i].getVariableZTag()    != tagVarZ         ) continue;
    if (the3DHistosForPlots[i].getHistoParameters() != otherParameters ) continue;
    
    return i;
  }
  WARNING_MSG << "Unable to find histo for (type,varX,varY,varZ,region,channel,parameters) "
              << "= (" << type       << ","
                       << tagVarX    << "," 
                       << tagVarY    << ","
                       << tagVarZ    << ","
                       << tagRegion  << ","
                       << tagChannel << "," 
                       << otherParameters << ")" << endl;
  return -1;
}



void HistoScrewdriver::Add3DHistoEntries(string varX, string varY, string varZ) 
{
  
   unsigned int indexVarX = getIndexOfVariable(varX);
   unsigned int indexVarY = getIndexOfVariable(varY);
   unsigned int indexVarZ = getIndexOfVariable(varZ);

   // ############################################
   // #  Loop over channels and process classes  #
   // ############################################
   for (unsigned int r = 0 ; r < theRegions->size()        ; r++)
   for (unsigned int c = 0 ; c < theChannels->size()       ; c++)
   for (unsigned int p = 0 ; p < theProcessClasses->size() ; p++)
   {
        Histo3DEntries newHisto(
          &((*theVariables)[indexVarX]),
          &((*theVariables)[indexVarY]),
          &((*theVariables)[indexVarZ]),
          &((*theProcessClasses)[p]),
          &((*theRegions)[r]),
          &((*theChannels)[c]));

        the3DHistosEntries.push_back(newHisto);
   }
}

void HistoScrewdriver::AutoFill3DProcessClass(string processClass, float weight)
{
  for (unsigned int i = 0 ; i < the3DHistosEntries.size() ; i++)
  {
    if (the3DHistosEntries[i].getProcessClassTag() != processClass) continue;

    // Check region and channel selection
    if (!the3DHistosEntries[i].getRegion()->getSelectionFlag()) continue;
    if (!the3DHistosEntries[i].getChannel()->getSelectionFlag()) continue;

    the3DHistosEntries[i].AutoFill(weight);
  }
}

Histo3DEntries* HistoScrewdriver::get3DHistoEntriesPointer(string varX, string varY, string varZ, string processClass, string region, string channel)
{
    int indexHisto = getIndexOfHisto3DEntries(varX,varY,varZ,processClass,region,channel);
    if (indexHisto == -1) 
    {   
        WARNING_MSG << "Unable to find histo for (varX,varY,varZ,processClass,region,channel) "
                                       << "= (" << varX          << "," 
                                                << varY          << "," 
                                                << varZ          << "," 
                                                << processClass  << "," 
                                                << region        << ","
                                                << channel        << ")" << endl;
        return 0; 
    }
    return &(the3DHistosEntries[indexHisto]);
}

Histo3D* HistoScrewdriver::get3DHistoForPlotPointer(string type, string varX, string varY, string varZ, string region, string channel, string otherParameters)
{
    int indexHisto = getIndexOfHisto3DForPlot(type,varX,varY,varZ,region,channel,otherParameters);
    if (indexHisto < 0) return 0;
    else return &(the3DHistosForPlots[indexHisto]);
}

TH3F* HistoScrewdriver::get3DHistoClone(string varX, string varY, string varZ, string processClass, string region, string channel)
{
    int indexHisto = getIndexOfHisto3DEntries(varX,varY,varZ,processClass,region,channel);
    if (indexHisto < 0) return 0;
    else return the3DHistosEntries[indexHisto].getClone();
}

vector<Histo3DEntries>* HistoScrewdriver::Get3DHistosEntries()
{
    return &(the3DHistosEntries);
}


Figure HistoScrewdriver::GetYieldAndError(string var, string processClass, string region, string channel)
{
    int indexHisto = getIndexOfHisto1DEntries(var,processClass,region,channel);
    if (indexHisto < 0) return Figure(0.0,0.0);
    else         return the1DHistosEntries[indexHisto].GetYieldAndError();
}
