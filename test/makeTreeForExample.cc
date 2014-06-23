#include "common.h"

typedef struct
{
    float invariantMass;
    float MET;
    float leptonPt;
    float leptonFlavor;
    int   mMuf;
}
microEvent;

#define ROOT_STRUCTURE_DESCRIPTION "invariantMass:MET:leptonPt:leptonFlavor:mMuf/I"

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
#     -> medium MET                     #
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

TTree dummy;

void fillEventWithProcessType(microEvent* myEvent, TRandom* gen, short int processType, bool isData = false);

int main()
{
    microEvent myEvent;
    TRandom* gen = new TRandom();

    // Create the trees
    system("mkdir -p ./trees/");

    TFile treeFooFile( "trees/foo.root", "RECREATE","Tree"); TTree *foo  = new TTree("theTree","This is a tree for SonicScrewdriver testing.");
    TFile treeBarFile( "trees/bar.root", "RECREATE","Tree"); TTree *bar  = new TTree("theTree","This is a tree for SonicScrewdriver testing.");
    TFile treeMufFile( "trees/muf.root", "RECREATE","Tree"); TTree *muf  = new TTree("theTree","This is a tree for SonicScrewdriver testing.");
    TFile treeDataFile("trees/data.root","RECREATE","Tree"); TTree *data = new TTree("theTree","This is a tree for SonicScrewdriver testing.");

    foo->Branch( "theBranch",&myEvent,ROOT_STRUCTURE_DESCRIPTION);
    bar->Branch( "theBranch",&myEvent,ROOT_STRUCTURE_DESCRIPTION);
    muf->Branch( "theBranch",&myEvent,ROOT_STRUCTURE_DESCRIPTION);
    data->Branch("theBranch",&myEvent,ROOT_STRUCTURE_DESCRIPTION);

    // Fill the trees 
    for (int i = 0 ; i < 50000 ; i++)
    {

        fillEventWithProcessType(&myEvent, gen, 0);
        foo->Fill();
        
        fillEventWithProcessType(&myEvent, gen, 1);
        bar->Fill();

        fillEventWithProcessType(&myEvent, gen, 2);
        muf->Fill();

        // For data, generate 1% muf,
        //                    49.5% foo,
        //                    49.5% bar
        int processTypeData = (int) (gen->Uniform()*2*(1+0.01)); 
        fillEventWithProcessType(&myEvent, gen, processTypeData,true);
        myEvent.mMuf = -1;
        data->Fill();
    }

    // Write the trees
    treeFooFile.Write();  
    treeBarFile.Write();  
    treeMufFile.Write();  
    treeDataFile.Write(); 

    return 0;
}

void fillEventWithProcessType(microEvent* myEvent, TRandom* gen, short int processType, bool isData)
{
    if (processType == 0)
    {
        myEvent->mMuf          = -1;
        myEvent->invariantMass = gen->Gaus(91,16);
        myEvent->MET           = gen->Exp(30);
        myEvent->leptonPt      = gen->Gaus(myEvent->invariantMass / 3.0, 20.0);
    }
    else if (processType == 1)
    {
        myEvent->mMuf          = -1;
        myEvent->invariantMass = gen->Uniform()*200;
        myEvent->MET           = gen->Exp(90);
        myEvent->leptonPt      = gen->Gaus(myEvent->MET / 10.0, 20.0);
    }
    else if (processType == 2)
    {
        if (isData) myEvent->mMuf = 125;
        else        myEvent->mMuf = ((int) (gen->Uniform()*11)*2) + 115;

        myEvent->invariantMass = gen->Gaus(myEvent->mMuf,(myEvent->mMuf-110)/5);
        myEvent->MET           = gen->Exp(150);
        myEvent->leptonPt      = gen->Gaus((myEvent->mMuf + myEvent->MET) / 5.0, 15.0);

        if (isData) myEvent->mMuf = -1;
    }

    myEvent->leptonFlavor = (int) (gen->Uniform()*2);
}
