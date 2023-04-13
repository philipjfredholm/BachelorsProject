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

const TH2D storeInHist::getForwardBackground() {
    return this->_noCorrelationForward;
}

const TH2D storeInHist::getBackwardBackground() {
    return this->_noCorrelationBackward;
}

const TH2D storeInHist::getBackToBackBackground() {
    return this->_noCorrelationBackToBack;
}

const TH2D storeInHist::getForwardProcessed() {
    return this->_processedForward;
}

const TH2D storeInHist::getBackwardProcessed() {
    return this->_processedBackward;
}

const TH2D storeInHist::getBackToBackProcessed() {
    return this->_processedBackToBack;
}



//Methods
void storeInHist::calculateCorrelation(TH2D& myHistogram, const std::vector<Double_t>& phi1, const std::vector<Double_t>& eta1,
                                  const std::vector<Double_t>& phi2, const std::vector<Double_t>& eta2,
                                  const std::vector<Int_t>& mult1, const std::vector<Int_t>& mult2) {


    //This is just for my own debugging, in a public member function we of course properly throw errors
    if (phi1.size() != eta1.size()) {
        std::cout << "Unequal Sizes detect" << std::endl;


    }

    if (phi2.size() != eta2.size()) {
        std::cout << "Unequal Sizes detect" << std::endl;


    }
    

    Double_t phiDiff;
    Double_t etaDiff;
    Double_t multiplicity;


    for (int detector1Track = 0; detector1Track < static_cast<int>(phi1.size()); detector1Track++) {
        for (int detector2Track = 0; detector2Track < static_cast<int>(phi2.size()); detector2Track++) {
            
            phiDiff = phi1[detector1Track] - phi2[detector2Track];
            if (phiDiff < 0) {
                phiDiff += 2*TMath::Pi();
            }

            etaDiff = eta1[detector1Track] - eta2[detector2Track];
            multiplicity = mult1[detector1Track] * mult2[detector2Track];
            myHistogram.Fill(phiDiff, etaDiff, multiplicity);


        }

    }


}
        
void storeInHist::calculateCorrelation(TH2D& myHistogram, const std::vector<Double_t>& phi1, const std::vector<Double_t>& eta1,
                                  const std::vector<Double_t>& phi2, const std::vector<Double_t>& eta2,
                                  const std::vector<Int_t>& mult2) {


    //This is just for my own debugging, in a public member function we of course throw errors
    if (phi1.size() != eta1.size()) {
        std::cout << "Unequal Sizes detect" << std::endl;


    }

    if (phi2.size() != eta2.size()) {
        std::cout << "Unequal Sizes detect" << std::endl;


    }


    Double_t phiDiff;
    Double_t etaDiff;

    for (int detector1Track = 0; detector1Track < static_cast<int>(phi1.size()); detector1Track++) {
        for (int detector2Track = 0; detector2Track < static_cast<int>(phi2.size()); detector2Track++) {
            phiDiff = phi1[detector1Track] - phi2[detector2Track];
            if (phiDiff < 0) {
                phiDiff += 2*TMath::Pi();
            }


            etaDiff = eta1[detector1Track] - eta2[detector2Track];
            myHistogram.Fill(phiDiff, etaDiff, mult2[detector2Track]);

            
        }

    }


}


