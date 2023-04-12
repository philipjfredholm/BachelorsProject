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



int main(int argc, char** argv) {
    TRint app("app", &argc, argv);
    TCanvas canvas("canvas", "Title", 0, 0 ,800,600);

    //Parameters
    int N = 10; //Bins in first histogram
    int M = 10; //Bins in second histogram
    int P = 19 ; //Bins in correlation histogram
    double a = 2.001; //First histogram lower end
    double b = 5; //First histogram upper end
    double c = 7; //Second histogram lower end
    double d = 10; //Second histogram upper end

    double aPrime = a + 0.5*v; //Centre of the first bin of the first histogram
    double bPrime = b - 0.5*v; //Centre of the last bin of the first histogram
    double cPrime = c + 0.5*w; //Centre of the first bin of the second histogram
    double dPrime = d - 0.5*w; //Centre of the last bin of the second histogram


    double v = (b-a)/N; //Bin width of the first histogram
    double w = (d-c)/M; //Bin width of the second histogram
    double u = (1.0/P)*( v*(N-1) + w*(M-1) ); //Bin width of the correlation histogram
    
 

    //Fills the first and second histogram to one entry per bins
    TH1D histogram1("histogram1", "Counts", N, a, b);
    TH1D histogram2("histogram2", "Counts", M, c, d);
    TH1D histogram3("histogram3", "Counts", P, std::abs(bPrime-cPrime)-0.5*u, std::abs(aPrime-dPrime)+0.5*u);
    
    //Adds 1 to every bin in the histograms
    for (int n = 1; n <= N; n++) {
        histogram1.AddBinContent(n);
    }

    for (int m = 1; m <= M; m++) {
        histogram2.AddBinContent(m);
    }


    double difference = 0;
    for (int n = 1; n <= N; n++) {
        for (int m = 1; m <= M; m++) {
            difference =  (cPrime + w*(m-1)) - (aPrime + v*(n-1));
            histogram3.Fill(difference);
            

        }     
    }

    histogram3.Draw();

    canvas.Modified(); 
    canvas.Update();
    app.Run();

    
}