//Core C++
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <future>
#include <mutex>



//Other
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


//All this does is return the number of events stored in a file

int main(int argc, char** argv) {

    std::string pathToFile = argv[1];
    TFile dataFile(pathToFile.c_str(), "dataFile", "READ");
    TTree* dataTree = (TTree*)dataFile.Get("LWTree");
    Int_t dataCount = dataTree->GetEntries();
    dataFile.Close();
    std::cout << dataCount << std::endl;
    //return dataCount;
    
    



    (void)argc;
}
    