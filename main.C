//C++ Libraries
#include <iostream>
#include <string>
#include <vector>


//ROOT Libraries and Files
#include <TFile.h>
#include <TTree.h>
#include "src/AliLWUtils.cxx"


void test() {

    TFile* dataFile = new TFile("wipData/sampleData.root", "dataFile", "READ");

    TTree* dataTree = (TTree*)dataFile->Get("LWTree");

    cout << dataTree->GetEntries() << endl;


    dataFile->Close();

    




}