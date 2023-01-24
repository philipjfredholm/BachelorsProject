//Core C++
#include <iostream>
#include <string>
#include <vector>

//ROOT
#include <TFile.h>
#include <TTree.h>
#include <TObject.h>
#include "AliLWUtils.h"


#include <TRint.h>
#include <TApplication.h>
#include <TBranch.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TClonesArray.h>







int main(int argc, char **argv) {
    //Makes the canvas
    //TRint* app = new TRint("app", &argc, argv);
    //TCanvas* canvas = new TCanvas("canvas", "Title", 0, 0 ,800,600);

    //Reads in the ROOT tree
    TFile* dataFile = new TFile("wipData/sampleData.root", "dataFile", "READ");
    TTree* dataTree = (TTree*)dataFile->Get("LWTree");
    //Int_t dataCount = dataTree->GetEntries();

    TClonesArray* myTrack = new TClonesArray("AliLWFMDTrack");
    dataTree->Print();
    dataTree->SetBranchAddress("TPCTracks", &myTrack);
    dataTree->GetEntry(444);
    std::cout << myTrack->GetEntries() << std::endl;
    AliLWFMDTrack* hello = static_cast<AliLWFMDTrack*>(myTrack->At(4));
    std::cout << hello->fEta << std::endl;

    //The data structure is that we have a TCloneArray filled with AliLWFMDTrack:s, with one per each event!

   // dataTree->Print();

    /*
    TClonesArray* currentTrack;
    //dataTree->Print();
    dataTree->SetBranchAddress("TPCTracks", &currentTrack);
    dataTree->GetEntry(4);
    std::cout << currentTrack->Capacity() << std::endl;
    //dataTree->GetEntry(6);
    std::cout << (*currentTrack)[2] << std::endl;


    dataFile->Close();

    std::cout << "Hello World!" << std::endl;
    */

/*



    TH1D* hist = new TH1D("hist", "title", 100,0,100);
    hist->Fill(50);
    hist->Fill(5);
    hist->Draw();
    
    


    canvas->Modified(); //Checks if the canvas has been modified in the event loop
    canvas->Update();
    app->Run();
    dataFile->Close();
    */
}


