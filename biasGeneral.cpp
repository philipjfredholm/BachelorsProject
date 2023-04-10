//Core C++
#include <iostream>
#include <string>
#include <vector>



//ROOT
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TObject.h>
#include <TRint.h>
#include <TApplication.h>
#include <TBranch.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TClonesArray.h>
#include <TMath.h>



std::vector<int> bias2(int P, double tau, double sigma, int N, int M) {
    int tauFloor = std::floor(tau);
    int tauCeil = std::ceil(1/tau);

    std::cout << sigma << std::endl;
    std::vector<int> bias {};
    int internalValue;
    int doubleInternalValue;
    double addValue;

    for (int k = 1; k <= P; k++) {
        internalValue = 0;


        for (int p = 1; p <= P; p++) {
            doubleInternalValue = 0;

            for (int pPrime = 1; pPrime <= p; pPrime++){
                addValue = 1;
                if ((p > M) && (pPrime > M)) {
                    addValue -= 1;
                }   

                if ((p > N) && (pPrime > N)) {
                    addValue -= 1;
                }    


                if ( (k-p+1)*tauFloor - pPrime*tauCeil - std::floor(p*sigma) > 0 ) {
                    if ( pPrime*tauCeil + std::floor(p*sigma) - (k-p)*tauFloor  > 0) {
                        doubleInternalValue+= addValue;
                    }
                }

            
            }
            

            internalValue += doubleInternalValue;

            
        }


        bias.push_back(internalValue);
        
        
    }


    std::cout << bias.size() << std::endl;
    return bias;



}


int main(int argc, char** argv) {
    TRint app("app", &argc, argv);
    TCanvas canvas("canvas", "Title", 0, 0 ,800,600);

    //Parameters
    int N = 30;
    int M = 30;
    int P = 85 ;
    double a = 2.01;
    double b = 5;
    double c = 7;
    double d = 10;

    double v = (b-a)/N;
    double w = (d-c)/M;
    double u = (1.0/P)*( v*(N-1) + w*(M-1) );
    double tau = u/(std::abs(v-w));
    double sigma = std::abs(u-w)/u;


    double aPrime = a + 0.5*v;
    double bPrime = b - 0.5*v;

    double cPrime = c + 0.5*w;
    double dPrime = d - 0.5*w;

    

    //Fills the first and second histogram to one entry per bins
    TH1D histogram1("histogram1", "Counts", N, a, b);
    TH1D histogram2("histogram2", "Counts", M, c, d);
    TH1D histogram3("histogram3", "Counts", P, std::abs(bPrime-cPrime)-0.5*u, std::abs(aPrime-dPrime)+0.5*u);

    
    for (int n = 1; n <= N; n++) {
        histogram1.AddBinContent(n);
    }

    for (int m = 1; m <= M; m++) {
        histogram2.AddBinContent(m);
    }


    
    //Fills the difference histogram
    
    
    std::vector<int> bias= bias2(P, tau, sigma, N, M);

    for (int p = 0; p < P; p++) {
        histogram3.AddBinContent(p+1, bias[p]);
        //std::cout << bias[p] << std::endl;
    }

    histogram3.Draw();
    




    canvas.Modified(); 
    canvas.Update();
    app.Run();

    
}