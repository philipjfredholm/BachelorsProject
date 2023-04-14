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
    int countsPhi1 = 10000; 
    int countsPhi2 = 100;  
    int countsEta1 = 1000; 
    int countsEta2 = 100;
    int countsPhiDiff = 18;
    int countsEtaDiff = 70;


    double phiStart1 = 0; 
    double phiEnd1 = 2*TMath::Pi();
    double etaStart1 = -0.75;
    double etaEnd1 = 0.75; 
    double phiStart2 = 0; 
    double phiEnd2 = 2*TMath::Pi();
    double etaStart2 = -3.1;
    double etaEnd2 = -2; 

    //Derived Parameters
    double binWidthPhi1 = (phiEnd1-phiStart1)/countsPhi1;
    double binWidthEta1 = (etaEnd1-etaStart1)/countsEta1; 
    double binWidthPhi2 = (phiEnd2-phiStart2)/countsPhi2;
    double binWidthEta2 = (etaEnd2-etaStart2)/countsEta2; 
    
 
    double firstBinCentrePhi1 = phiStart1 + 0.5*binWidthPhi1; 
    double firstBinCentreEta1 = etaStart1 + 0.5*binWidthEta1;
    double firstBinCentrePhi2 = phiStart2 + 0.5*binWidthPhi2;
    double firstBinCentreEta2 = etaStart2 + 0.5*binWidthEta2; 



    TH2D histogram("histogram3", "Counts", countsPhiDiff, 0, 2*TMath::Pi(), countsEtaDiff, -6, 10);
    



    double differencePhi = 0;
    double differenceEta = 0;

    for (int phiBin1 = 1; phiBin1 < countsPhi1; phiBin1++) {
        for (int phiBin2 = 1; phiBin2 < countsPhi2; phiBin2++) {
            differencePhi = firstBinCentrePhi2 + binWidthPhi2*(phiBin2-1) - ( firstBinCentrePhi1 + binWidthPhi1*(phiBin1 -1) );
            if (differencePhi < 0) {differencePhi += 2*TMath::Pi();}

            for (int etaBin1 = 1; etaBin1 < countsEta1; etaBin1++) {
                for (int etaBin2 = 1; etaBin2 < countsEta2; etaBin2++) {
                    differenceEta = firstBinCentreEta2 + binWidthEta2*(etaBin2-1) - ( firstBinCentreEta1 + binWidthEta1*(etaBin1-1));

                    histogram.Fill(differencePhi, differenceEta);

                }
            }



        }

    }

    std::string storageLocation {"bias3.root"};
    TFile writeData(storageLocation.c_str(), "RECREATE"); 
    TH2D* histogramPointer {&histogram};
    writeData.WriteObject(histogramPointer, "histogram");

    histogram.Draw("surf1");
    canvas.Modified(); 
    canvas.Update();
    app.Run();

    
}