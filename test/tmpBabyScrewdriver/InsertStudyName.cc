
// C++ and ROOT headers
#include <vector>
using namespace std;
#include "TTree.h"

// BabyTuple format and on-the-fly variables
#include "BabyTupleFormat.h"
babyEvent myEvent;
#include "OnTheFlyVariables.h"

// BabyScrewdriver
#include "BabyScrewdriver.h"

bool muonChannelSelector() { return true; }

// ################################################################

void BabyScrewdriver::Init()
{
    PrintBoxedMessage("Initializing babyScrewdriver");

    babyTuplePath = "/path/to/your/babyTuple/store/";
    
    totalNumberOfWorkers = 1;

    AddVariable("MT", "whatever",  "", 10,   0, 100,  &(myEvent.MT));
    AddVariable("x", "whatever2", "", 20, -40,  40,  &(onTheFlyVariables.someCoolVariable));
    // ...

    AddProcessClass("foo", "Foo", "background", kRed);
        AddDataset("foo", "foo", 0, 0);
    // ...

    AddRegion("preselection", "Preselection", { Cut("MT", '>', 50) });
    // ...

    AddChannel("muon", "#mu channel", &muonChannelSelector);
    // ...

    SetLumi(3.14);

    Create1DHistos();
}

// ################################################################

void BabyScrewdriver::ActionForEachEvent(string currentDataset)
{
    // Compute on the fly variables if needed

    ComputeOnTheFlyVariables();

    // Determine which processClass to fill
    // (in the most trivial case, only call GetProcessClass(currentDataset),
    // but you might want to split a dataset according to
    // the number of generated leptons, for instance)
    string currentProcessClass = GetProcessClass(currentDataset);
    string currentProcessType  = GetProcessClassType(currentProcessClass);

    // Compute weight for current event

    float weightLumi = myEvent.crossSection * GetLumi() / myEvent.totalNumberOfInitialEvent;

    float weight     = weightLumi;
    if (currentProcessType == "data") weight = 1.0;

    // Fill this event in the histo collections

    //AutoFillProcessClass(currentProcessClass, weight);
    AutoFillProcessClass(currentProcessClass, 0.001);
}

// ################################################################

void BabyScrewdriver::PostProcessingStep()
{
    // ######################
    //  Plot configuration and production
    // ######################

    // Schedule plots

    SchedulePlots("1DSuperimposed");
    SchedulePlots("1DStack");

    // Config plots

    SetGlobalStringOption("Plot", "infoTopRight", "CMS Simulation");
    SetGlobalStringOption("Plot", "infoTopLeft",  "#sqrt{s} = 8 TeV");

    SetGlobalBoolOption("Plot", "exportPdf", true);
    SetGlobalBoolOption("Plot", "exportEps", false);
    SetGlobalBoolOption("Plot", "exportPng", false);

    // Make and write the plots

    cout << endl;
    cout << "   > Making plots..." << endl;
    MakePlots();
    cout << "   > Saving plots..." << endl;
    WritePlots("./plots/");

    // ######################
    //  Tables and other stuff
    // ######################

    TableDataMC(this,{"preselection"},"muon", "").Print("table.tab",4);

}



