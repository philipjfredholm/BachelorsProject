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


    std::string storageLocation {"bias3.root"};
    TFile dataFile(storageLocation.c_str(), "dataFile", "READ");
    TH2D* histogramForward = (TH2D*)dataFile.Get("histogram");
    TH2D histogram = *histogramForward;
    dataFile.Close();


    histogram.Draw("surf1");
    canvas.Modified(); 
    canvas.Update();
    app.Run();

    
}