#include "../include/storeInHist.h"

ClassImp(storeInHist);


//Getters and Setters

void storeInHist::setStorageName(std::string location) {
    this->_pathToFile = location;
}


std::string storeInHist::getFilePath() {
    return this->_pathToFile;
}


const TH2D storeInHist::getForwardHistogram() {
    return this->_storedForwardHistogram;
}

const TH2D storeInHist::getBackwardHistogram() {
    return this->_storedBackwardHistogram;
}

const TH2D storeInHist::getBackToBackHistogram() {
    return this->_storedBackToBackHistogram;
}



//Methods

void storeInHist::addHistogram(storeInHist secondHistogram) {
    if (this->_initialised == 0) {
        this->_storedForwardHistogram = secondHistogram.getForwardHistogram();
        this->_storedBackwardHistogram = secondHistogram.getBackwardHistogram();
        this->_storedBackToBackHistogram = secondHistogram.getBackToBackHistogram();
        this->_pathToFile = secondHistogram.getFilePath();
        this->_initialised = 1;

    } else { 
        TH2D secondForwardHistogramCopy = secondHistogram.getForwardHistogram(); //Making a copy is necessary because g++ complains about references to "r-value" otherwise
        TH2D secondBackwardHistogramCopy = secondHistogram.getBackwardHistogram();
        TH2D secondBackToBackHistogramCopy = secondHistogram.getBackToBackHistogram();
        this->_storedForwardHistogram.Add(&secondForwardHistogramCopy);
        this->_storedBackwardHistogram.Add(&secondBackwardHistogramCopy);
        this->_storedBackToBackHistogram.Add(&secondBackToBackHistogramCopy);

    }

    
}


void storeInHist::storeHistogramInFile() {
    //Sets the new filename
    if (std::filesystem::exists("./processedData") == false) {
        std::filesystem::create_directory("./processedData");
    }

    const std::string filename = _pathToFile.substr(_pathToFile.find("/")+1, _pathToFile.length());
    std::string storageLocation = "processedData/"+filename.substr(0, filename.find_last_of(".")) +"Processed"+ ".root"; 

    //Stores the histogram
    TH2D* histogramForwardPointer = &(this->_storedForwardHistogram);
    TH2D* histogramBackwardPointer = &(this->_storedBackwardHistogram);
    TH2D* histogramBackToBackPointer = &(this->_storedBackToBackHistogram);
    TFile writeData(storageLocation.c_str(), "RECREATE"); 
    writeData.WriteObject(histogramForwardPointer, "processedDataForwardHistogram");
    writeData.WriteObject(histogramBackwardPointer, "processedDataBackwardHistogram");
    writeData.WriteObject(histogramBackToBackPointer, "processedDataBackToBackHistogram");
    writeData.Close();

}





//Constructors


//Dummy default constructor since the default constructor is taken by inheritance from TObject
storeInHist::storeInHist(Int_t number) {
    this->_initialised = 0;
    this->_pathToFile = "";
    (void)number;
}


//Initialisation by reading a file
storeInHist::storeInHist(std::string pathToFile) : _pathToFile{pathToFile} {
    TFile dataFile(pathToFile.c_str(), "dataFile", "READ");
    TH2D* histogramForward = (TH2D*)dataFile.Get("processedDataForwardHistogram");
    TH2D* histogramBackward = (TH2D*)dataFile.Get("processedDataBackwardHistogram");
    TH2D* histogramBackToBack = (TH2D*)dataFile.Get("processedDataBackToBackHistogram");

    this->_storedForwardHistogram = *histogramForward;
    this->_storedBackwardHistogram = *histogramBackward;
    this->_storedBackToBackHistogram = *histogramBackToBack;
    this->_initialised = 1;
}


