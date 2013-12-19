

#include <stdlib.h>
#include <iostream>

#include "TTree.h"
#include "TFile.h"
#include "TRandom.h"

using namespace std;

typedef struct
{

    Float_t invariantMass;
    Float_t MET;

    Float_t leptonPt;
    Float_t leptonFlavor;

}
microEvent;

#define ROOT_STRUCTURE_DESCRIPTION "invariantMass:MET:leptonPt:leptonFlavor"

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

    system("mkdir -p ./trees/");

    TFile treeFooFile( "trees/foo.root", "RECREATE","Tree"); TTree *foo  = new TTree("theTree","This is a tree for SonicScrewdriver testing.");
    TFile treeBarFile( "trees/bar.root", "RECREATE","Tree"); TTree *bar  = new TTree("theTree","This is a tree for SonicScrewdriver testing.");
    TFile treeMufFile( "trees/muf.root", "RECREATE","Tree"); TTree *muf  = new TTree("theTree","This is a tree for SonicScrewdriver testing.");
    TFile treeDataFile("trees/data.root","RECREATE","Tree"); TTree *data = new TTree("theTree","This is a tree for SonicScrewdriver testing.");

    foo->Branch( "theBranch",&myEvent,ROOT_STRUCTURE_DESCRIPTION);
    bar->Branch( "theBranch",&myEvent,ROOT_STRUCTURE_DESCRIPTION);
    muf->Branch( "theBranch",&myEvent,ROOT_STRUCTURE_DESCRIPTION);
    data->Branch("theBranch",&myEvent,ROOT_STRUCTURE_DESCRIPTION);

    TRandom* gen = new TRandom();

    int bosonType = -1;

    for (int i = 0 ; i < 50000 ; i++)
    {

        // ProcessTypeMC : - 0 = foo
        //                 - 1 = bar
        //                 - 2 = muf (signal)
        //                 - 3 = data (data)

        int processTypeMC         = (int) (gen->Uniform()*4);

        // For data, generate 1% muf,
        //                    49.5% foo,
        //                    49.5% bar
        int processTypeData = -1;
        if (processTypeMC == 3) processTypeData = (int) (gen->Uniform()*2*(1+0.01)); 

        // "Generate" event if processTypeMC or processTypeData = i
        if ((processTypeMC == 0) || (processTypeData == 0))
        {
            myEvent.invariantMass = gen->Gaus(91,16);
            myEvent.MET           = gen->Exp(30);
            myEvent.leptonPt      = gen->Gaus(myEvent.invariantMass / 3.0, 20.0);
        }
        else if ((processTypeMC == 1) || (processTypeData == 1))
        {
            myEvent.invariantMass = gen->Uniform()*200;
            myEvent.MET           = gen->Exp(120);
            myEvent.leptonPt      = gen->Gaus(myEvent.MET / 10.0, 20.0);
        }
        else if ((processTypeMC == 2) || (processTypeData == 2))
        {
            myEvent.invariantMass = gen->Gaus(125,4);
            myEvent.MET           = gen->Exp(150);
            myEvent.leptonPt      = gen->Gaus((myEvent.invariantMass + myEvent.MET) / 5.0, 15.0);
        }

        myEvent.leptonFlavor = (int) (gen->Uniform()*2);

             if (processTypeMC == 0) foo ->Fill();
        else if (processTypeMC == 1) bar ->Fill();
        else if (processTypeMC == 2) muf ->Fill();
        else if (processTypeMC == 3) data->Fill();

    }

    treeFooFile.Write();  treeFooFile.Write();
    treeBarFile.Write();  treeBarFile.Write();
    treeMufFile.Write();  treeMufFile.Write();
    treeDataFile.Write(); treeDataFile.Write();

    return 0;
}
