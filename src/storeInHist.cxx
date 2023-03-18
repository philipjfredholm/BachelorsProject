#include "../include/storeInHist.h"

ClassImp(storeInHist);



storeInHist::storeInHist(std::string pathToFile) : _pathToFile{pathToFile} {
    //Opens the file to be read
    TFile dataFile("wipData/sampleData.root", "dataFile", "READ");
    TTree* dataTree = (TTree*)dataFile.Get("LWTree");
    Int_t binCount = 200;
    TH1D* histogram = new TH1D("histogram", "Counts", binCount, 0, 2*TMath::Pi());

    //Creates variables to write the read-in variables to.
    //AliLWEvent* event;
    TClonesArray* tpcTrack = new TClonesArray("AliLWTPCTrack");
    TClonesArray* fmdTrack = new TClonesArray("AliLWFMDTrack");
    //dataTree->SetBranchAddres("Event", &event);
    dataTree->SetBranchAddress("TPCTracks", &tpcTrack);
    dataTree->SetBranchAddress("FMDTracks", &fmdTrack);
    


    //Reads in the data and fills the histogram
    Int_t dataCount = dataTree->GetEntries(); //Number of entries in the tree
    Int_t trackCountTPC;
    Int_t trackCountFMD;
    Double_t phiValTPC;
    Double_t phiValFMD;
    Int_t fmdMult;
    Double_t phiDiff;
    AliLWTPCTrack* currentTrackTPC;
    AliLWFMDTrack* currentTrackFMD;

    //if (dataCount > 200) {dataCount = 100;};


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
                //This loops through each measured angle bin in an FMD event
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

    //Now all the relevant data is stored in the histogram.
    

    TFile writeData("histograms.root", "RECREATE");
    writeData.WriteObject(histogram, "test");

    writeData.Close();
    dataFile.Close();


}