//Actual Constructor
storeInHist::storeInHist(std::string pathToFile, Short_t cutOption, 
                                                    Double_t centralityMin, Double_t centralityMax,
                                                    Double_t ptMin, Double_t ptMax,
                                                    Double_t etaMin, Double_t etaMax,
                                                    Int_t countsPhi, Int_t countsEta) : _pathToFile{pathToFile} {
    //Gets the number of entries in the tree
    TFile dataFile(pathToFile.c_str(), "dataFile", "READ");
    TTree* dataTree = (TTree*)dataFile.Get("LWTree");
    Int_t dataCount = dataTree->GetEntries();
    dataFile.Close();
    

    //loadHistogram creates the three histograms that are wanted. Separate method as it is very lengthy.
    std::vector<TH2D> histogramVector = loadHistogram(pathToFile, cutOption,
                                                      centralityMin, centralityMax,
                                                      ptMin, ptMax,
                                                      etaMin, etaMax,
                                                      countsPhi, countsEta,
                                                      0, dataCount);
    this->_storedForwardHistogram = histogramVector[0];
    this->_storedBackwardHistogram = histogramVector[1];
    this->_storedBackToBackHistogram = histogramVector[2];
    this->_initialised = 1;
    storeHistogramInFile();



}


//Helper-method to a constructor to read in the data properly into the correct histograms
std::vector<TH2D> storeInHist::loadHistogram(std::string pathToFile, Short_t cutOption, 
                                            Double_t centralityMin, Double_t centralityMax,
                                            Double_t ptMin, Double_t ptMax,
                                            Double_t etaMin, Double_t etaMax,
                                            Int_t countsPhi, Int_t countsEta,
                                            Int_t start, Int_t stop) {

    //Creates the histograms which will be filled
    TH2D histogramForward("histogramForward", "Counts", countsPhi, 0, 2*TMath::Pi(), countsEta, etaMin, etaMax);
    TH2D histogramBackward("histogramBackWard", "Counts", countsPhi, 0, 2*TMath::Pi(), countsEta, etaMin, etaMax);
    TH2D histogramBackToBack("histogramBackToBack", "Counts", countsPhi, 0, 2*TMath::Pi(), countsEta, etaMin, etaMax);
    
    
    //Opens the data
    TFile dataFile(pathToFile.c_str(), "dataFile", "READ");
    TTree* dataTree = (TTree*)dataFile.Get("LWTree");

    
    //Creates variables to write the read-in variables to.
    AliLWEvent* event = new AliLWEvent; //Not great with raw pointers, but ROOT requires pointers and refuses smart pointers
    TClonesArray* tpcTrack = new TClonesArray("AliLWTPCTrack"); 
    TClonesArray* fmdTrack = new TClonesArray("AliLWFMDTrack");
    dataTree->SetBranchAddress("Event", &event);
    dataTree->SetBranchAddress("TPCTracks", &tpcTrack);
    dataTree->SetBranchAddress("FMDTracks", &fmdTrack);


    //Variables for looping
    Int_t trackCountTPC;
    Int_t trackCountFMD;
    AliLWTPCTrack* currentTrackTPC;
    AliLWFMDTrack* currentTrackFMD;


    //Event-variables
    Double_t centrality;

    //TPC-variables
    Double_t phiValTPC;
    Double_t etaValTPC;
    Short_t cutFlag;
    Double_t pT; //Tranvsverse momentum
    

    //FMD-variables
    Double_t phiValFMD; //For TPC-FMD correlations
    Double_t etaValFMD;
    Int_t fmdMultiplicity;

    Double_t phiForwardFMD; //For FMD-FMD correlations
    Double_t phiBackwardFMD;
    Double_t etaForwardFMD;
    Double_t etaBackwardFMD;
    Int_t forwardMult;
    Int_t backwardMult;
    std::vector<Double_t> forwardTracksPhi;
    std::vector<Double_t> backwardTracksPhi;
    std::vector<Double_t> forwardTracksEta;
    std::vector<Double_t> backwardTracksEta; 
    std::vector<Int_t> forwardTracksMult;
    std::vector<Int_t> backwardTracksMult;

    
    //Values to be stored in the histograms
    Double_t etaDiff; //For TPC-FMD correlations
    Double_t phiDiff;

    Double_t phiDiffBackToBack; //For FMD-FMD correlations
    Double_t etaDiffBackToBack;


    //Event-loop
    for (Int_t eventNumber = start; eventNumber < stop; eventNumber++) {
        //Reads in the tracks for an event
        dataTree->GetEntry(eventNumber);
        trackCountTPC = tpcTrack->GetEntries(); 
        trackCountFMD = fmdTrack->GetEntries();


        //Checks if the centrality is within the desired region        
        centrality = event->fCent;
        if ((centrality < centralityMin) || (centrality > centralityMax)) {
            continue;
        }


        //Clears Data from a previous event
        forwardTracksEta.clear();
        backwardTracksEta.clear();
        forwardTracksPhi.clear();
        backwardTracksPhi.clear();
        forwardTracksMult.clear();
        backwardTracksMult.clear();

            
        //Loops through all tracks in the FMD
        for (Int_t fmdTrackNumber = 0; fmdTrackNumber < trackCountFMD; fmdTrackNumber++) {
            //Gets details about the track
            currentTrackFMD = static_cast<AliLWFMDTrack*>((*fmdTrack)[fmdTrackNumber]); 
            fmdMultiplicity = currentTrackFMD->fMult;
            phiValFMD = currentTrackFMD->fPhi;
            etaValFMD = currentTrackFMD->fEta;


            //Cutting away data where the resolution is low
            if ((etaValFMD < -3.1) || (etaValFMD > -2)) {
                if ((etaValFMD < 3.8) || (etaValFMD > 4.7)) {
                    if ((etaValFMD < 2.5) || (etaValFMD > 3.1)) {
                        continue;

                    }
                }
            }

            //Stores values for the forward and backward FMD-tracks.
            if (etaValFMD >= 0) { 
                forwardTracksEta.push_back(etaValFMD);
                forwardTracksPhi.push_back(phiValFMD);
                forwardTracksMult.push_back(fmdMultiplicity);
                
            } else { 
                backwardTracksEta.push_back(etaValFMD);
                backwardTracksPhi.push_back(phiValFMD);
                backwardTracksMult.push_back(fmdMultiplicity);

            }

            //Loops through all tracks in the TPC so TPC-FMD correlations can be calculated
            for (Int_t tpcTrackNumber = 0; tpcTrackNumber < trackCountTPC; tpcTrackNumber++) {
                currentTrackTPC = static_cast<AliLWTPCTrack*>((*tpcTrack)[tpcTrackNumber]);
                phiValTPC = currentTrackTPC->fPhi;
                etaValTPC = currentTrackTPC->fEta;
                pT = currentTrackTPC->fPt;
                cutFlag = currentTrackTPC->fTrFlag;
                phiDiff = phiValTPC - phiValFMD;
                etaDiff = etaValTPC - etaValFMD;


                //Cutting away data where with the wrong flag(s).
                if (cutOption == 3) {
                    //Intentionally does nothing
                } else {
                    if (cutFlag != cutOption) {
                        continue;
                    }
                }

                //Cuts away unwanted pT:s
                if ((pT < ptMin) || (pT > ptMax)) {
                    continue;
                }


                //Cutting away data where the resolution is low
                if ((etaValTPC < -0.75) || (etaValTPC > 0.75)) { 
                    continue;
                } 


                //Makes sures all values are positive (we want all values within one period)
                if (phiDiff < 0) {phiDiff += 2*TMath::Pi();}


                //Fills the correct histogram. The sign determines if it is the forwards or backwards FMD
                if (etaValFMD > 0) {
                    histogramForward.Fill(phiDiff, etaDiff, fmdMultiplicity);

                } else {
                    histogramBackward.Fill(phiDiff, etaDiff, fmdMultiplicity);

                }

            } //TPC-loop end





        } //FMD-loop end



        //Calculates FMD-FMD correlations
        for (int forwardTrackNumber = 0; forwardTrackNumber < static_cast<int>(forwardTracksEta.size()); forwardTrackNumber++) {
            phiForwardFMD = forwardTracksPhi[forwardTrackNumber];
            etaForwardFMD = forwardTracksEta[forwardTrackNumber];
            forwardMult = forwardTracksMult[forwardTrackNumber];

            for (int backwardTrackNumber = 0; backwardTrackNumber < static_cast<int>(backwardTracksEta.size()); backwardTrackNumber++) {
                phiBackwardFMD = backwardTracksPhi[backwardTrackNumber];
                etaBackwardFMD = backwardTracksEta[backwardTrackNumber];
                backwardMult = backwardTracksMult[backwardTrackNumber];

                phiDiffBackToBack = phiForwardFMD - phiBackwardFMD;
                etaDiffBackToBack = etaForwardFMD - etaBackwardFMD;

                histogramBackToBack.Fill(phiDiffBackToBack, etaDiffBackToBack, forwardMult*backwardMult);
            }
        }




      
    } //Event-loop end  


    //Closes files and removes objects from the heap
    dataFile.Close();
    delete tpcTrack;
    delete fmdTrack;
    

    //Returns the results
    std::vector<TH2D> returnVector;
    returnVector.push_back(histogramForward);
    returnVector.push_back(histogramBackward);
    returnVector.push_back(histogramBackToBack);

    return returnVector;


}



