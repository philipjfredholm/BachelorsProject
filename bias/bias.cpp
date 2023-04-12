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



std::vector<int> bias1(int N, int M) {
    int P = N + M - 1;

    std::vector<int> bias {};

    

    int biasValue;
    for (int p = 1; p <= P; p++) {
        biasValue = 0;
        biasValue += p;

        if (p > M) {
            biasValue -= p-M;
        }

        if (p > N) {
            biasValue -= p-N;
        }

        bias.push_back(biasValue);

        

    }

    
    return bias;



}


int main(int argc, char** argv) {
    TRint app("app", &argc, argv);
    TCanvas canvas("canvas", "Title", 0, 0 ,800,600);

    //Note that these values must be picked such taht
    // v=w for this demo to work.
    int N = 10;
    int M = 10; 
    int P = N+M-1; 

    //int P = N+M-1;
    double a = 2;
    double b = 5;
    double c = 7;
    double d = 10;

    double v = (b-a)/N;
    //double w = (d-c)/M; //v has to equal w for this demo to work
    double aPrime = a + 0.5*v;
    double bPrime = b - 0.5*v;
    double cPrime = c + 0.5*v;
    double dPrime = d - 0.5*v;

    std::vector<int> bias= bias1(N, M);

    TH1D histogram1("histogram1", "Counts", N, a, b);
    TH1D histogram2("histogram2", "Counts", M, c, d);
    TH1D histogram3("histogram3", "Counts", P, std::abs(bPrime-cPrime)-0.5*v, std::abs(aPrime-dPrime)+0.5*v);

    for (int n = 1; n <= N; n++) {
        histogram1.AddBinContent(n);

    }

    for (int m = 1; m <= M; m++) {
        histogram2.AddBinContent(m);

    }


    for (int p = 0; p < P; p++) {
        histogram3.AddBinContent(p+1, bias[p]);

    }

    histogram3.Draw();
    




    canvas.Modified(); 
    canvas.Update();
    app.Run();

    
}