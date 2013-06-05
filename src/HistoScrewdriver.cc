
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
    AutoFill1DProcessClass(processClass,weight);
    AutoFill2DProcessClass(processClass,weight);
    AutoFill3DProcessClass(processClass,weight);
}

// ###########################
// #   1D histo management   #
// ###########################

int HistoScrewdriver::getIndexOfHisto1D(string tagVar, string tagProcessClass, string tagRegion, string tagChannel)
{
  for (unsigned int i = 0 ; i < the1DHistos.size() ; i++)
  {
    if (the1DHistos[i].getChannelTag()      != tagChannel     ) continue;
    if (the1DHistos[i].getRegionTag()       != tagRegion      ) continue;
    if (the1DHistos[i].getProcessClassTag() != tagProcessClass) continue;
    if (the1DHistos[i].getVariableTag()     != tagVar         ) continue;
    return i;
  }
  WARNING_MSG << "Unable to find histo for (var,processClass,region,channel) "
                                   << "= (" << tagVar << "," 
                     << tagProcessClass << "," 
                     << tagRegion       << ","
                     << tagChannel      << ")" << endl;
  return -1;
}
 
void HistoScrewdriver::Create1DHistos()
{
    // #########################
    // #  Loop over variables  #
    // #########################
    for (unsigned int v = 0 ; v < theVariables->size()     ; v++)
    {
       bool autoFill = (*theVariables)[v].haveFillPointer();
      
       // ############################################
       // #  Loop over channels and process classes  #
       // ############################################
       for (unsigned int r = 0 ; r < theRegions->size()        ; r++)
       for (unsigned int c = 0 ; c < theChannels->size()       ; c++)
       for (unsigned int p = 0 ; p < theProcessClasses->size() ; p++)
       {
            Histo1D newHisto(
                  &((*theVariables)[v]),
                  &((*theProcessClasses)[p]),
                  &((*theRegions)[r]),
                  &((*theChannels)[c]),
                  autoFill
                 );
            the1DHistos.push_back(newHisto);
       }
    }
}

void HistoScrewdriver::Fill(string var,  string processClass, float value, float weight)
{
  for (unsigned int i = 0 ; i < the1DHistos.size() ; i++)
  {
    if (the1DHistos[i].getVariableTag()     != var) continue;
    if (the1DHistos[i].getProcessClassTag() != processClass) continue;

    // Check region and channel selection
    if (!the1DHistos[i].getRegion()->passSelection()) continue;
    if (!the1DHistos[i].getChannel()->passSelection()) continue;

    the1DHistos[i].Fill(value,weight);
  }
}

void HistoScrewdriver::Fill(string var,  string processClass, string region, string channel, float value, float weight)
{
  int indexHisto = getIndexOfHisto1D(var,processClass,region,channel);
  if (indexHisto < 0) return;
  else                return the1DHistos[indexHisto].Fill(value,weight);
}

void HistoScrewdriver::AutoFill1DProcessClass(string processClass, float weight)
{
  for (unsigned int i = 0 ; i < the1DHistos.size() ; i++)
  {
    if (the1DHistos[i].getProcessClassTag() != processClass) continue;
    if (!(the1DHistos[i].doAutoFill())) continue;

    // Check region and channel selection
    if (!the1DHistos[i].getRegion()->passSelection()) continue;
    if (!the1DHistos[i].getChannel()->passSelection()) continue;

    the1DHistos[i].AutoFill(weight);
  }
}
 
TH1F* HistoScrewdriver::get1DHistoClone(string var, string processClass, string region, string channel)
{
  int indexHisto = getIndexOfHisto1D(var,processClass,region,channel);
    if (indexHisto < 0) return 0;
  else         return the1DHistos[indexHisto].getClone();
}

TH1F* HistoScrewdriver::get1DHistoEntriesClone(string var, string processClass, string region, string channel)
{
  int indexHisto = getIndexOfHisto1D(var,processClass,region,channel);
    if (indexHisto < 0) return 0;
  else         return the1DHistos[indexHisto].getEntriesClone();
}

void HistoScrewdriver::ApplyScaleFactor(string var, string processClass, string region, string channel, Figure scaleFactor)
{
  int indexHisto = getIndexOfHisto1D(var,processClass,region,channel);
  if (indexHisto < 0) return;
  else                return the1DHistos[indexHisto].ApplyScaleFactor(scaleFactor);
}

// ###########################
// #   2D histo management   #
// ###########################

