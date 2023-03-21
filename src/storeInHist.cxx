#include "../include/storeInHist.h"

ClassImp(storeInHist);



storeInHist::storeInHist(std::string pathToFile) : _pathToFile{pathToFile} {
    //Opens the file to be read
    TFile dataFile(pathToFile.c_str(), "dataFile", "READ");
    TTree* dataTree = (TTree*)dataFile.Get("LWTree");
    Int_t binCount = 20; //Number of bins the interval [0,2pi] is to be divided over
    TH1D* histogram = new TH1D("histogram", "Counts", binCount, 0, 2*TMath::Pi());

    //Creates variables to write the read-in variables to.
    //AliLWEvent* event;
    TClonesArray* tpcTrack = new TClonesArray("AliLWTPCTrack");
    TClonesArray* fmdTrack = new TClonesArray("AliLWFMDTrack");
    //dataTree->SetBranchAddres("Event", &event);
    dataTree->SetBranchAddress("TPCTracks", &tpcTrack);
    dataTree->SetBranchAddress("FMDTracks", &fmdTrack);
    


    //Variables needed in the for-loops
    Int_t dataCount = dataTree->GetEntries(); //Number of entries in the tree
    std::cout << dataCount << std::endl;
    Int_t trackCountTPC;
    Int_t trackCountFMD;
    Double_t phiValTPC;
    Double_t phiValFMD;
    Int_t fmdMult;
    Double_t phiDiff;
    AliLWTPCTrack* currentTrackTPC;
    AliLWFMDTrack* currentTrackFMD;

    if (dataCount > 200) {dataCount = 2000;}; //For faster read times when debugging

    //Reads in the data and fills the histogram
    for (Int_t eventNumber = 0; eventNumber < dataCount; eventNumber++) {
        //This loops through each event
        dataTree->GetEntry(eventNumber);
        trackCountTPC = tpcTrack->GetEntries(); //Number of tracks in the current event
        trackCountFMD = fmdTrack->GetEntries();

        for (Int_t tpcTrackNumber = 0; tpcTrackNumber < trackCountTPC; tpcTrackNumber++) {
            //This loops through each track in a TPC event
            currentTrackTPC = static_cast<AliLWTPCTrack*>((*tpcTrack)[tpcTrackNumber]); //tpcTrack->At(m) is the same as (*tpcTrack)[m]
            phiValTPC = currentTrackTPC->fPhi;
            
            for (Int_t fmdTrackNumber = 0; fmdTrackNumber < trackCountFMD; fmdTrackNumber++) {
                //This loops through each measured angle bin in an FMD event
                currentTrackFMD = static_cast<AliLWFMDTrack*>((*fmdTrack)[fmdTrackNumber]); 
                fmdMult = currentTrackFMD->fMult;
                phiValFMD = currentTrackFMD->fPhi;
                phiDiff = phiValTPC - phiValFMD;

                //Makes sures all values are positive
                if (phiDiff < 0) {
                    phiDiff += 2*TMath::Pi();
                }

                //This weights the phi-difference by the number of multiplicities in the FMD
                for (Int_t p = 0; p < fmdMult; p++) {
                    
                    histogram->Fill(phiDiff);
                }
            }
        } 
    }

    



    //Now all the relevant data is stored.
    
    this->storedHistogram = *histogram; //Storing in the class object

    std::string storageLocation = pathToFile.substr(pathToFile.find("/")+1, pathToFile.length()) +"Processed"+ ".root"; 
    std::cout << storageLocation << std::endl;
    
    /*bool storageLocationExists = std::filesystem::exists("storedHistograms");
    if (storageLocationExists == false) {
        std::filesystem::create_directory("storedHistograms");
    }
    */

    TFile writeData(storageLocation.c_str(), "RECREATE"); //Storing in a root-file for later read-in
    writeData.WriteObject(histogram, "test");

    writeData.Close();
    dataFile.Close();


}


TH1D storeInHist::getHist() {
    return this->storedHistogram;
}