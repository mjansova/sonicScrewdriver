#ifndef FigureOfMerit_h
#define FigureOfMerit_h

#include "interface/Common.h" 

using namespace std;

namespace theDoctor
{

    class FigureOfMerit 
    {

        public:

            FigureOfMerit() { }
            ~FigureOfMerit() { }

            static TH1F Compute(TH1F* signalHisto, TH1F* backgrHisto, short int cutType, OptionsScrewdriver theGlobalOptions)
            {
                float f = theGlobalOptions.GetGlobalFloatOption("FigureOfMerit","backgroundSystematicUncertainty");

                TH1F theFOM = *((TH1F*) signalHisto->Clone());
                int nBins = theFOM.GetNbinsX();

                if (backgrHisto == 0)
                {
                    WARNING_MSG << "Background histogram for Figure of Merit computation is empty ?" << endl;
                    return theFOM;
                }

                for (int i = 1 ; i <= nBins ; i++)
                {

                    float S;
                    float B;

                    // Keep-high-value case
                    if (cutType == 1) { S = signalHisto->Integral(i,nBins+1); B = backgrHisto->Integral(i,nBins+1); }
                    // Keep-low-value case
                    else              { S = signalHisto->Integral(0,i);       B = backgrHisto->Integral(0,i);       }

                    if (B < 1) B = 1;
                    float sOverSqrtB = 0.0;
                    if (S >= 3) sOverSqrtB = S / sqrt(B + f*f * B*B);

                    theFOM.SetBinContent(i,sOverSqrtB);
                    theFOM.SetBinError(i,0.0);
                }

                return theFOM;
            }; 

    };

}

#endif