int HistoScrewdriver::getIndexOfHisto2D(string tagVarX, string tagVarY, string tagProcessClass, string tagRegion, string tagChannel)
{
  for (unsigned int i = 0 ; i < the2DHistos.size() ; i++)
  {
    if (the2DHistos[i].getVariableXTag()    != tagVarX   ) continue;
    if (the2DHistos[i].getVariableYTag()    != tagVarY   ) continue;
    if (the2DHistos[i].getProcessClassTag() != tagProcessClass) continue;
    if (the2DHistos[i].getRegionTag()       != tagRegion ) continue;
    if (the2DHistos[i].getChannelTag()      != tagChannel) continue;
    return i;
  }
  return -1;
}
 
void HistoScrewdriver::Add2DHisto(string varX, string varY, bool autoFill, int nBinsX, float minX, float maxX, int nBinsY, float minY, float maxY)
{

   int indexVarX = getIndexOfVariable(varX);
   int indexVarY = getIndexOfVariable(varY);

   bool autoFillVarX = false; 
   bool autoFillVarY = false;
   bool autoFill_ = false;

   for (unsigned int v = 0 ; v < theVariables->size()     ; v++)
   {
        if (((int) v) == indexVarX) autoFillVarX = (*theVariables)[v].haveFillPointer();
        if (((int) v) == indexVarY) autoFillVarY = (*theVariables)[v].haveFillPointer(); 
   }
      
   if ((autoFillVarX) && (autoFillVarY)) autoFill_ = true;

   // ############################################
   // #  Loop over channels and process classes  #
   // ############################################
   for (unsigned int r = 0 ; r < theRegions->size()        ; r++)
   for (unsigned int c = 0 ; c < theChannels->size()       ; c++)
   for (unsigned int p = 0 ; p < theProcessClasses->size() ; p++)
   {
        Histo2D newHisto(
          &((*theVariables)[indexVarX]),
          &((*theVariables)[indexVarY]),
          &((*theProcessClasses)[p]),
          &((*theRegions)[r]),
          &((*theChannels)[c]),
          autoFill_,
          nBinsX,minX,maxX,
          nBinsY,minY,maxY);

        the2DHistos.push_back(newHisto);
    }
}

void HistoScrewdriver::Fill(string varX, string varY, string processClass, float valueX, float valueY, float weight)
{

  for (unsigned int i = 0 ; i < the2DHistos.size() ; i++)
  {
    if (the2DHistos[i].getProcessClassTag() != processClass) continue;
    if (the2DHistos[i].getVariableXTag()    != varX   ) continue;
    if (the2DHistos[i].getVariableYTag()    != varY   ) continue;
    if (!the2DHistos[i].getRegion()->passSelection()) continue;
    if (!the2DHistos[i].getChannel()->passSelection()) continue;
    the2DHistos[i].Fill(valueX,valueY,weight);
  }

}
void HistoScrewdriver::AutoFill2DProcessClass(string processClass, float weight)
{
  for (unsigned int i = 0 ; i < the2DHistos.size() ; i++)
  {
    if (the2DHistos[i].getProcessClassTag() != processClass) continue;
    if (!(the2DHistos[i].doAutoFill())) continue;

    // Check region and channel selection
    if (!the2DHistos[i].getRegion()->passSelection()) continue;
    if (!the2DHistos[i].getChannel()->passSelection()) continue;

    the2DHistos[i].AutoFill(weight);
  }
}

Histo2D* HistoScrewdriver::get2DHistoPointer(string varX, string varY, string processClass, string region, string channel)
{
    int indexHisto = getIndexOfHisto2D(varX,varY,processClass,region,channel);
    if (indexHisto == -1) 
    {   
        WARNING_MSG << "Unable to find histo for (varX,varY,processClass,region,channel) "
                                       << "= (" << varX          << "," 
                                                << varY          << "," 
                                                << processClass  << "," 
                                                << region        << ","
                                                << channel        << ")" << endl;
        return 0; 
    }
    return &(the2DHistos[indexHisto]);
}

TH2F* HistoScrewdriver::get2DHistoClone(string varX, string varY, string processClass, string region, string channel)
{
    return get2DHistoPointer(varX,varY,processClass,region,channel)->getClone();
}

TH2F* HistoScrewdriver::get2DHistoEntriesClone(string varX, string varY, string processClass, string region, string channel)
{
    return get2DHistoPointer(varX,varY,processClass,region,channel)->getEntriesClone();
}

vector<Histo2D>* HistoScrewdriver::Get2DHistoList()
{
    return &(the2DHistos);
}



// ###########################
// #   3D histo management   #
// ###########################

int HistoScrewdriver::getIndexOfHisto3D(string tagVarX, string tagVarY, string tagVarZ,
                                        string tagProcessClass, string tagRegion, string tagChannel)
{
  for (unsigned int i = 0 ; i < the3DHistos.size() ; i++)
  {
    if (the3DHistos[i].getVariableXTag()    != tagVarX   ) continue;
    if (the3DHistos[i].getVariableYTag()    != tagVarY   ) continue;
    if (the3DHistos[i].getVariableZTag()    != tagVarZ   ) continue;
    if (the3DHistos[i].getProcessClassTag() != tagProcessClass) continue;
    if (the3DHistos[i].getRegionTag()       != tagRegion ) continue;
    if (the3DHistos[i].getChannelTag()      != tagChannel) continue;
    return i;
  }
  return -1;
}
 
