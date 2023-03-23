//Core C++
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <future>
#include <mutex>


//Other
#include "include/storeInHist.h"
#include "AliLWUtils.h"

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







int main(int argc, char **argv) {
    //Makes the application
    TRint app("app", &argc, argv);
    TCanvas canvas("canvas", "Title", 0, 0 ,800,600);


    //Code
    storeInHist myHistogram {"wipData/sampleData.root"};
    TH2D histogram = myHistogram.getHist();
    histogram.Draw("surf1");


    //Runs the application
    canvas.Modified(); 
    canvas.Update();
    app.Run();



}


