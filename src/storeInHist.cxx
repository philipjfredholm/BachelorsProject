#include "../include/storeInHist.h"

ClassImp(storeInHist);



storeInHist::storeInHist(std::string pathToFile) : _pathToFile{pathToFile} {
    //Gets the number of entries in the tree
    TFile dataFile(pathToFile.c_str(), "dataFile", "READ");
    TTree* dataTree = (TTree*)dataFile.Get("LWTree");
    Int_t dataCount = dataTree->GetEntries();
    dataFile.Close();
    (void)dataCount;

    Int_t binCountsX = 15;
    Int_t binCountsY = 50;
    TH2D histogram = loadHistogram(pathToFile, 0, 200, binCountsX, binCountsY);
    this->_storedHistogram = histogram;
    storeHistogramInFile();



}




const TH2D storeInHist::loadHistogram(std::string pathToFile, Int_t start, Int_t stop, 
                                Int_t countsX, Int_t countsY) {

    //Opens the data
    TFile dataFile(pathToFile.c_str(), "dataFile", "READ");
    TTree* dataTree = (TTree*)dataFile.Get("LWTree");
    

    //Creates variables to write the read-in variables to.
    //Not great with raw pointers, but ROOT requires pointers and refuses smart pointers
    TClonesArray* tpcTrack = new TClonesArray("AliLWTPCTrack");
    TClonesArray* fmdTrack = new TClonesArray("AliLWFMDTrack");
    dataTree->SetBranchAddress("TPCTracks", &tpcTrack);
    dataTree->SetBranchAddress("FMDTracks", &fmdTrack);


    //Variables needed in the for-loops
    Int_t trackCountTPC;
    Int_t trackCountFMD;
    Double_t phiValTPC;
    Double_t phiValFMD;
    Double_t etaValTPC;
    Double_t etaValFMD;
    Double_t etaDiff;
    Int_t fmdMult;
    Double_t phiDiff;
    AliLWTPCTrack* currentTrackTPC;
    AliLWFMDTrack* currentTrackFMD;


    //Reads in the data and fills the histogram
    TH2D histogram("histogram", "Counts", countsX, 0, 2*TMath::Pi(), countsY, -6, 6);

    //Loops through each event
    for (Int_t eventNumber = start; eventNumber < stop; eventNumber++) {
        dataTree->GetEntry(eventNumber);
        trackCountTPC = tpcTrack->GetEntries(); //Number of tracks in the current event
        trackCountFMD = fmdTrack->GetEntries();

        
        //This loops through each track in a TPC event
        for (Int_t tpcTrackNumber = 0; tpcTrackNumber < trackCountTPC; tpcTrackNumber++) {
            currentTrackTPC = static_cast<AliLWTPCTrack*>((*tpcTrack)[tpcTrackNumber]);
            phiValTPC = currentTrackTPC->fPhi;
            etaValTPC = currentTrackTPC->fEta;

            
            //This loops through each measured angle bin in an FMD event
            for (Int_t fmdTrackNumber = 0; fmdTrackNumber < trackCountFMD; fmdTrackNumber++) {
                currentTrackFMD = static_cast<AliLWFMDTrack*>((*fmdTrack)[fmdTrackNumber]); 
                fmdMult = currentTrackFMD->fMult;
                phiValFMD = currentTrackFMD->fPhi;
                etaValFMD = currentTrackFMD->fEta;
                phiDiff = phiValTPC - phiValFMD;
                etaDiff = etaValTPC-etaValFMD;

                //Makes sures all values are positive
                if (phiDiff < 0) {phiDiff += 2*TMath::Pi();}

                histogram.Fill(phiDiff, etaDiff, fmdMult);


            }
        } 
    }


    //Closes files and removes objects from the heap
    dataFile.Close();
    delete tpcTrack;
    delete fmdTrack;
    
    /*
    It would be better to return a pointer to larger object like this histogram and 
    implement a proper accompanying destructor (RAII), but ROOT or some optimisation (-O3 ?) 
    seems to be messing this up. The histogram gets destroyed when out of scope of the function 
    even if I place it on the heap. Since ROOT handles pointers on its own when using its 
    interpreter, this might be a bug in the standalone version.
    */
    return histogram;


}




const TH2D storeInHist::getHist() {
    return this->_storedHistogram;
}




void storeInHist::storeHistogramInFile() {
    //Sets the new filename
    const std::string filename = _pathToFile.substr(_pathToFile.find("/")+1, _pathToFile.length());
    std::string storageLocation = filename.substr(0, filename.find_last_of(".")) +"Processed"+ ".root"; 

    //Stores the histogram
    TH2D* histogramPointer = &(this->_storedHistogram);
    TFile writeData(storageLocation.c_str(), "RECREATE"); 
    writeData.WriteObject(histogramPointer, "processedDataHistogram");
    writeData.Close();

}