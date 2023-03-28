//Core C++
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <future>
#include <mutex>
#include <filesystem>


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
    int phiBins = 60;
    int etaBins = 135;
    double etaMin = -6;
    double etaMax = 10;
    Short_t cutOption = 3;

    std::string pathToFile = argv[1];
    storeInHist myHistogram {pathToFile, phiBins, etaBins, etaMin, etaMax, cutOption};



    (void)argc;

}