std::vector<TH2D> storeInHist::loadBackground(std::string pathToFile, Short_t cutOption, 
                                            Double_t centralityMin, Double_t centralityMax,
                                            Double_t ptMin, Double_t ptMax,
                                            Double_t etaMin, Double_t etaMax,
                                            Int_t countsPhi, Int_t countsEta,
                                            Int_t start, Int_t stop) {

    //Creates the histograms which will be filled
    TH2D histogramForward("histogramForward", "Counts", countsPhi, 0, 2*TMath::Pi(), countsEta, etaMin, etaMax);
    TH2D histogramBackward("histogramBackWard", "Counts", countsPhi, 0, 2*TMath::Pi(), countsEta, etaMin, etaMax);
    TH2D histogramBackToBack("histogramBackToBack", "Counts", countsPhi, 0, 2*TMath::Pi(), countsEta, etaMin, etaMax);
    
    
    //Opens the data
    TFile dataFile(pathToFile.c_str(), "dataFile", "READ");
    TTree* dataTree = (TTree*)dataFile.Get("LWTree");

    
    //Creates variables to write the read-in variables to.
    AliLWEvent* event = new AliLWEvent; //Not great with raw pointers, but ROOT requires pointers and refuses smart pointers
    TClonesArray* tpcTrack = new TClonesArray("AliLWTPCTrack"); 
    TClonesArray* fmdTrack = new TClonesArray("AliLWFMDTrack");
    dataTree->SetBranchAddress("Event", &event);
    dataTree->SetBranchAddress("TPCTracks", &tpcTrack);
    dataTree->SetBranchAddress("FMDTracks", &fmdTrack);



    //Variables for looping
    Int_t trackCountTPC;
    Int_t trackCountFMD;
    AliLWTPCTrack* currentTrackTPC;
    AliLWFMDTrack* currentTrackFMD;


    //Event-variables
    Double_t centrality;

    //TPC-variables
    Double_t phiValTPC;
    Double_t etaValTPC;
    Short_t cutFlag;
    Double_t pT; //Tranvsverse momentum

    std::vector<Double_t> tracksPhiTPC;
    std::vector<Double_t> tracksEtaTPC;
    

    //FMD-variables
    Double_t phiValFMD; //For TPC-FMD correlations
    Double_t etaValFMD;
    Int_t fmdMultiplicity;

    Double_t phiForwardFMD; //For FMD-FMD correlations
    Double_t phiBackwardFMD;
    Double_t etaForwardFMD;
    Double_t etaBackwardFMD;
    Int_t forwardMult;
    Int_t backwardMult;
    std::vector<Double_t> forwardTracksPhi;
    std::vector<Double_t> backwardTracksPhi;
    std::vector<Double_t> forwardTracksEta;
    std::vector<Double_t> backwardTracksEta; 
    std::vector<Int_t> forwardTracksMult;
    std::vector<Int_t> backwardTracksMult;

    
    //Values to be stored in the histograms
    Double_t etaDiff; //For TPC-FMD correlations
    Double_t phiDiff;

    Double_t phiDiffBackToBack; //For FMD-FMD correlations
    Double_t etaDiffBackToBack;


    //Storing old events' data
    std::vector<std::vector<Double_t>> oldPhiTracksTPC;
    std::vector<std::vector<Double_t>> oldEtaTracksTPC;

    std::vector<std::vector<Double_t>> oldPhiTracksForwardFMD;
    std::vector<std::vector<Double_t>> oldEtaTracksForwardFMD;
    std::vector<std::vector<Int_t>> oldMultiplicityTracksForwardFMD;

    std::vector<std::vector<Double_t>> oldPhiTracksBackwardFMD;
    std::vector<std::vector<Double_t>> oldEtaTracksBackwardFMD;
    std::vector<std::vector<Int_t>> oldMultiplicityTracksBackwardFMD;


     //Loads in the first 5 events satisfying our conditions
    Int_t startCounter = 0;
    while (oldPhiTracksTPC.size() < 5) {
        //Reads in information about the event
        dataTree->GetEntry(startCounter);
        trackCountTPC = tpcTrack->GetEntries(); 
        trackCountFMD = fmdTrack->GetEntries();


        //Cuts away unwanted centralities
        centrality = event->fCent;
        if ((centrality < centralityMin) || (centrality > centralityMax)) {
            continue;
        }


        //Clears Data from a previous event
        forwardTracksEta.clear();
        backwardTracksEta.clear();
        forwardTracksPhi.clear();
        backwardTracksPhi.clear();
        forwardTracksMult.clear();
        backwardTracksMult.clear();


        //Loops through all tracks in the FMD
        for (Int_t fmdTrackNumber = 0; fmdTrackNumber < trackCountFMD; fmdTrackNumber++) {
            //Gets details about the track
            currentTrackFMD = static_cast<AliLWFMDTrack*>((*fmdTrack)[fmdTrackNumber]); 
            fmdMultiplicity = currentTrackFMD->fMult;
            phiValFMD = currentTrackFMD->fPhi;
            etaValFMD = currentTrackFMD->fEta;


            //Cutting away data where the resolution is low
            if ((etaValFMD < -3.1) || (etaValFMD > -2)) {
                if ((etaValFMD < 3.8) || (etaValFMD > 4.7)) {
                    if ((etaValFMD < 2.5) || (etaValFMD > 3.1)) {
                        continue;

                    }
                }
            }

            //Stores values for the forward and backward FMD-tracks.
            if (etaValFMD >= 0) { 
                forwardTracksPhi.push_back(phiValFMD);
                forwardTracksEta.push_back(etaValFMD); 
                forwardTracksMult.push_back(fmdMultiplicity);
                
            } else { 
                backwardTracksPhi.push_back(phiValFMD);
                backwardTracksEta.push_back(etaValFMD);
                backwardTracksMult.push_back(fmdMultiplicity);

            }

            //Loops through all tracks in the TPC so TPC-FMD correlations can be calculated
            for (Int_t tpcTrackNumber = 0; tpcTrackNumber < trackCountTPC; tpcTrackNumber++) {
                currentTrackTPC = static_cast<AliLWTPCTrack*>((*tpcTrack)[tpcTrackNumber]);
                phiValTPC = currentTrackTPC->fPhi;
                etaValTPC = currentTrackTPC->fEta;
                pT = currentTrackTPC->fPt;
                cutFlag = currentTrackTPC->fTrFlag;


                //Cutting away data where with the wrong flag(s).
                if (cutOption == 3) {
                    //Intentionally does nothing
                } else {
                    if (cutFlag != cutOption) {
                        continue;
                    }
                }

                //Cuts away unwanted pT:s
                if ((pT < ptMin) || (pT > ptMax)) {
                    continue;
                }


                //Cutting away data where the resolution is low
                if ((etaValTPC < -0.75) || (etaValTPC > 0.75)) { 
                    continue;
                } 

                
                //Stores the read-in and approved values
                tracksPhiTPC.push_back(phiValTPC);
                tracksEtaTPC.push_back(etaValTPC);

            } //TPC-loop end


        } //FMD-loop end

        oldPhiTracksTPC.push_back(tracksPhiTPC);
        oldEtaTracksTPC.push_back(tracksEtaTPC);

        oldPhiTracksForwardFMD.push_back(forwardTracksPhi);
        oldEtaTracksForwardFMD.push_back(forwardTracksEta);
        oldMultiplicityTracksForwardFMD.push_back(forwardTracksMult);

        oldPhiTracksBackwardFMD.push_back(backwardTracksPhi);
        oldEtaTracksBackwardFMD.push_back(backwardTracksEta);
        oldMultiplicityTracksBackwardFMD.push_back(backwardTracksMult);



        startCounter++;
    }


    



    for (Int_t eventNumber = startCounter; eventNumber < stop; eventNumber++) {
        //Reads in information about the event
        dataTree->GetEntry(startCounter);
        trackCountTPC = tpcTrack->GetEntries(); 
        trackCountFMD = fmdTrack->GetEntries();


        //Cuts away unwanted centralities
        centrality = event->fCent;
        if ((centrality < centralityMin) || (centrality > centralityMax)) {
            continue;
        }


        //Clears Data from a previous event
        forwardTracksEta.clear();
        backwardTracksEta.clear();
        forwardTracksPhi.clear();
        backwardTracksPhi.clear();
        forwardTracksMult.clear();
        backwardTracksMult.clear();


        //Loops through all tracks in the FMD
        for (Int_t fmdTrackNumber = 0; fmdTrackNumber < trackCountFMD; fmdTrackNumber++) {
            //Gets details about the track
            currentTrackFMD = static_cast<AliLWFMDTrack*>((*fmdTrack)[fmdTrackNumber]); 
            fmdMultiplicity = currentTrackFMD->fMult;
            phiValFMD = currentTrackFMD->fPhi;
            etaValFMD = currentTrackFMD->fEta;


            //Cutting away data where the resolution is low
            if ((etaValFMD < -3.1) || (etaValFMD > -2)) {
                if ((etaValFMD < 3.8) || (etaValFMD > 4.7)) {
                    if ((etaValFMD < 2.5) || (etaValFMD > 3.1)) {
                        continue;

                    }
                }
            }

            //Stores values for the forward and backward FMD-tracks.
            if (etaValFMD >= 0) { 
                forwardTracksPhi.push_back(phiValFMD);
                forwardTracksEta.push_back(etaValFMD); 
                forwardTracksMult.push_back(fmdMultiplicity);
                
            } else { 
                backwardTracksPhi.push_back(phiValFMD);
                backwardTracksEta.push_back(etaValFMD);
                backwardTracksMult.push_back(fmdMultiplicity);

            }

            //Loops through all tracks in the TPC so TPC-FMD correlations can be calculated
            for (Int_t tpcTrackNumber = 0; tpcTrackNumber < trackCountTPC; tpcTrackNumber++) {
                currentTrackTPC = static_cast<AliLWTPCTrack*>((*tpcTrack)[tpcTrackNumber]);
                phiValTPC = currentTrackTPC->fPhi;
                etaValTPC = currentTrackTPC->fEta;
                pT = currentTrackTPC->fPt;
                cutFlag = currentTrackTPC->fTrFlag;


                //Cutting away data where with the wrong flag(s).
                if (cutOption == 3) {
                    //Intentionally does nothing
                } else {
                    if (cutFlag != cutOption) {
                        continue;
                    }
                }

                //Cuts away unwanted pT:s
                if ((pT < ptMin) || (pT > ptMax)) {
                    continue;
                }


                //Cutting away data where the resolution is low
                if ((etaValTPC < -0.75) || (etaValTPC > 0.75)) { 
                    continue;
                } 

                
                //Stores the read-in and approved values
                tracksPhiTPC.push_back(phiValTPC);
                tracksEtaTPC.push_back(etaValTPC);

            } //TPC-loop end


        } //FMD-loop end


}