void storeInHist::addHistogram(storeInHist secondHistogram) {
    if (this->_initialised == 0) {
        //Raw
        this->_storedForwardHistogram = secondHistogram.getForwardHistogram();
        this->_storedBackwardHistogram = secondHistogram.getBackwardHistogram();
        this->_storedBackToBackHistogram = secondHistogram.getBackToBackHistogram();


        //Background
        this->_noCorrelationForward = secondHistogram.getForwardBackground();
        this->_noCorrelationBackward = secondHistogram.getBackwardBackground();
        this->_noCorrelationBackToBack = secondHistogram.getBackToBackBackground();


        //Normalised
        this->_processedForward = secondHistogram.getForwardProcessed();
        this->_processedBackward = secondHistogram.getBackwardProcessed();
        this->_processedBackToBack = secondHistogram.getBackToBackProcessed(); 

        //Initialisation
        this->_pathToFile = secondHistogram.getFilePath();
        this->_initialised = 1;



    } else { 
        //Raw
        TH2D secondForwardHistogramCopy = secondHistogram.getForwardHistogram(); //Making a copy is necessary because g++ complains about references to "r-value" otherwise
        TH2D secondBackwardHistogramCopy = secondHistogram.getBackwardHistogram();
        TH2D secondBackToBackHistogramCopy = secondHistogram.getBackToBackHistogram();
        
        this->_storedForwardHistogram.Add(&secondForwardHistogramCopy);
        this->_storedBackwardHistogram.Add(&secondBackwardHistogramCopy);
        this->_storedBackToBackHistogram.Add(&secondBackToBackHistogramCopy);       
        
        //Background
        TH2D secondForwardBackgroundCopy = secondHistogram.getForwardBackground();
        TH2D secondBackwardBackgroundCopy = secondHistogram.getBackwardBackground();
        TH2D secondBackToBackBackgroundCopy = secondHistogram.getBackToBackBackground();

        this->_noCorrelationForward.Add(&secondForwardBackgroundCopy);
        this->_noCorrelationBackward.Add(&secondBackwardBackgroundCopy);
        this->_noCorrelationBackToBack.Add(&secondBackToBackBackgroundCopy);


        //Noramlised
        TH2D secondForwardProcessed = secondHistogram.getForwardProcessed();
        TH2D secondBackwardProcessed = secondHistogram.getBackwardProcessed();
        TH2D secondBackToBackProcessed = secondHistogram.getBackToBackProcessed();

        this->_processedForward.Add(&secondForwardProcessed);
        this->_processedBackward.Add(&secondBackwardProcessed);
        this->_processedBackToBack.Add(&secondBackToBackProcessed);


    }

    


}


void storeInHist::storeHistogramInFile() {
    //Sets the new filename
    const std::string filename = _pathToFile.substr(_pathToFile.find_last_of("/")+1, _pathToFile.length());
    std::string storageLocation = "processedData/"+filename.substr(0, filename.find_last_of(".")) +"Processed"+ ".root"; 

    //Stores the histogram
    TH2D* histogramForwardPointer = &(this->_storedForwardHistogram);
    TH2D* histogramBackwardPointer = &(this->_storedBackwardHistogram);
    TH2D* histogramBackToBackPointer = &(this->_storedBackToBackHistogram);

    TH2D* backgroundForwardPointer = &(this->_noCorrelationForward);
    TH2D* backgroundBackwardPointer = &(this->_noCorrelationBackward);
    TH2D* backgroundBackToBackPointer = &(this->_noCorrelationBackToBack);

    TH2D* processedForwardPointer = &(this->_processedForward);
    TH2D* processedBackwardPointer = &(this->_processedBackward);
    TH2D* processedBackToBackPointer = &(this->_processedBackToBack);


    TFile writeData(storageLocation.c_str(), "RECREATE"); 

    writeData.WriteObject(histogramForwardPointer, "dataForwardHistogram");
    writeData.WriteObject(histogramBackwardPointer, "dataBackwardHistogram");
    writeData.WriteObject(histogramBackToBackPointer, "dataBackToBackHistogram");

    writeData.WriteObject(backgroundForwardPointer, "dataForwardHistogramBackground");
    writeData.WriteObject(backgroundBackwardPointer, "dataBackwardHistogramBackground");
    writeData.WriteObject(backgroundBackToBackPointer, "dataBackToBackHistogramBackground");


    writeData.WriteObject(processedForwardPointer, "dataForwardHistogramProcessed");
    writeData.WriteObject(processedBackwardPointer, "dataBackwardHistogramProcessed");
    writeData.WriteObject(processedBackToBackPointer, "dataBackToBackHistogramProcessed");

    writeData.Close();

}

