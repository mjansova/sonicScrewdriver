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

            static TH1D Compute(TH1D* signalHisto, TH1D* backgrHisto, short int cutType, OptionsScrewdriver theGlobalOptions)
            {

                string type = theGlobalOptions.GetGlobalStringOption("FigureOfMerit","type");
                if (type == "") type = "Zbi";

                float Bmin = theGlobalOptions.GetGlobalFloatOption("FigureOfMerit","minBackground");
                float f = theGlobalOptions.GetGlobalFloatOption("FigureOfMerit","backgroundSystematicUncertainty");

                TH1D theFOM = *((TH1D*) signalHisto->Clone());
                int nBins = theFOM.GetNbinsX();

                if (backgrHisto == 0)
                {
                    WARNING_MSG << "Background histogram for Figure of Merit computation is empty ?" << endl;
                    theFOM.Scale(0);
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

                    if (B < Bmin) B = Bmin;

                    float FOM = -1;
                    if (type == "sOverSqrtB")
                    {
                        FOM = S / sqrt(B + f*f*B*B);
                    }
                    else if (type == "sOverSqrtSplusB")
                    {
                        FOM = S / sqrt(S + B + f*f*B*B);
                    }
                    else if (type == "Zbi")
                    {
                        FOM = Zbi(S,B,f);
                    }

                    theFOM.SetBinContent(i,FOM);
                    theFOM.SetBinError(i,0.0);
                }

                return theFOM;
            };

            // From arXiv:physics/0702156
            static float Zbi(float n_sig, float n_b, float rel_uncert_b)
            {
                float  n_on     = n_sig+n_b;
                float  mu_b_hat = n_b;
                float  sigma_b  = rel_uncert_b*n_b;
                float  tau      = mu_b_hat/(sigma_b*sigma_b);
                float  n_off    = tau*mu_b_hat;
                float  P_Bi     = TMath::BetaIncomplete(1./(1.+tau),n_on,n_off+1);
                float  Z_Bi     = sqrt(2)*TMath::ErfInverse(1 - 2*P_Bi);

                return Z_Bi;
            };


    };

}

#endif
