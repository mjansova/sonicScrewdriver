#ifndef Histo1DSumData_h
#define Histo1DSumData_h

#include "interface/histos/Histo1D.h"
#include "interface/Figure.h"

namespace theDoctor
{

    class Histo1DSumData : public Histo1D
    {

     public:

      Histo1DSumData(Variable* theVar_,
                     Region* theRegion_,
                     Channel* theChannel_,
                     vector<Histo1DEntries*> theDatas) :
      Histo1D(Name("1DSumData","Entries"),theVar_,theRegion_,theChannel_)
      {
          string nameHisto =  string("v:")+theVar->getTag()
                                   +"|r:" +theRegion->getTag()
                                   +"|c:" +theChannel->getTag()
                                   +"|t:" +theHistoType.getTag();

          theHisto->SetName(nameHisto.c_str());

          // Loop on the input histograms and add each of them to the histo
          for (unsigned int i = 0 ; i < theDatas.size() ; i++)
          {
              theHisto->Add(theDatas[i]->getClone());
          }
      };

      ~Histo1DSumData() { };

      static void GetHistoDependencies(vector<pair<string,string> >& output, string options = "")
      {
      }

      // Accessors
      Figure GetYieldAndError()
      {
          double error;
          double yield = theHisto->IntegralAndError(0,theHisto->GetNbinsX()+1,error);

          return Figure(yield,error);
      }

      static void Produce(vector<Variable>* theVariables,
                          vector<ProcessClass>* theProcessClasses,
                          vector<Region>* theRegions,
                          vector<Channel>* theChannels,
                          HistoScrewdriver* theHistoScrewdriver,
                          OptionsScrewdriver theGlobalOptions,
                          string histoOptions)
      {
          // Browse the (var x reg x chan)
          for (unsigned int v = 0 ; v < theVariables->size() ; v++)
          for (unsigned int r = 0 ; r < theRegions->size()   ; r++)
          for (unsigned int c = 0 ; c < theChannels->size()  ; c++)
          {

              vector<Histo1DEntries*> theDatas;

              Variable* theVar     = &((*theVariables)[v]);
              Region*   theRegion  = &((*theRegions)[r]);
              Channel*  theChannel = &((*theChannels)[c]);

              // Now loop on the histos
              for (unsigned int i = 0 ; i < theProcessClasses->size() ; i++)
              {
                  // If this processClass is not data, we skip it
                  if ((*theProcessClasses)[i].getType() != "data") continue;
                  // If it it, we add it to the relevant backgrounds
                  //
                  Histo1DEntries* thisData = theHistoScrewdriver->get1DHistoEntriesPointer(theVar->getTag(),
                                                                                           (*theProcessClasses)[i].getTag(),
                                                                                           theRegion->getTag(),
                                                                                           theChannel->getTag());
                  theDatas.push_back(thisData);
              }

              theHistoScrewdriver->Add1DHistoForPlots(
                                                        Histo1DSumData(theVar,theRegion,theChannel,theDatas)
                                                     );
          }
      }

     private:

    };

}

#endif
