

#include <iostream>

#include "TTree.h"
#include "TFile.h"
#include "TRandom.h"

using namespace std;

typedef struct
{

	Float_t invariantMass;
	Float_t pT;

	Float_t channelType;

}
DataStruct;

int main()
{
 
	DataStruct data;

  	TFile treeFile("tree.root","RECREATE","Tree");
  	TTree *tree = new TTree("theTree","This is a tree for SonicScrewdriver testing.");
  
  	tree->Branch("theBranch",&data,"invariantMass:pT:channelType");

	TRandom* gen = new TRandom();

	int bosonType = -1;

	for (int i = 0 ; i < 100000 ; i++)
	{

		data.channelType = (int) (gen->Uniform()*3);
		
		if (data.channelType == 2)
		{
			if (gen->Uniform() < 0.5) bosonType = 0;
			else					  bosonType = 1;						
		}
		else bosonType = data.channelType;

		if (bosonType == 0)
		{
			data.invariantMass = gen->Gaus(80.3,2);
			data.pT = gen->Exp(20);
		}
		else
		{
			data.invariantMass = gen->Gaus(91.2,2);
			data.pT = gen->Exp(15);
		}

		tree->Fill();
	}

    tree->Print();
    treeFile.Write();
    treeFile.Close();

	return 0;
}
