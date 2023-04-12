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
    std::cout << tau << std::endl;

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
                if ((std::floor(p*(N+M-1)/P) > M) && (std::floor(pPrime*(N+M-1)/P) > M)) {
                    addValue -= 1;
                }   

                if ((std::floor(p*(N+M-1)/P) > N) && (std::floor(pPrime*(N+M-1)/P) > N)) {
                    addValue -= 1;
                }    

                
                if ( (k-p+1)*tauFloor - pPrime*tauCeil + std::floor(p*sigma) > 0 ) {
                    if ( pPrime*tauCeil - std::floor(p*sigma) - (k-p)*tauFloor  > 0) {
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



std::vector<int> bias3(int P, double tau, double sigma, int N, int M, double u, double v, double w, double aPrime, double cPrime, double bPrime, double dPrime) {
    std::vector<int> bias {};
    double diff;
    (void)sigma;
    (void)dPrime;
    (void)tau;

    for (int k = 1; k <= P; k++) {

        

        int counter = 0;



        for (int n = 1; n <= N; n++) {
            for (int m = 1; m <= M; m++) {


                diff = -aPrime - v*(n-1) + (cPrime + w*(m-1));
                if ((std::abs(bPrime-cPrime)-0.5*u+ k*u >= diff) && ( std::abs(bPrime-cPrime)-0.5*u+(k-1)*u < diff)) {
                    counter++;
                }

            }
        }

        bias.push_back(counter);

    }



    return bias;
}


int main(int argc, char** argv) {
    TRint app("app", &argc, argv);
    TCanvas canvas("canvas", "Title", 0, 0 ,800,600);

    //Parameters
    int N = 10;
    int M = 10;
    int P = 19 ;
    double a = 2.001;
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
    TH1D histogram4("histogram4", "Counts", P, std::abs(bPrime-cPrime)-0.5*u, std::abs(aPrime-dPrime)+0.5*u);
    
    for (int n = 1; n <= N; n++) {
        histogram1.AddBinContent(n);
    }

    for (int m = 1; m <= M; m++) {
        histogram2.AddBinContent(m);
    }


    //For testing that the answer agrees with the numerical calculations
    double diff = 0;
    for (int n = 1; n <= N; n++) {
        for (int m = 1; m <= M; m++) {
            diff = -aPrime - v*(n-1) + (cPrime + w*(m-1));
            std::cout << diff << std::endl;
            histogram4.Fill(diff);
            

        }     
    }





    //Fills the difference histogram
    
    
    //std::vector<int> bias= bias2(P, tau, sigma, N, M);
    std::vector<int> bias= bias3(P, tau, sigma, N, M, u, v, w, aPrime, cPrime, bPrime, dPrime);
    for (int p = 0; p < P; p++) {
        histogram3.AddBinContent(p+1, bias[p]);
        //std::cout << bias[p] << std::endl;
    }

    histogram3.Draw();
    


    std::cout << "u is " << u << std::endl;

    canvas.Modified(); 
    canvas.Update();
    app.Run();

    
}