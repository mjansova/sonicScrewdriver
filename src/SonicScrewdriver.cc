
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
    // FIXME this should be handled an other way
    AddVariable("yield","Yield","",1,0,0,&sonicScrewdriver_DummyUnity,"");
    // FIXME this too
    PlotDefaultStyles::Set2DPalette(GetGlobalOptions(),"smooth");
    thePlotScrewdriver.SetGlobalFloatOption("DataMCRatio", "min", 0.5);
    thePlotScrewdriver.SetGlobalFloatOption("DataMCRatio", "max", 1.5);
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

void SonicScrewdriver::AddVariable(string tag, string plotLabel, string unit, int nBins, int min, int max,     int* (*autoFillFunction)(),   string options)
{    theVariables.push_back(Variable(tag,plotLabel,unit,nBins,min,max,autoFillFunction,options));    }
void SonicScrewdriver::AddVariable(string tag, string plotLabel, string unit, int nBins, float* binning,       int* (*autoFillFunction)(),   string options)
{    theVariables.push_back(Variable(tag,plotLabel,unit,nBins,binning,autoFillFunction,options));    }
void SonicScrewdriver::AddVariable(string tag, string plotLabel, string unit, int nBins, float min, float max, float* (*autoFillFunction)(), string options)
{    theVariables.push_back(Variable(tag,plotLabel,unit,nBins,min,max,autoFillFunction,options));    }
void SonicScrewdriver::AddVariable(string tag, string plotLabel, string unit, int nBins, float* binning,       float* (*autoFillFunction)(), string options)
{    theVariables.push_back(Variable(tag,plotLabel,unit,nBins,binning,autoFillFunction,options));    }

int SonicScrewdriver::GetIndexOfVariable(string tag)
{
    for (unsigned int i = 0 ; i < theVariables.size() ; i++)
        if (theVariables[i].getTag() == tag) return i;

    return -1;
}


void SonicScrewdriver::SetSizeOfVarArray(string tag, uint32_t arrSize)
{
    theVariables.at(GetIndexOfVariable(tag)).setArrSize(arrSize); 
}

void SonicScrewdriver::AddProcessClass(string tag, string plotLabel, string type, Color_t color, string options)
{    theProcessClasses.push_back(ProcessClass(tag,plotLabel,type,color,options));    }

void SonicScrewdriver::AddDataset(string tag, string processClass, int trueNumberOfEvents, float xsecOrLumi, string options)
{    
    theDatasets.push_back(Dataset(tag,processClass,trueNumberOfEvents,xsecOrLumi,options));    
    if (GetProcessClassType(processClass) == "data") theLumi += xsecOrLumi;
}


