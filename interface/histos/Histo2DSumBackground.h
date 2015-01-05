#ifndef Histo2DSumBackground_h
#define Histo2DSumBackground_h

#include "interface/histos/Histo1D.h"
#include "interface/Figure.h"

namespace theDoctor
{

    class Histo2DSumBackground : public Histo2D
    {

     public:

      Histo2DSumBackground(Variable* theXVar_,
                           Variable* theYVar_,
                           Region* theRegion_,
                           Channel* theChannel_,
                           vector<Histo2DEntries*> theBackgrounds) :
      Histo2D(Name("2DSumBackground","Entries"),theXVar_,theYVar_,theRegion_,theChannel_)
      {
          string nameHisto =  string("vX:")+theXVar->getTag()
                                   +"|vY:"+theYVar->getTag()
                                   +"|r:" +theRegion->getTag()
                                   +"|c:" +theChannel->getTag()
                                   +"|t:" +theHistoType.getTag();

          theHisto->SetName(nameHisto.c_str());

          // Loop on the input histograms and add each of them to the histo
          for (unsigned int i = 0 ; i < theBackgrounds.size() ; i++)
          {
              theHisto->Add(theBackgrounds[i]->getClone());
          }
      };

      ~Histo2DSumBackground() { };

      static void GetHistoDependencies(vector<pair<string,string> >& output, string options = "")
      {
      }

      static void Produce(vector<Variable>* theVariables,
                          vector<ProcessClass>* theProcessClasses,
                          vector<Region>* theRegions,
                          vector<Channel>* theChannels,
                          HistoScrewdriver* theHistoScrewdriver,
                          OptionsScrewdriver theGlobalOptions,
                          string histoOptions)
      {
          vector<Histo2DEntries>* histo2DList = theHistoScrewdriver->Get2DHistosEntries();
          std::set< pair<string,string> > varCouples;

          for (unsigned int i = 0 ; i < histo2DList->size() ; i++)
          {
              string varX = ((*histo2DList)[i]).getVariableX()->getTag();
              string varY = ((*histo2DList)[i]).getVariableY()->getTag();

              pair<string,string> couple(varX,varY);

              varCouples.insert(couple);
          }

          for (std::set< pair<string,string> >::iterator varCouple = varCouples.begin() ; varCouple != varCouples.end() ; varCouple++)
          {

              string varX = varCouple->first;
              string varY = varCouple->second;

              // Browse the (var x reg x chan)
              for (unsigned int vX = 0 ; vX < theVariables->size() ; vX++)
              for (unsigned int vY = 0 ; vY < theVariables->size() ; vY++)
              {
                  Variable* theXVar = &((*theVariables)[vX]);
                  Variable* theYVar = &((*theVariables)[vY]);

                  if (theXVar->getTag() != varX) continue;
                  if (theYVar->getTag() != varY) continue;

                  for (unsigned int r = 0 ; r < theRegions->size()   ; r++)
                  for (unsigned int c = 0 ; c < theChannels->size()  ; c++)
                  {
                      vector<Histo2DEntries*> theBackgrounds;

                      Region*   theRegion  = &((*theRegions)[r]);
                      Channel*  theChannel = &((*theChannels)[c]);

                      // Now loop on the histos
                      for (unsigned int i = 0 ; i < theProcessClasses->size() ; i++)
                      {
                          // If this processClass is not a background, we skip it
                          if ((*theProcessClasses)[i].getType() != "background") continue;
                          // If it it, we add it to the relevant backgrounds
                          //
                          Histo2DEntries* thisBackground = theHistoScrewdriver->get2DHistoEntriesPointer(theXVar->getTag(),
                                                                                                         theYVar->getTag(),
                                                                                                         (*theProcessClasses)[i].getTag(),
                                                                                                         theRegion->getTag(),
                                                                                                         theChannel->getTag());
                          theBackgrounds.push_back(thisBackground);
                      }

                      theHistoScrewdriver->Add2DHistoForPlots(
                                                                Histo2DSumBackground(theXVar,theYVar,theRegion,theChannel,theBackgrounds)
                                                             );
                  }
              }
          }
      }

     private:

    };

}

#endif
