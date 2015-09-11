// ############################################################
// # Usage                                                    #
// #                                                          #
// # - Include this header in your code                       #
// # - Create an instance of babyEvent, for instance event :  #
// #      babyEvent event;                                    #
// # - Open your tree, for example :                          #
// #      TTree* theTree = f.Get("babyTuple");                #
// # - Init branches by calling :                             #
// #      InitializeBranchesForReading(theTree,&event);       #
// # - To read the i-ish event, call :                        #
// #      ReadEvent(theTree,i,&event);                        #
// # - Get the value of your branch by acessing               #
// #      event.branchName;                                   #
// ############################################################

#ifndef babyFormat
#define babyFormat

// ##########################
// #  Baby event structure  #
// ##########################

typedef struct
{
    float leptonsPhi;
    float jetsPhi;
    int   triggerIsE;
    int   triggerIsMu;
    float metPuppiPhi;
    float triggerPhi;
    float jetsPUid;
    int   electronMuonTrigger;
    int   numberOfSelectedJets;
    int   numberOfTriggerObjects;
    float ETmissPhi;
    float HT;
    int   doubleElectronTrigger;
    int   leptonsId;
    float leptonsPt;
    int   numberOfBTaggedJets;
    float triggerEta;
    float ETmiss;
    float triggerPt;
    float crossSection;
    int   eventId;
    float jetsCSVv2;
    int   singleMuonTrigger;
    float MT;
    float metPuppi;
    float leptonsEta;
    int   singleElectronTrigger;
    float leptonsIso;
    int   doubleMuonTrigger;
    int   numberOfSelectedLeptons;
    int   lumiId;
    int   trigger;
    int   runId;
    int   totalNumberOfInitialEvent;
    float ETmissSumet;
    float metPuppiSumet;
    float jetsEta;
    float mcweight;
    int   muonElectronTrigger;
    float jetsPt;
    int   doubleMuonTrTrigger;

    // Intermediate pointers for special types
    // Yes, this shit is needed because ROOT is crap.

}
babyEvent;

// #############################
// #  Branches initialization  #
// #############################

void InitializeBranchesForReading(TTree* theTree, babyEvent* myEvent)
{

    theTree->SetBranchAddress("leptonsPhi",              &(myEvent->leptonsPhi));
    theTree->SetBranchAddress("jetsPhi",                 &(myEvent->jetsPhi));
    theTree->SetBranchAddress("triggerIsE",              &(myEvent->triggerIsE));
    theTree->SetBranchAddress("triggerIsMu",             &(myEvent->triggerIsMu));
    theTree->SetBranchAddress("metPuppiPhi",             &(myEvent->metPuppiPhi));
    theTree->SetBranchAddress("triggerPhi",              &(myEvent->triggerPhi));
    theTree->SetBranchAddress("jetsPUid",                &(myEvent->jetsPUid));
    theTree->SetBranchAddress("electronMuonTrigger",     &(myEvent->electronMuonTrigger));
    theTree->SetBranchAddress("numberOfSelectedJets",    &(myEvent->numberOfSelectedJets));
    theTree->SetBranchAddress("numberOfTriggerObjects",  &(myEvent->numberOfTriggerObjects));
    theTree->SetBranchAddress("ETmissPhi",               &(myEvent->ETmissPhi));
    theTree->SetBranchAddress("HT",                      &(myEvent->HT));
    theTree->SetBranchAddress("doubleElectronTrigger",   &(myEvent->doubleElectronTrigger));
    theTree->SetBranchAddress("leptonsId",               &(myEvent->leptonsId));
    theTree->SetBranchAddress("leptonsPt",               &(myEvent->leptonsPt));
    theTree->SetBranchAddress("numberOfBTaggedJets",     &(myEvent->numberOfBTaggedJets));
    theTree->SetBranchAddress("triggerEta",              &(myEvent->triggerEta));
    theTree->SetBranchAddress("ETmiss",                  &(myEvent->ETmiss));
    theTree->SetBranchAddress("triggerPt",               &(myEvent->triggerPt));
    theTree->SetBranchAddress("crossSection",            &(myEvent->crossSection));
    theTree->SetBranchAddress("eventId",                 &(myEvent->eventId));
    theTree->SetBranchAddress("jetsCSVv2",               &(myEvent->jetsCSVv2));
    theTree->SetBranchAddress("singleMuonTrigger",       &(myEvent->singleMuonTrigger));
    theTree->SetBranchAddress("MT",                      &(myEvent->MT));
    theTree->SetBranchAddress("metPuppi",                &(myEvent->metPuppi));
    theTree->SetBranchAddress("leptonsEta",              &(myEvent->leptonsEta));
    theTree->SetBranchAddress("singleElectronTrigger",   &(myEvent->singleElectronTrigger));
    theTree->SetBranchAddress("leptonsIso",              &(myEvent->leptonsIso));
    theTree->SetBranchAddress("doubleMuonTrigger",       &(myEvent->doubleMuonTrigger));
    theTree->SetBranchAddress("numberOfSelectedLeptons", &(myEvent->numberOfSelectedLeptons));
    theTree->SetBranchAddress("lumiId",                  &(myEvent->lumiId));
    theTree->SetBranchAddress("trigger",                 &(myEvent->trigger));
    theTree->SetBranchAddress("runId",                   &(myEvent->runId));
    theTree->SetBranchAddress("totalNumberOfInitialEvent", &(myEvent->totalNumberOfInitialEvent));
    theTree->SetBranchAddress("ETmissSumet",             &(myEvent->ETmissSumet));
    theTree->SetBranchAddress("metPuppiSumet",           &(myEvent->metPuppiSumet));
    theTree->SetBranchAddress("jetsEta",                 &(myEvent->jetsEta));
    theTree->SetBranchAddress("mcweight",                &(myEvent->mcweight));
    theTree->SetBranchAddress("muonElectronTrigger",     &(myEvent->muonElectronTrigger));
    theTree->SetBranchAddress("jetsPt",                  &(myEvent->jetsPt));
    theTree->SetBranchAddress("doubleMuonTrTrigger",     &(myEvent->doubleMuonTrTrigger));
}

// ################################
// #  Function to read one event  #
// ################################

void ReadEvent(TTree* theTree, long int i, babyEvent* myEvent)
{
    theTree->GetEntry(i);

    // Put actual content of special type branches where they should be...
}

#endif