void storeInHist::loadProcessed() {
    if (this->_initialised == 0) {
        //Just for debugging, if the code was for public use the 
        //errors should be using std::throw
        std::cout << "Error: Trying to process unloaded histograms" << std::endl;
    }

    TH2D histogramForward = this->getForwardHistogram();
    TH2D histogramBackward = this->getBackwardHistogram();
    TH2D histogramBackToBack = this->getBackToBackHistogram();

    TH2D histogramForwardBackground = this->getForwardBackground();
    TH2D histogramBackwardBackground = this->getBackwardBackground();
    TH2D histogramBackToBackBackground = this->getBackToBackBackground();
    
    Double_t maxValueForward = histogramForwardBackground.GetMaximum();
    Double_t maxValueBackward = histogramBackwardBackground.GetMaximum();
    Double_t maxValueBackToBack = histogramBackToBackBackground.GetMaximum();
    
    // *= syntax does not seem to be implemented for TH2D
    TH2D normalisedForwardBackground = (1/maxValueForward)*histogramForwardBackground;
    TH2D normalisedBackwardBackground = (1/maxValueBackward)*histogramBackwardBackground;
    TH2D normalisedBackToBackBackground = (1/maxValueBackToBack)*histogramBackToBackBackground;

    histogramForward.Divide(&normalisedForwardBackground);
    histogramBackward.Divide(&normalisedBackwardBackground);
    histogramBackToBack.Divide(&normalisedBackToBackBackground);

    this->_processedForward = histogramForward;
    this->_processedBackward = histogramBackward;
    this->_processedBackToBack = histogramBackToBack;
}





//Constructors


//Dummy default constructor since the default constructor is taken by inheritance from TObject
storeInHist::storeInHist(Int_t number) {
    this->_initialised = 0;
    this->_pathToFile = "";
    (void)number; //Deliberately not using static_cast<void>(number) since I do not care what happens to the number, 
                    //it is just to turn off warnings of unused variables.
}


//Initialisation by reading a file
storeInHist::storeInHist(std::string pathToFile) : _pathToFile{pathToFile} {
    TFile dataFile(pathToFile.c_str(), "dataFile", "READ");

    TH2D* histogramForward = (TH2D*)dataFile.Get("dataForwardHistogram");
    TH2D* histogramBackward = (TH2D*)dataFile.Get("dataBackwardHistogram");
    TH2D* histogramBackToBack = (TH2D*)dataFile.Get("dataBackToBackHistogram");
    
    TH2D* histogramForwardBackground = (TH2D*)dataFile.Get("dataForwardHistogramBackground");
    TH2D* histogramBackwardBackground = (TH2D*)dataFile.Get("dataBackwardHistogramBackground");
    TH2D* histogramBackToBackBackground = (TH2D*)dataFile.Get("dataBackToBackHistogramBackground");
    
    this->_storedForwardHistogram = *histogramForward;
    this->_storedBackwardHistogram = *histogramBackward;
    this->_storedBackToBackHistogram = *histogramBackToBack;

    this->_noCorrelationForward = *histogramForwardBackground;
    this->_noCorrelationBackward = *histogramBackwardBackground;
    this->_noCorrelationBackToBack = *histogramBackToBackBackground;


    //Old versions of the program did not save the processed histograms as member variables 
    // nor in the file, this is just for backwards compatibility.
    try {
        TH2D* histogramForwardProcessed = (TH2D*)dataFile.Get("dataForwardHistogramProcessed");
        TH2D* histogramBackwardProcessed = (TH2D*)dataFile.Get("dataBackwardHistogramProcessed");
        TH2D* histogramBackToBackProcessed = (TH2D*)dataFile.Get("dataBackToBackHistogramProcessed");

        this->_processedForward = *histogramForwardProcessed;
        this->_processedBackward = *histogramBackwardProcessed;
        this->_processedBackToBack = *histogramBackToBackProcessed;


    } catch (...) {

        this->loadProcessed();

    }



    this->_pathToFile = pathToFile.c_str();
    this->_initialised = 1;
    dataFile.Close();

    
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

    this->_noCorrelationForward = histogramVector[3];
    this->_noCorrelationBackward = histogramVector[4];
    this->_noCorrelationBackToBack = histogramVector[5];

    

    
    this->_initialised = 1;
    this->loadProcessed(); //This needs to go after _initialised is set to 1 since loadProcessed() checks the initialisation status.
    storeHistogramInFile();


}


