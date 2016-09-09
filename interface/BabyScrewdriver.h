#ifndef BABYSCREWDRIVER_H
#define BABYSCREWDRIVER_H

#include "string.h"
#include <sys/wait.h>

#include "interface/SonicScrewdriver.h"
#include "interface/tables/TableDataMC.h"
#include "interface/tables/TableBackgroundSignal.h"
#include "interface/tables/TableToBackgroundRatio.h"
#include "interface/tables/TableZbi.h"

using namespace theDoctor;

class BabyScrewdriver : public SonicScrewdriver
{

    public :

        BabyScrewdriver()
        {
            Init();
        };

        ~BabyScrewdriver() { };

        void Init();

        void ProcessDatasets(int workerId);

        void ActionForEachEvent(string currentDataset);

        void PostProcessingStep();

        int  CreateWorkers();

        void PrintBoxedMessage(string message);

        void WaitForWorkers();

        void ExportWorkerOutput(int workerId, int masterPID);

        void MergeWorkerOutputs(int masterPID);

    private :

        string babyTuplePath;
        int totalNumberOfWorkers;
        int workerID;

};

///*
// This should be moved elsewhere
string intToString(int input)
{
    std::ostringstream ss;
    ss << input;
    return ss.str();
}
//*/

int main()
{
    BabyScrewdriver b;

    // Get master PID
    int masterPID = getpid();

    // Instantiate workers
    // After this line, this process is forked n times,
    // this includes a duplication of all the personal memory of the process
    // Each worker/fork starts at this line, with a different return value (0 for master, >0 for workers)
    int workerId = b.CreateWorkers();

    // #########
    //  Workers
    // #########

    if (workerId != 0)
    {
    	// for gprofile
    	//gmon_thread_timer(1);

        // Process the datasets
        b.ProcessDatasets(workerId);

        cout << "[Worker " << workerId << "] "
             << "Done processing. Exporting histogram collection to be grabbed by master."
             << endl;

        // Export histogram collection
        b.ExportWorkerOutput(workerId,masterPID);
    	
	//for gprofile
	//gmon_thread_timer(0);

        cout << "[Worker " << workerId << "] "
             << "Exiting." << endl;

    }

    // ########
    //  Master
    // ########

    else
    {
        cout << "[Master] Waiting for workers." << endl;

        b.WaitForWorkers();

        cout << "[Master] All workers done, merging outputs." << endl;

        b.MergeWorkerOutputs(masterPID);

        cout << "[Master] Now doing post processing steps." << endl;

        b.PostProcessingStep();
    }
}

void BabyScrewdriver::ExportWorkerOutput(int workerId, int masterPID)
{
    system("mkdir -p /opt/sbg/cms/ui6_data1/mjansova/ssMess/");
    ExportHistosEntries("/opt/sbg/cms/ui6_data1/mjansova/ssMess/.babyScrewdriver."+intToString(masterPID)+"-"+intToString(workerId)+".output.root");
}

void BabyScrewdriver::MergeWorkerOutputs(int masterPID)
{
    for (int w = 1 ; w <= totalNumberOfWorkers ; w++)
        ImportHistosEntries("/opt/sbg/cms/ui6_data1/mjansova/ssMess/.babyScrewdriver."+intToString(masterPID)+"-"+intToString(w)+".output.root");
}

void BabyScrewdriver::ProcessDatasets(int workerId)
{
    vector<string> datasetsList;
    GetDatasetList(&datasetsList);

    for (unsigned int d = 0 ; d < datasetsList.size() ; d++)
    {
        // Get dataset name
        string currentDataset      = datasetsList[d];

        cout << "[Worker " << workerId << "] "
             << "Starting to process " << currentDataset
             << ", part " << workerId << "/" << totalNumberOfWorkers
             << endl;

        // Open the tree
        TFile f((babyTuplePath+"/"+currentDataset+".root").c_str());
        TTree* theTree = (TTree*) f.Get("babyTuple");
        cout<<"here: call Initializetion"<<endl;
	InitializeBranchesForReading(theTree,&myEvent);
	
	// retrievve total nof weighted events
	//@EC@ temporarilly changed for trigger study
	TH1F* hWeight = (TH1F*) f.Get("htotweight");
	float TotalWeight = hWeight->GetBinContent(1);
	//float TotalWeight = 1;

        // Get number of entries, determine what range to run on
        unsigned int nEntries           = theTree->GetEntries();


                                                                    // For tests only
                                                                    // nEntries = 10000000;


        unsigned int entriesToProcess   = nEntries / totalNumberOfWorkers;
        if (workerId == totalNumberOfWorkers)
                      entriesToProcess += nEntries % totalNumberOfWorkers;

        unsigned int entryIdMin = (workerId - 1) * (nEntries / totalNumberOfWorkers);
        unsigned int entryIdMax = entryIdMin + entriesToProcess;

        // Loop on the events
        for (unsigned int i = entryIdMin ; i < entryIdMax ; i++)
        {
            // Get the i-th entry
	    ReadEvent(theTree,i,&myEvent);

            //Writing the total number of events (weighted)
	    myEvent.totalNumberOfInitialEvent = TotalWeight;
	    ActionForEachEvent(currentDataset);
        }

        // Close the tree
        f.Close();
    }
}

int BabyScrewdriver::CreateWorkers()
{
    //gmon_start_all_thread_timers()

    int i = 0;
    int pid = -1;
    for (i = 1 ; i <= totalNumberOfWorkers ; i++)
    {
        // Black magic right here

        pid = fork();

        // After this fork(), the mother thread has pid = 0, and the child has pid > 0.
        // We don't want the child to fork themselves, so we break the loop for
        // them, while mother keeps forking.

        if (pid == 0) break;

        // pid < 0 means fork didn't work, so let's kill everyone.

        if (pid < 0) { cout << "Worker creation failed. Exiting." << endl; exit(-1); }
    }

    if (pid == 0) return i;
    else          return 0;

}

void BabyScrewdriver::WaitForWorkers()
{
    while (true)
    {
        int status;
        pid_t done = wait(&status);
        if (done == -1)
        {
            if (errno == ECHILD) break; // no more child processes
        }
        else
        {
            if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
            {
                cout << "status " << status << endl;
                cout << "WIFEXITED " <<   WIFEXITED(status) << endl;
                cout << "WEXITSTATUS " << WEXITSTATUS(status) << endl;
                cerr << "Worker with PID " << done << " failed. Exiting." << endl;
                exit(-1);
            }
        }
    }
}

// TODO / FIXME This should be moved elsewhere
void BabyScrewdriver::PrintBoxedMessage(string message)
{
    cout << endl;

    cout << "   ┌──";
    for(unsigned int i = 0 ; i <= message.size() ; i++) cout << "─";
    cout << "─┐  " << endl;

    cout << "   │  " << message << "  │  " << endl;

    cout << "   └──";
    for(unsigned int i = 0 ; i <= message.size() ; i++) cout << "─";
    cout << "─┘  " << endl;

    cout << endl;

}

#endif
