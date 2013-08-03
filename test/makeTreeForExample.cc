

#include <iostream>

#include "TTree.h"
#include "TFile.h"
#include "TRandom.h"

using namespace std;

typedef struct
{

	Float_t invariantMass;
	Float_t MET;

	Float_t processType;
	Float_t leptonsFlavor;

}
microEvent;

#define ROOT_STRUCTURE_DESCRIPTION "invariantMass:MET:processType:leptonsFlavor"

/*
#########################################
#                                       #
#   This code generate a dummy TTree    #
#   only meant to DEMONSTRATE how to    #
#   use the SonicScrewdriver            #
#                                       #
#   Processes :                         #
#                                       #
#   a) foo                              #
#     -> invariant mass peak at ~91GeV  #
#     -> low MET                        #
#                                       #
#   b) bar                              #
#     -> invariant mass ~flat           #
#     -> higher MET                     #
#                                       #
#   c) muf                              #
#     -> invariant mass peak at ~125GeV #
#     -> higher MET                     #
#                                       #
#   d) (pseudo-)dat                     #
#     -> Mean of a,b,0.05*c fluctuated  #
#                                       #
#########################################
*/

int main()
{
 
	microEvent myEvent;

  	TFile treeFile("tree.root","RECREATE","Tree");
  	TTree *tree = new TTree("theTree","This is a tree for SonicScrewdriver testing.");
  
  	tree->Branch("theBranch",&myEvent,ROOT_STRUCTURE_DESCRIPTION);

	TRandom* gen = new TRandom();

	int bosonType = -1;

	for (int i = 0 ; i < 100000 ; i++)
	{

		myEvent.processType   = (int) (gen->Uniform()*4);
		myEvent.leptonsFlavor = (int) (gen->Uniform()*2);

		int processTypeData = -1;
		if (myEvent.processType == 3) processTypeData = (int) (gen->Uniform()*2*(1+0.05)); 


		     if ((myEvent.processType == 0) || (processTypeData == 0))
		{
			myEvent.invariantMass = gen->Gaus(91,8);
			myEvent.MET           = gen->Exp(30);
		}
		else if ((myEvent.processType == 1) || (processTypeData == 1))
		{
			myEvent.invariantMass = gen->Uniform()*200;
			myEvent.MET           = gen->Exp(120);
		}
		else if ((myEvent.processType == 2) || (processTypeData == 2))
		{
			myEvent.invariantMass = gen->Gaus(125,8);
			myEvent.MET           = gen->Exp(150);
		}

		tree->Fill();
	}

    tree->Print();
    treeFile.Write();
    treeFile.Close();

	return 0;
}
