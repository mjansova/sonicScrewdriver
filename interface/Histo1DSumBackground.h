#ifndef Histo1DSumBackground_h
#define Histo1DSumBackground_h

#include "interface/Histo1D.h"
#include "interface/Figure.h"

namespace theDoctor
{

    class Histo1DSumBackground : public Histo1D 
    {
      
     public:

      Histo1DSumBackground(Variable* theVar_, 
                           Region* theRegion_, 
                           Channel* theChannel_,
                           vector<Histo1DEntries*> theRelevantHistograms) : 
      Histo1D(Name("sumBackground","Entries"),theVar_,theRegion_,theChannel_)
      {
          DEBUG_MSG << endl;

          string nameHisto =  string("v:")+theVar->getTag()
                                   +"|r:" +theRegion->getTag()
                                   +"|c:" +theChannel->getTag()
                                   +"|t:" +theHistoType.getTag();

          theHisto->SetName(nameHisto.c_str());

          // Loop on the input histograms and add each of them to the histo
          for (unsigned int i = 0 ; i < theRelevantHistograms.size() ; i++)
          {
              theHisto->Add(theRelevantHistograms[i]->getClone());
          }

          DEBUG_MSG << endl;
      }; 

      ~Histo1DSumBackground() { };
  
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

              vector<Histo1DEntries*> theRelevantHistograms;

              Variable* theVar     = &((*theVariables)[v]);
              Region*   theRegion  = &((*theRegions)[r]);
              Channel*  theChannel = &((*theChannels)[c]);
      
              DEBUG_MSG << "v = " << theVar->getTag() << " ; r = " << theRegion->getTag() << " ; c = " << theChannel->getTag() << endl;

              // Now loop on the histos
              for (unsigned int i = 0 ; i < theProcessClasses->size() ; i++)
              {
                  // If this processClass is not a background, we skip it
                  if ((*theProcessClasses)[i].getType() != "background") continue;
                  // If it it, we add it to the relevant backgrounds
                  //
                  Histo1DEntries* thisBackground = theHistoScrewdriver->get1DHistoPointer(theVar->getTag(),
                                                                                          (*theProcessClasses)[i].getTag(),
                                                                                          theRegion->getTag(),
                                                                                          theChannel->getTag());
                  theRelevantHistograms.push_back(thisBackground);
              }

              theHistoScrewdriver->Add1DHistoForPlots(
                                                        Histo1DSumBackground(theVar,theRegion,theChannel,theRelevantHistograms)
                                                     );
          }
      }

     private:

    };

}

#endif
