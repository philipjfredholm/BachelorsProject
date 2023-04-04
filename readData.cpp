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
    int phiBins = 12;
    int etaBins = 70;
    double etaMin = -6;
    double etaMax = 10;
    Short_t cutOption = 3;
    double centralityStart = 40;
    double centralityStop = 60;
    double ptStart = 0.5; //What unit is this?
    double ptStop = 1;



    std::string pathToFile = argv[1];
    storeInHist myHistogram {pathToFile, cutOption, 
                             centralityStart, centralityStop,
                             ptStart, ptStop,
                             etaMin, etaMax,
                             phiBins, etaBins};



    (void)argc;

}


