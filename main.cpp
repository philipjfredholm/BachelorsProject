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
#include <TMath.h>







int main(int argc, char **argv) {
    //Makes the histogram
    TRint* app = new TRint("app", &argc, argv);
    TCanvas* canvas = new TCanvas("canvas", "Title", 0, 0 ,800,600);
    TH1D* histogram = new TH1D("histogram", "Counts", 1000, 0, 2*TMath::Pi());

    //Reads in the ROOT tree
    TFile* dataFile = new TFile("wipData/sampleData.root", "dataFile", "READ");
    TTree* dataTree = (TTree*)dataFile->Get("LWTree");

    //AliLWEvent* event;
    TClonesArray* tpcTrack = new TClonesArray("AliLWTPCTrack");
    TClonesArray* fmdTrack = new TClonesArray("AliLWFMDTrack");
    
    //dataTree->SetBranchAddres("Event", &event);
    dataTree->SetBranchAddress("TPCTracks", &tpcTrack);
    dataTree->SetBranchAddress("FMDTracks", &fmdTrack);
    


    //Reads in the data and fills the histogram
    Int_t dataCount = dataTree->GetEntries();
    Int_t trackCountTPC;
    Int_t trackCountFMD;
    Double_t phiValTPC;
    Double_t phiValFMD;
    Int_t fmdMult;
    Double_t phiDiff;
    AliLWTPCTrack* currentTrackTPC;
    AliLWFMDTrack* currentTrackFMD;

    //if (dataCount > 200) {dataCount = 2000;};


    for (Int_t n = 0; n < dataCount; n++) {
        //This loops through each event
        dataTree->GetEntry(n);
        trackCountTPC = tpcTrack->GetEntries();
        trackCountFMD = fmdTrack->GetEntries();

        for (Int_t m = 0; m < trackCountTPC; m++) {
            //This loops through each track in a TPC event
            currentTrackTPC = static_cast<AliLWTPCTrack*>(tpcTrack->At(m)); //tpcTrack->At(m) is the same as (*tpcTrack)[m]
            phiValTPC = currentTrackTPC->fPhi;
            
            for (Int_t o = 0; o < trackCountFMD; o++) {
                currentTrackFMD = static_cast<AliLWFMDTrack*>(fmdTrack->At(0)); 
                fmdMult = currentTrackFMD->fMult;
                phiValFMD = currentTrackFMD->fPhi;
                phiDiff = phiValTPC - phiValFMD;
                for (Int_t p = 0; p < fmdMult; p++) {
                    //This weights the phi-difference by the number of multiplicities in the FMD
                    histogram->Fill(phiDiff);
                }
            }
        } 
    }

    

    //Draws the histogram
    histogram->Draw();
    canvas->Modified(); //Checks if the canvas has been modified in the event loop
    canvas->Update();
    app->Run();
    dataFile->Close();

    
    /*
    TClonesArray* myTrack = new TClonesArray("AliLWFMDTrack");
    dataTree->Print();
    dataTree->SetBranchAddress("TPCTracks", &myTrack);
    dataTree->GetEntry(444);
    std::cout << myTrack->GetEntries() << std::endl;
    AliLWFMDTrack* hello = static_cast<AliLWFMDTrack*>(myTrack->At(4));
    std::cout << hello->fEta << std::endl;
    */
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
    */
    
    



}