void SonicScrewdriver::SetTrueNumberOfEvents(string datasetTag, int n)
{
    Dataset* dataset = GetDataset(datasetTag);
    if (dataset != 0) dataset->setTrueNumberOfEvents(n);
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


float SonicScrewdriver::GetDatasetLumiWeight(string datasetTag)
{
    Dataset* dataset = GetDataset(datasetTag);
    if (dataset == 0) return 0.0;

    string type = GetProcessClassType(dataset->getProcessClass());

    if (type == "data") return 1.0;
   
    return dataset->getXsecOrLumi() * theLumi / dataset->getTrueNumberOfEvents();
}

float SonicScrewdriver::GetDatasetCrossSection(string datasetTag)
{
    Dataset* dataset = GetDataset(datasetTag);
    if (dataset == 0) return -1.0;
    
    string type = GetProcessClassType(dataset->getProcessClass());
    if (type == "data") return -1.0;

    return dataset->getXsecOrLumi();
}

string SonicScrewdriver::GetProcessClass(string datasetTag)
{
    Dataset* dataset = GetDataset(datasetTag);
    if (dataset != 0) return dataset->getProcessClass();
    else              return "";
}

Dataset* SonicScrewdriver::GetDataset(string datasetTag)
{
    for (unsigned int i = 0 ; i < theDatasets.size() ; i++)
    {
        if (theDatasets[i].getTag() != datasetTag) continue;
        return &(theDatasets[i]);
    }

    WARNING_MSG << "Could not find dataset named '" << datasetTag << "'" << endl;
    return 0;
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

vector<TH1D*> SonicScrewdriver::Get1DHistoCloneFromFile(string dir, string type, string var, vector<string> processClass, string region, string channel)
{ return theHistoScrewdriver.get1DHistoCloneFromFile(dir,type,var,processClass,region,channel); }

vector<TH1D*> SonicScrewdriver::Get1DHistoCanFileFromFile(TFile** file, TCanvas** can, string dir, string type, string var, vector<string> processClass, string region, string channel)
{ return theHistoScrewdriver.get1DHistoCanFileFromFile(file,can,dir,type,var,processClass,region,channel); }

Figure SonicScrewdriver::GetYieldAndError(string processClass,string region,string channel)
{ return theHistoScrewdriver.GetYieldAndError(processClass,region,channel); }

vector<Histo1DEntries>* SonicScrewdriver::Get1DHistosEntries()
{ return theHistoScrewdriver.Get1DHistosEntries(); }

vector<Histo2DEntries>* SonicScrewdriver::Get2DHistosEntries()
{ return theHistoScrewdriver.Get2DHistosEntries(); }

vector<Histo3DEntries>* SonicScrewdriver::Get3DHistosEntries()
{ return theHistoScrewdriver.Get3DHistosEntries(); }

TH1D* SonicScrewdriver::Get1DHistoClone(string var, string processClass, string region, string channel)
{ return theHistoScrewdriver.get1DHistoClone(var,processClass,region,channel); }

TH1D* SonicScrewdriver::Get1DCompositeHistoClone(string var, string type, string region, string channel, string otherParameters)
{ return theHistoScrewdriver.get1DCompositeHistoClone(var,type,region,channel,otherParameters); }

TH2D* SonicScrewdriver::Get2DHistoClone(string varX, string varY, string processClass, string region, string channel)
{ return theHistoScrewdriver.get2DHistoClone(varX,varY,processClass,region,channel); }

TH2D* SonicScrewdriver::Get2DCompositeHistoClone(string varX, string varY, string type, string region, string channel, string otherParameters)
{ return theHistoScrewdriver.get2DCompositeHistoClone(varX,varY,type,region,channel,otherParameters); }

void SonicScrewdriver::ExportHistosEntries(string outputFile)
{ 
    vector<Histo1DEntries>* Entries1D = Get1DHistosEntries();
    vector<Histo2DEntries>* Entries2D = Get2DHistosEntries();
    vector<Histo3DEntries>* Entries3D = Get3DHistosEntries();

    TFile* f = new TFile(outputFile.c_str(),"RECREATE");

    for (unsigned int i = 0 ; i < Entries1D->size() ; i++)
        Entries1D->at(i).getHisto()->Write();
    for (unsigned int i = 0 ; i < Entries2D->size() ; i++)
        Entries2D->at(i).getHisto()->Write();
    for (unsigned int i = 0 ; i < Entries3D->size() ; i++)
        Entries3D->at(i).getHisto()->Write();

    f->Close();
    
}

void SonicScrewdriver::ImportHistosEntries(string inputFile)
{ 
    TFile* f = new TFile(inputFile.c_str(),"READ");
    
    vector<Histo1DEntries>* Entries1D = Get1DHistosEntries();
    vector<Histo2DEntries>* Entries2D = Get2DHistosEntries();
    vector<Histo3DEntries>* Entries3D = Get3DHistosEntries();

    for (unsigned int i = 0 ; i < Entries1D->size() ; i++)
    {
        TH1D* localHisto = Entries1D->at(i).getHisto();
        string name = localHisto->GetName();
        TH1D* fileHisto = (TH1D*) f->Get(name.c_str());
        localHisto->Add(fileHisto);
    }
    for (unsigned int i = 0 ; i < Entries2D->size() ; i++)
    {
        TH2D* localHisto = Entries2D->at(i).getHisto();
        string name = localHisto->GetName();
        TH2D* fileHisto = (TH2D*) f->Get(name.c_str());
        localHisto->Add(fileHisto);
    }
    for (unsigned int i = 0 ; i < Entries3D->size() ; i++)
    {
        TH3D* localHisto = Entries3D->at(i).getHisto();
        string name = localHisto->GetName();
        TH3D* fileHisto = (TH3D*) f->Get(name.c_str());
        localHisto->Add(fileHisto);
    }

    f->Close();
    
}

void SonicScrewdriver::ImportHistosFromFile(string inputFile)
{ 
    TFile* f = new TFile(inputFile.c_str(),"READ");
    
    vector<Histo1DEntries>* Entries1D = Get1DHistosEntries();
    vector<Histo2DEntries>* Entries2D = Get2DHistosEntries();
    vector<Histo3DEntries>* Entries3D = Get3DHistosEntries();

    //cout<<Entries1D->size()<<" is the size"<<endl;
    for (unsigned int i = 0 ; i < Entries1D->size() ; i++)
    {
        string dir = Entries1D->at(i).getChannel()->getTag()+string("/")+Entries1D->at(i).getRegion()->getTag();
	string var = Entries1D->at(i).getVariable()->getTag();
	TH1D* localHisto = Entries1D->at(i).getHisto();
        string name = localHisto->GetName();
        //cout << "the name of the histogram: " << name << endl;
        //cout <<" search: "<<dir+name<<endl;
	//cout<<"dir = "<<dir<<endl;
	TDirectory* d = (TDirectory*) f->Get(dir.c_str());
	TCanvas* c = (TCanvas*) d->Get(var.c_str());
	TH1D* fileHisto = (TH1D*) c->GetPrimitive(name.c_str());
	localHisto->Add(fileHisto);
    }
    for (unsigned int i = 0 ; i < Entries2D->size() ; i++)
    {
        string dir = Entries2D->at(i).getChannel()->getTag()+string("/")+Entries2D->at(i).getRegion()->getTag()+string("/");
	string var ; // to be updated = Entries2D->at(i).getVariable()->getTag();
        TH2D* localHisto = Entries2D->at(i).getHisto();
        string name = localHisto->GetName();
	TDirectory* d = (TDirectory*) f->Get(dir.c_str());
	TCanvas* c = (TCanvas*) d->Get(var.c_str());
	TH2D* fileHisto = (TH2D*) c->GetPrimitive(name.c_str());
        localHisto->Add(fileHisto);
    }
    for (unsigned int i = 0 ; i < Entries3D->size() ; i++)
    {
        string dir = Entries3D->at(i).getChannel()->getTag()+string("/")+Entries3D->at(i).getRegion()->getTag()+string("/");
	string var ; // to be udpated = Entries3D->at(i).getVariable()->getTag();
        TH3D* localHisto = Entries3D->at(i).getHisto();
        string name = localHisto->GetName();
	TDirectory* d = (TDirectory*) f->Get(dir.c_str());
	TCanvas* c = (TCanvas*) d->Get(var.c_str());
	TH3D* fileHisto = (TH3D*) c->GetPrimitive(name.c_str());
        localHisto->Add(fileHisto);
    }

    f->Close();
    
}





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

void SonicScrewdriver::WriteXMLConfig(string outputfilename){
	ofstream ofile(outputfilename.c_str());
	
	//  header
	ofile<<"<?xml version=\"1.0\"?>"<<endl;

	//---------------------
	// -- Write Variables
	//---------------------
	ofile<<"<Variables>"<<endl;
	for(unsigned int i=0;i<theVariables.size();i++){
		ofile<<"<Variable tag=\""<<theVariables[i].getTag()<<"\"";
		ofile<<" label=\""<<theVariables[i].getLabel()<<"\"";
		ofile<<" unit=\""<<theVariables[i].getUnit()<<"\"";
		ofile<<" nbins=\""<<theVariables[i].getNbins()<<"\"";
		if(theVariables[i].getCustomBinning()==0){
			ofile<<"min=\""<<theVariables[i].getMin()<<"\"";
			ofile<<"max=\""<<theVariables[i].getMax()<<"\"";
		}
		else{
			ofile<<" binning=\"";
			for(int b=0;b<theVariables[i].getNbins();b++){
				ofile<<theVariables[i].getCustomBinning()[b];
				if(b!= theVariables[i].getNbins()-1) ofile<<"-";
			}
			ofile<<"\"";
		}
		ofile<<"/>"<<endl;
	}
	ofile<<"</Variables>"<<endl;
	
	//---------------------
	// -- Write Regions
	//---------------------
	ofile<<"<Regions>"<<endl;
	for(unsigned int i=0;i<theRegions.size();i++){
		ofile<<"<Region tag=\""<<theRegions[i].getTag()<<"\"";
		ofile<<" label=\""<<theRegions[i].getLabel()<<"\"";
		ofile<<" />"<<endl;
	}
	ofile<<"</Regions>"<<endl;


	//---------------------
	// -- Write Channels
	//---------------------
	ofile<<"<Channels>"<<endl;
	for(unsigned int i=0;i<theChannels.size();i++){
		ofile<<"<Channel tag=\""<<theChannels[i].getTag()<<"\"";
		ofile<<" label=\""<<theChannels[i].getLabel()<<"\"";
		ofile<<" />"<<endl;
	}
	ofile<<"</Channels>"<<endl;

	//---------------------
	// -- Write ProcessClass
	//---------------------
	ofile<<"<ProcessClasses>"<<endl;
	for(unsigned int i=0;i<theProcessClasses.size();i++){
		ofile<<"<ProcessClass tag=\""<<theProcessClasses[i].getTag()<<"\"";
		ofile<<" label=\""<<theProcessClasses[i].getLabel()<<"\"";
		ofile<<" type=\""<<theProcessClasses[i].getType()<<"\"";
		ofile<<" color=\""<<theProcessClasses[i].getColor()<<"\"";
		ofile<<" />"<<endl;
	}
	ofile<<"</ProcessClasses>"<<endl;
	
	//---------------------
	// -- Write Datasets
	//---------------------
	ofile<<"<Datasets>"<<endl;
	for(unsigned int i=0;i<theDatasets.size();i++){
		ofile<<"<Dataset tag=\""<<theDatasets[i].getTag()<<"\"";
		ofile<<" label=\""<<theDatasets[i].getLabel()<<"\"";
      		ofile<<" processClass=\""<<theDatasets[i].getProcessClass()<<"\"";
		ofile<<" XsecOrLumi=\""<<theDatasets[i].getXsecOrLumi()<<"\"";
		ofile<<" getTrueNumberOfEvents=\""<<theDatasets[i].getTrueNumberOfEvents()<<"\"";
		ofile<<" />"<<endl;
	}
	ofile<<"</Datasets>"<<endl;


	ofile.close();
}



void SonicScrewdriver::LoadXMLConfig(string inputfilename){
   // First create engine
   TXMLEngine* xml = new TXMLEngine;
   
   // Now try to parse xml file
   // Only file with restricted xml syntax are supported
   XMLDocPointer_t xmldoc = xml->ParseFile(inputfilename.c_str());
   if (xmldoc==0) {
      delete xml;
      return;  
   }   

   // take access to main node   
   XMLNodePointer_t node = xml->DocGetRootElement(xmldoc);
   
   // display recursively all nodes and subnodes
   //DisplayNode(xml, mainnode, 1); 
   //mainode = xml->GetNodeName(mainnode)

   // loop over the nodes
   for(; node; node = xml->GetNext(node)){
	cout<<xml->GetNodeName(node)<<endl;	
   	string nodename = xml->GetNodeName(node);
	if(nodename == string("Variables")){
		XMLNodePointer_t child = xml->GetChild(node);
		while (child!=0) {
		       if( string(xml->GetNodeName(child)) == string("Variable")){
				float* dummy = 0;
				string tag;
		       		string label;
		       		string unit;
				float min = 0;
				float max = 0;
				int nBins = 0;
				float* binning = 0;
				if(xml->HasAttr(child,"tag")) tag = xml->GetAttr(child,"tag");	
		       		if(xml->HasAttr(child,"label")) label = xml->GetAttr(child,"label");
		       		if(xml->HasAttr(child,"unit")) unit = xml->GetAttr(child,"unit");
				if(xml->HasAttr(child,"min")) min = atof(xml->GetAttr(child,"min"));
				if(xml->HasAttr(child,"max")) max = atof(xml->GetAttr(child,"max"));
				if(xml->HasAttr(child,"nbins")){
					nBins = atoi(xml->GetAttr(child,"nbins"));
					binning = new float[nBins];
					if(xml->HasAttr(child,"binning")){
						string s = xml->GetAttr(child,"binning");
						string delimiter = "-";

						size_t pos = 0;
						std::string token;
						int iter = 0;
						while ((pos = s.find(delimiter)) != std::string::npos) {
    							token = s.substr(0, pos);
    							//std::cout << token << std::endl;
   							if(iter<nBins){
							   binning[iter] = atof(token.c_str());
							   iter++;
							}
							s.erase(0, pos + delimiter.length());
						}
   						if(iter<nBins){
							   binning[iter] = atof(s.c_str());
							   iter++;
							
						}
					}
      					AddVariable(tag, label, unit, nBins, binning, dummy);
				}
				else{
      					 AddVariable(tag, label, unit, nBins, min, max, dummy);
				}
      		
		       }
		       child = xml->GetNext(child);
		}   
	}
	if(nodename == string("ProcessClasses")){
		XMLNodePointer_t child = xml->GetChild(node);
		while (child!=0) {
		       if( string(xml->GetNodeName(child)) == string("ProcessClass")){
				string tag;
		       		string label;
				string type;
				int color = 0;
		       		if(xml->HasAttr(child,"tag")) tag = xml->GetAttr(child,"tag");	
		       		if(xml->HasAttr(child,"label")) label = xml->GetAttr(child,"label");
		       		if(xml->HasAttr(child,"type")) type = xml->GetAttr(child,"type");
		       		if(xml->HasAttr(child,"color")) color = atoi(xml->GetAttr(child,"color"));
				//cout<<"tag = "<<tag<<" label = "<<label<<endl;
      			 	AddProcessClass(tag, label, type, color);
		       }
		       child = xml->GetNext(child);
		}   
	}
	if(nodename == string("Regions")){
		XMLNodePointer_t child = xml->GetChild(node);
		while (child!=0) {
		       if( string(xml->GetNodeName(child)) == string("Region")){
				string tag;
		       		string label;
		       		if(xml->HasAttr(child,"tag")) tag = xml->GetAttr(child,"tag");	
		       		if(xml->HasAttr(child,"label")) label = xml->GetAttr(child,"label");
				//cout<<"tag = "<<tag<<" label = "<<label<<endl;
		       		AddRegion(tag,label,0);
		       }
		       child = xml->GetNext(child);
		}   
	}
	if(nodename == string("Channels")){
		XMLNodePointer_t child = xml->GetChild(node);
		while (child!=0) {
		       if( string(xml->GetNodeName(child)) == string("Channel")){
				string tag;
		       		string label;
		       		if(xml->HasAttr(child,"tag")) tag = xml->GetAttr(child,"tag");	
		       		if(xml->HasAttr(child,"label")) label = xml->GetAttr(child,"label");
				//cout<<"tag = "<<tag<<" label = "<<label<<endl;
		       		AddChannel(tag,label,0);
		       }
		       child = xml->GetNext(child);
		}   
	}
	if(nodename == string("Datasets")){
		XMLNodePointer_t child = xml->GetChild(node);
		while (child!=0) {
		       if( string(xml->GetNodeName(child)) == string("Dataset")){
				string tag;
		       		string label;
		       		string processClass;
				int trueNofEvents = -1;
				int XsecOrLumi = -1;
				if(xml->HasAttr(child,"tag")) tag = xml->GetAttr(child,"tag");	
		       		if(xml->HasAttr(child,"label")) label = xml->GetAttr(child,"label");
		       		if(xml->HasAttr(child,"processClass")) label = xml->GetAttr(child,"processClass");
		       		if(xml->HasAttr(child,"getTrueNumberOfEvents")) trueNofEvents = atoi(xml->GetAttr(child,"getTrueNumberOfEvents"));
		       		if(xml->HasAttr(child,"XsecOrLumi")) XsecOrLumi = atoi(xml->GetAttr(child,"XsecOrLumi"));
		       		AddDataset(tag, processClass, trueNofEvents, XsecOrLumi);
		       }
		       child = xml->GetNext(child);
		}   
	}
   }

   // Release memory before exit
   xml->FreeDoc(xmldoc);
   delete xml;
}
