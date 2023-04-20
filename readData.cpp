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
    int phiBins = 20; //20 bins for the fmd:s
    int etaBins = 320; //400 bins over -10 to 10
    double etaMin = -6;
    double etaMax = 10;
    Short_t cutOption = 3;
    double centralityStart = 50;
    double centralityStop = 90;
    double ptStart = 0.2; //What unit is this?
    double ptStop = 6;


    std::string pathToFile = argv[1];
    std::string startString = argv[2];
    std::string stopString = argv[3];
    int start = std::stoi(startString);
    int stop = std::stoi(stopString);
    storeInHist myHistogram {pathToFile, cutOption, 
                             centralityStart, centralityStop,
                             ptStart, ptStop,
                             etaMin, etaMax,
                             phiBins, etaBins,
                             start, stop};



    (void)argc;

}


