//Core C++
#include <iostream>
#include <string>
#include <vector>

//ROOT
#include <TH1D.h>
#include <TFile.h>
#include <TTree.h>

#include "TObject.h"
#include "AliLWUtils.h"



int main() {
/*

    TFile* dataFile = new TFile("wipData/sampleData.root", "dataFile",  "READ");
    TTree* dataTree = (TTree*)dataFile->Get("LWTree");
    Int_t dataCount = dataTree->GetEntries();

    std::cout << dataCount << std::endl;

    dataFile->Close();
*/
    TFile* dataFile = new TFile("wipData/sampleData.root", "dataFile", "READ");

    TTree* dataTree = (TTree*)dataFile->Get("LWTree");

    std::cout << dataTree->GetEntries() << std::endl;


    dataFile->Close();

}
