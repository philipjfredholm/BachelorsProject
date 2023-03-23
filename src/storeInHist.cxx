#include "../include/storeInHist.h"

ClassImp(storeInHist);



storeInHist::storeInHist(std::string pathToFile) : _pathToFile{pathToFile} {
    //Gets the number of entries in the tree
    TFile dataFile(pathToFile.c_str(), "dataFile", "READ");
    TTree* dataTree = (TTree*)dataFile.Get("LWTree");
    Int_t dataCount = dataTree->GetEntries();
    dataFile.Close();
    
    //Parameters for reading in data and creating histograms
    Int_t dataFraction = dataCount/4;
    Int_t binCountsX = 15;
    Int_t binCountsY = 50;

    //Sets up return values
    std::promise<TH2D> returnHistogramSetter1;
    std::future<TH2D> returnHistogramGetter1 = returnHistogramSetter1.get_future();

    std::promise<TH2D> returnHistogramSetter2;
    std::future<TH2D> returnHistogramGetter2 = returnHistogramSetter2.get_future();    

    
    std::promise<TH2D> returnHistogramSetter3;
    std::future<TH2D> returnHistogramGetter3 = returnHistogramSetter3.get_future();

    std::promise<TH2D> returnHistogramSetter4;
    std::future<TH2D> returnHistogramGetter4 = returnHistogramSetter4.get_future();


    std::thread worker1(&storeInHist::loadHistogram, this, std::move(returnHistogramSetter1), 
                                    pathToFile, dataFraction*0, dataFraction*1, binCountsX, binCountsY, "uno");

    
    
    std::thread worker2(&storeInHist::loadHistogram, this, std::move(returnHistogramSetter2), 
                                    "wipData/sampleData2.root", dataFraction*1, dataFraction*2, binCountsX, binCountsY, "dos");

    

    std::thread worker3(&storeInHist::loadHistogram, this, std::move(returnHistogramSetter3), 
                                    "wipData/sampleData3.root", dataFraction*2, dataFraction*3, binCountsX, binCountsY, "tres");

    
    std::thread worker4(&storeInHist::loadHistogram, this, std::move(returnHistogramSetter4), 
                                    "wipData/sampleData4.root", dataFraction*3, dataCount, binCountsX, binCountsY, "quatro");

    
    worker1.join();
    worker2.join();
    worker3.join();
    worker4.join();
    

    TH2D histogram1 = returnHistogramGetter1.get();
    TH2D histogram2 = returnHistogramGetter2.get();
    TH2D histogram3 = returnHistogramGetter3.get();
    TH2D histogram4 = returnHistogramGetter4.get();



    histogram1.Add(&histogram2);
    histogram1.Add(&histogram3);
    histogram1.Add(&histogram4);
    
    this->_storedHistogram = histogram1;
    storeHistogramInFile();



}




void storeInHist::loadHistogram(std::promise<TH2D> myPromise, std::string pathToFile, Int_t start, Int_t stop, 
                                Int_t countsX, Int_t countsY, std::string histname) {

    //Opens the data

    TFile dataFile(pathToFile.c_str(), "dataFile", "READ");
    TTree* dataTree = (TTree*)dataFile.Get("LWTree");
    
    std::cout << dataTree->GetEntries() << std::endl;
    std::cout << histname << std::endl;

    //Creates variables to write the read-in variables to.
    //Not great with raw pointers, but ROOT requires pointers and refuses smart pointers
    TClonesArray* tpcTrack = new TClonesArray("AliLWTPCTrack");
    TClonesArray* fmdTrack = new TClonesArray("AliLWFMDTrack");
    std::cout << "pointers defined" << std::endl;
    dataTree->SetBranchAddress("TPCTracks", &tpcTrack);
    dataTree->SetBranchAddress("FMDTracks", &fmdTrack);
    std::cout << "branches defined" << std::endl;


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
    TH2D histogram(histname.c_str(), "Counts", countsX, 0, 2*TMath::Pi(), countsY, -6, 6);
    std::cout << "loop starts here" << std::endl;
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
    
    std::cout << "loop over" << std::endl;
    delete tpcTrack;
    delete fmdTrack;
    

    myPromise.set_value(histogram);
    dataFile.Close();



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


/*
TTree* storeInHist::getTree(std::string pathToFile) {
    std::mutex myMutex;
    myMutex.lock();

    TFile dataFile(pathToFile.c_str(), "dataFile", "READ");
    TTree* dataTreePrimitive = (TTree*)dataFile.Get("LWTree");
    gROOT->cd(); //Something about telling ROOT to allow copies
    TTree* dataTree = dataTreePrimitive->CloneTree();
    //dataTree->CopyEntries(dataTreePrimitive);

    dataFile.Close();

    myMutex.unlock();
    return dataTree;
}
*/