void HistoScrewdriver::Add3DHisto(string varX, string varY, string varZ, bool autoFill, 
                                  int nBinsX, float minX, float maxX, 
                                  int nBinsY, float minY, float maxY, 
                                  int nBinsZ, float minZ, float maxZ)
{
  
   unsigned int indexVarX = getIndexOfVariable(varX);
   unsigned int indexVarY = getIndexOfVariable(varY);
   unsigned int indexVarZ = getIndexOfVariable(varZ);

   bool autoFillVarX = false; 
   bool autoFillVarY = false;
   bool autoFillVarZ = false;
   bool autoFill_ = false;

   for (unsigned int v = 0 ; v < theVariables->size()     ; v++)
   {
        if (v == indexVarX) autoFillVarX = (*theVariables)[v].haveFillPointer();
        if (v == indexVarY) autoFillVarY = (*theVariables)[v].haveFillPointer();
        if (v == indexVarZ) autoFillVarZ = (*theVariables)[v].haveFillPointer();
   }

   if ((autoFillVarX) && (autoFillVarY) && (autoFillVarZ)) autoFill_ = true;

   // ############################################
   // #  Loop over channels and process classes  #
   // ############################################
   for (unsigned int r = 0 ; r < theRegions->size()        ; r++)
   for (unsigned int c = 0 ; c < theChannels->size()       ; c++)
   for (unsigned int p = 0 ; p < theProcessClasses->size() ; p++)
   {
        Histo3D newHisto(
          &((*theVariables)[indexVarX]),
          &((*theVariables)[indexVarY]),
          &((*theVariables)[indexVarZ]),
          &((*theProcessClasses)[p]),
          &((*theRegions)[r]),
          &((*theChannels)[c]),
          autoFill_,
          nBinsX,minX,maxX,
          nBinsY,minY,maxY,
          nBinsZ,minZ,maxZ);

        the3DHistos.push_back(newHisto);
   }
}

void HistoScrewdriver::Fill(string varX, string varY, string varZ, string processClass, float valueX, float valueY, float valueZ, float weight)
{

  for (unsigned int i = 0 ; i < the3DHistos.size() ; i++)
  {
    if (the3DHistos[i].getProcessClassTag() != processClass) continue;
    if (the3DHistos[i].getVariableXTag()    != varX   ) continue;
    if (the3DHistos[i].getVariableYTag()    != varY   ) continue;
    if (the3DHistos[i].getVariableZTag()    != varZ   ) continue;
    if (!the3DHistos[i].getRegion()->passSelection()) continue;
    if (!the3DHistos[i].getChannel()->passSelection()) continue;
    the3DHistos[i].Fill(valueX,valueY,valueZ,weight);
  }

}

void HistoScrewdriver::AutoFill3DProcessClass(string processClass, float weight)
{
  for (unsigned int i = 0 ; i < the3DHistos.size() ; i++)
  {
    if (the3DHistos[i].getProcessClassTag() != processClass) continue;
    if (!(the3DHistos[i].doAutoFill())) continue;

    // Check region and channel selection
    if (!the3DHistos[i].getRegion()->passSelection()) continue;
    if (!the3DHistos[i].getChannel()->passSelection()) continue;

    the3DHistos[i].AutoFill(weight);
  }
}

Histo3D* HistoScrewdriver::get3DHistoPointer(string varX, string varY, string varZ, string processClass, string region, string channel)
{
    int indexHisto = getIndexOfHisto3D(varX,varY,varZ,processClass,region,channel);
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
    return &(the3DHistos[indexHisto]);
}

TH3F* HistoScrewdriver::get3DHistoClone(string varX, string varY, string varZ, string processClass, string region, string channel)
{
    return get3DHistoPointer(varX,varY,varZ,processClass,region,channel)->getClone();
}

TH3F* HistoScrewdriver::get3DHistoEntriesClone(string varX, string varY, string varZ, string processClass, string region, string channel)
{
    return get3DHistoPointer(varX,varY,varZ,processClass,region,channel)->getEntriesClone();
}

vector<Histo3D>* HistoScrewdriver::Get3DHistoList()
{
    return &(the3DHistos);
}













Figure HistoScrewdriver::GetYieldAndError(string var, string processClass, string region, string channel)
{
    int indexHisto = getIndexOfHisto1D(var,processClass,region,channel);
    if (indexHisto < 0) return Figure(0.0,0.0);
    else         return the1DHistos[indexHisto].getYieldAndError();
}