//Helper-method to a constructor to read in the data properly into the correct histograms
std::vector<TH2D> storeInHist::loadHistogram(std::string pathToFile, Short_t cutOption, 
                                            Double_t centralityMin, Double_t centralityMax,
                                            Double_t ptMin, Double_t ptMax,
                                            Double_t etaMin, Double_t etaMax,
                                            Int_t countsPhi, Int_t countsEta,
                                            Int_t start, Int_t stop) {

    //Number of old events to store                                          
    Int_t numberOlderEventsToSave = 5;
    
    //Creates the histograms which will be filled
    TH2D histogramForward("histogramForward", "Counts", countsPhi, 0, 2*TMath::Pi(), countsEta, etaMin, etaMax);
    TH2D histogramBackward("histogramBackWard", "Counts", countsPhi, 0, 2*TMath::Pi(), countsEta, etaMin, etaMax);
    TH2D histogramBackToBack("histogramBackToBack", "Counts", countsPhi, 0, 2*TMath::Pi(), countsEta, etaMin, etaMax);


    TH2D backgroundForward("histogramForwardBackground", "Counts", countsPhi, 0, 2*TMath::Pi(), countsEta, etaMin, etaMax);
    TH2D backgroundBackward("histogramBackWardBackground", "Counts", countsPhi, 0, 2*TMath::Pi(), countsEta, etaMin, etaMax);
    TH2D backgroundBackToBack("histogramBackToBackBackground", "Counts", countsPhi, 0, 2*TMath::Pi(), countsEta, etaMin, etaMax);
    
    
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

    Int_t approvedTPCTracksGlobal = 0;
    Int_t approvedForwardFMDTracksGlobal = 0;
    Int_t approvedTPCTracks = 0;
    Int_t approvedForwardFMDTracks = 0;

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


    std::vector<Double_t> forwardTracksPhi;
    std::vector<Double_t> backwardTracksPhi;
    std::vector<Double_t> forwardTracksEta;
    std::vector<Double_t> backwardTracksEta; 
    std::vector<Int_t> forwardTracksMult;
    std::vector<Int_t> backwardTracksMult;

    
    //Values to be stored in the histograms
    Double_t etaDiff; //For TPC-FMD correlations
    Double_t phiDiff;


    //Storing old events' data
    std::vector<std::vector<Double_t>> oldPhiTracksTPC;
    std::vector<std::vector<Double_t>> oldEtaTracksTPC;

    std::vector<std::vector<Double_t>> oldPhiTracksForwardFMD;
    std::vector<std::vector<Double_t>> oldEtaTracksForwardFMD;
    std::vector<std::vector<Int_t>> oldMultiplicityTracksForwardFMD;

    std::vector<std::vector<Double_t>> oldPhiTracksBackwardFMD;
    std::vector<std::vector<Double_t>> oldEtaTracksBackwardFMD;
    std::vector<std::vector<Int_t>> oldMultiplicityTracksBackwardFMD;




    //Loads in the first numberOlderEventsToSave events satisfying our conditions
    Int_t startCounter = 0;
    while (static_cast<int>(oldPhiTracksTPC.size()) < numberOlderEventsToSave) {
        std::cout << startCounter << std::endl;
        //Reads in the tracks for an event
        dataTree->GetEntry(startCounter);
        trackCountTPC = tpcTrack->GetEntries(); //Number of tracks in the event
        trackCountFMD = fmdTrack->GetEntries();


        //Checks if the centrality is within the desired region        
        centrality = event->fCent;
        if ((centrality < centralityMin) || (centrality > centralityMax)) {
            startCounter++;
            continue;
        }




        //Clears Data from a previous event
        tracksPhiTPC.clear();
        tracksEtaTPC.clear();

        forwardTracksEta.clear();
        backwardTracksEta.clear();
        forwardTracksPhi.clear();
        backwardTracksPhi.clear();
        forwardTracksMult.clear();
        backwardTracksMult.clear();


        approvedForwardFMDTracks = 0;
        //Loops through all tracks in the FMD
        for (Int_t fmdTrackNumber = 0; fmdTrackNumber < trackCountFMD; fmdTrackNumber++) {
            //Gets details about the track
            currentTrackFMD = static_cast<AliLWFMDTrack*>((*fmdTrack)[fmdTrackNumber]); 
            fmdMultiplicity = currentTrackFMD->fMult;
            phiValFMD = currentTrackFMD->fPhi;
            etaValFMD = currentTrackFMD->fEta;


            //Cutting away data where the resolution is low
            if ((etaValFMD < -3.1) || (etaValFMD > -2)) { //Maybe these numbers should be set at the top of the method for easier changing...
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

            
            approvedTPCTracks = 0;
            //Loops through all tracks in the TPC so TPC-FMD correlations may be calculated
            for (Int_t tpcTrackNumber = 0; tpcTrackNumber < trackCountTPC; tpcTrackNumber++) {
                currentTrackTPC = static_cast<AliLWTPCTrack*>((*tpcTrack)[tpcTrackNumber]);
                phiValTPC = currentTrackTPC->fPhi;
                etaValTPC = currentTrackTPC->fEta;
                pT = currentTrackTPC->fPt;
                cutFlag = currentTrackTPC->fTrFlag;
                
                phiDiff = phiValTPC - phiValFMD;
                etaDiff = etaValTPC - etaValFMD;

                //Makes sures all values are positive (we want all values within one period)
                if (phiDiff < 0) {phiDiff += 2*TMath::Pi();}


                //Cutting away data where with the wrong flag(s).
                if (cutOption == 3) {
                    //Intentionally does nothing
                } else {
                    if (cutFlag != cutOption) {
                        continue;
                    }
                }

                //Cuts away unwanted pT:s (pT = transverse momentum)
                if ((pT < ptMin) || (pT > ptMax)) {
                    continue;
                }


                //Cutting away data where the resolution is low
                if ((etaValTPC < -0.75) || (etaValTPC > 0.75)) { 
                    continue;
                } 

                approvedTPCTracks++;

                //Stores the read-in and approved values for later event mixing
                tracksPhiTPC.push_back(phiValTPC);
                tracksEtaTPC.push_back(etaValTPC);

                //Fills the correct histogram. The sign determines if it is the forwards or backwards FMD
                if (etaValFMD > 0) {
                    histogramForward.Fill(phiDiff, etaDiff, fmdMultiplicity);

                } else {
                    histogramBackward.Fill(phiDiff, etaDiff, fmdMultiplicity);

                }

            } //TPC-loop end



            //Makes sure that all events have not been cut away
            if ( (tracksPhiTPC.size() == 0) && ((forwardTracksPhi.size() == 0) || (backwardTracksPhi.size() == 0)) ) {
                startCounter++;
                continue;
            }

            approvedForwardFMDTracks += fmdMultiplicity;
        } //FMD-loop end


        approvedTPCTracksGlobal += approvedTPCTracks;
        approvedForwardFMDTracksGlobal += approvedForwardFMDTracks;

        calculateCorrelation(histogramBackToBack, forwardTracksPhi, forwardTracksEta,
                            backwardTracksPhi, backwardTracksEta,
                            forwardTracksMult, backwardTracksMult);




        oldPhiTracksTPC.push_back(tracksPhiTPC);
        oldEtaTracksTPC.push_back(tracksEtaTPC);

        oldPhiTracksForwardFMD.push_back(forwardTracksPhi);
        oldEtaTracksForwardFMD.push_back(forwardTracksEta);
        oldMultiplicityTracksForwardFMD.push_back(forwardTracksMult);
        
        oldPhiTracksBackwardFMD.push_back(backwardTracksPhi);
        oldEtaTracksBackwardFMD.push_back(backwardTracksEta);
        oldMultiplicityTracksBackwardFMD.push_back(backwardTracksMult);
        


        startCounter++;

    } //End of while-loop

    

    //Event-loop
    for (Int_t eventNumber = start+startCounter; eventNumber < stop; eventNumber++) {
        std::cout << eventNumber << std::endl;
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
        tracksPhiTPC.clear();
        tracksEtaTPC.clear();

        forwardTracksEta.clear();
        backwardTracksEta.clear();
        forwardTracksPhi.clear();
        backwardTracksPhi.clear();
        forwardTracksMult.clear();
        backwardTracksMult.clear();

        approvedForwardFMDTracks = 0;
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

            approvedTPCTracks = 0;
            //Loops through all tracks in the TPC so TPC-FMD correlations can be calculated
            for (Int_t tpcTrackNumber = 0; tpcTrackNumber < trackCountTPC; tpcTrackNumber++) {
                currentTrackTPC = static_cast<AliLWTPCTrack*>((*tpcTrack)[tpcTrackNumber]);
                phiValTPC = currentTrackTPC->fPhi;
                etaValTPC = currentTrackTPC->fEta;
                pT = currentTrackTPC->fPt;
                cutFlag = currentTrackTPC->fTrFlag;
                phiDiff = phiValTPC - phiValFMD;
                etaDiff = etaValTPC - etaValFMD;

                //Makes sures all values are positive (we want all values within one period)
                if (phiDiff < 0) {phiDiff += 2*TMath::Pi();}


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

                approvedTPCTracks++;
                //Stores the read-in and approved values for later event mixing
                tracksPhiTPC.push_back(phiValTPC);
                tracksEtaTPC.push_back(etaValTPC);

                //Fills the correct histogram. The sign determines if it is the forwards or backwards FMD
                if (etaValFMD > 0) {
                    histogramForward.Fill(phiDiff, etaDiff, fmdMultiplicity);

                } else {
                    histogramBackward.Fill(phiDiff, etaDiff, fmdMultiplicity);

                }

            } //TPC-loop end



            approvedForwardFMDTracks++;

        } //FMD-loop end

        approvedForwardFMDTracksGlobal += approvedForwardFMDTracks;
        approvedTPCTracksGlobal += approvedTPCTracks;

        //Calculates FMD-FMD correlations
        calculateCorrelation(histogramBackToBack, forwardTracksPhi, forwardTracksEta, 
                            backwardTracksPhi, backwardTracksEta, 
                            forwardTracksMult, backwardTracksMult);

        //Single event end




        //Event Mixing Starts
        //TPC-FMD correlations
        for (int oldEventNumber = 0; oldEventNumber < static_cast<int>(oldPhiTracksTPC.size()); oldEventNumber++) {
            calculateCorrelation(backgroundForward, oldPhiTracksTPC[oldEventNumber], oldEtaTracksTPC[oldEventNumber],
                            forwardTracksPhi, forwardTracksEta,
                            forwardTracksMult);

            calculateCorrelation(backgroundBackward, oldPhiTracksTPC[oldEventNumber], oldEtaTracksTPC[oldEventNumber],
                            backwardTracksPhi, backwardTracksEta,
                            backwardTracksMult);

        }



        //FMD-FMD for backwards tracks with old forward tracks


        for (int oldEventNumber = 0; oldEventNumber < static_cast<int>(oldPhiTracksForwardFMD.size()); oldEventNumber++) {       
            calculateCorrelation(backgroundBackToBack, oldPhiTracksForwardFMD[oldEventNumber], oldEtaTracksForwardFMD[oldEventNumber],
                                backwardTracksPhi, backwardTracksEta,
                                oldMultiplicityTracksForwardFMD[oldEventNumber], backwardTracksMult);

        }





        //Updates the stored old events with the current one and removes the oldest one
        std::cout << "tpc track number" << tracksPhiTPC.size() << std::endl;
        std::cout << "fmd track number" << forwardTracksPhi.size() << std::endl;
        oldPhiTracksTPC.push_back(tracksPhiTPC);
        oldPhiTracksTPC.erase(oldPhiTracksTPC.begin());
        oldEtaTracksTPC.push_back(tracksEtaTPC);
        oldEtaTracksTPC.erase(oldEtaTracksTPC.begin());

        oldPhiTracksForwardFMD.push_back(forwardTracksPhi);
        oldPhiTracksForwardFMD.erase(oldPhiTracksForwardFMD.begin());
        oldEtaTracksForwardFMD.push_back(forwardTracksEta);
        oldEtaTracksForwardFMD.erase(oldEtaTracksForwardFMD.begin());
        oldMultiplicityTracksForwardFMD.push_back(forwardTracksMult);
        oldMultiplicityTracksForwardFMD.erase(oldMultiplicityTracksForwardFMD.begin());

        oldPhiTracksBackwardFMD.push_back(backwardTracksPhi);
        oldPhiTracksBackwardFMD.erase(oldPhiTracksBackwardFMD.begin());
        oldEtaTracksBackwardFMD.push_back(backwardTracksEta);
        oldEtaTracksBackwardFMD.erase(oldEtaTracksBackwardFMD.begin());
        oldMultiplicityTracksBackwardFMD.push_back(backwardTracksMult);
        oldMultiplicityTracksBackwardFMD.erase(oldMultiplicityTracksBackwardFMD.begin());


      //Event Mixing End
    } //Event-loop end  



    //Returns the results
    std::vector<TH2D> returnVector;
    returnVector.push_back(histogramForward*(1.0/approvedTPCTracksGlobal));
    returnVector.push_back(histogramBackward*(1.0/approvedTPCTracksGlobal));
    returnVector.push_back(histogramBackToBack*(1.0/approvedForwardFMDTracksGlobal));
    returnVector.push_back(backgroundForward);
    returnVector.push_back(backgroundBackward);
    returnVector.push_back(backgroundBackToBack);

    dataFile.Close();
    delete event;
    delete tpcTrack;
    delete fmdTrack;


    return returnVector;


}


