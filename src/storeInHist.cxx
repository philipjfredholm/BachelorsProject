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


//Methods


void storeInHist::calculateCorrelation(TH2D& myHistogram, const std::vector<Double_t>& phi1, const std::vector<Double_t>& eta1,
                                  const std::vector<Double_t>& phi2, const std::vector<Double_t>& eta2,
                                  const std::vector<Int_t>& mult1, const std::vector<Int_t>& mult2) {


    //This is just for my own debugging, in a public member function we of course throw errors
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
            etaDiff = eta1[detector1Track] - eta2[detector2Track];
            myHistogram.Fill(phiDiff, etaDiff, mult2[detector2Track]);
        }

    }


}


void storeInHist::addHistogram(storeInHist secondHistogram) {
    if (this->_initialised == 0) {
        this->_storedForwardHistogram = secondHistogram.getForwardHistogram();
        this->_storedBackwardHistogram = secondHistogram.getBackwardHistogram();
        this->_storedBackToBackHistogram = secondHistogram.getBackToBackHistogram();
        this->_noCorrelationForward = secondHistogram.getForwardBackground();
        this->_noCorrelationBackward = secondHistogram.getBackwardBackground();
        this->_noCorrelationBackToBack = secondHistogram.getBackToBackBackground();

        this->_pathToFile = secondHistogram.getFilePath();
        this->_initialised = 1;

    } else { 
        TH2D secondForwardHistogramCopy = secondHistogram.getForwardHistogram(); //Making a copy is necessary because g++ complains about references to "r-value" otherwise
        TH2D secondBackwardHistogramCopy = secondHistogram.getBackwardHistogram();
        TH2D secondBackToBackHistogramCopy = secondHistogram.getBackToBackHistogram();
        TH2D secondForwardBackgroundCopy = secondHistogram.getForwardBackground();
        TH2D secondBackwardBackgroundCopy = secondHistogram.getBackwardBackground();
        TH2D secondBackToBackBackgroundCopy = secondHistogram.getBackToBackBackground();
        this->_storedForwardHistogram.Add(&secondForwardHistogramCopy);
        this->_storedBackwardHistogram.Add(&secondBackwardHistogramCopy);
        this->_storedBackToBackHistogram.Add(&secondBackToBackHistogramCopy);
        this->_noCorrelationForward.Add(&secondForwardBackgroundCopy);
        this->_noCorrelationBackward.Add(&secondBackwardBackgroundCopy);
        this->_noCorrelationBackToBack.Add(&secondBackToBackBackgroundCopy);

    }

    
}


void storeInHist::storeHistogramInFile() {
    //Sets the new filename
    const std::string filename = _pathToFile.substr(_pathToFile.find("/")+1, _pathToFile.length());
    std::string storageLocation = "processedData/"+filename.substr(0, filename.find_last_of(".")) +"Processed"+ ".root"; 

    //Stores the histogram
    TH2D* histogramForwardPointer = &(this->_storedForwardHistogram);
    TH2D* histogramBackwardPointer = &(this->_storedBackwardHistogram);
    TH2D* histogramBackToBackPointer = &(this->_storedBackToBackHistogram);

    TH2D* backgroundForwardPointer = &(this->_noCorrelationForward);
    TH2D* backgroundBackwardPointer = &(this->_noCorrelationBackward);
    TH2D* backgroundBackToBackPointer = &(this->_noCorrelationBackToBack);


    TFile writeData(storageLocation.c_str(), "RECREATE"); 
    writeData.WriteObject(histogramForwardPointer, "processedDataForwardHistogram");
    writeData.WriteObject(histogramBackwardPointer, "processedDataBackwardHistogram");
    writeData.WriteObject(histogramBackToBackPointer, "processedDataBackToBackHistogram");

    writeData.WriteObject(backgroundForwardPointer, "processedDataForwardHistogramBackground");
    writeData.WriteObject(backgroundBackwardPointer, "processedDataBackwardHistogramBackground");
    writeData.WriteObject(backgroundBackToBackPointer, "processedDataBackToBackHistogramBackground");
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
    
    TH2D* histogramForwardBackground = (TH2D*)dataFile.Get("processedDataForwardHistogramBackground");
    TH2D* histogramBackwardBackground = (TH2D*)dataFile.Get("processedDataBackwardHistogramBackground");
    TH2D* histogramBackToBackBackground = (TH2D*)dataFile.Get("processedDataBackToBackHistogramBackground");

    this->_storedForwardHistogram = *histogramForward;
    this->_storedBackwardHistogram = *histogramBackward;
    this->_storedBackToBackHistogram = *histogramBackToBack;

    this->_noCorrelationForward = *histogramForwardBackground;
    this->_noCorrelationBackward = *histogramBackwardBackground;
    this->_noCorrelationBackToBack = *histogramBackToBackBackground;

    this->_pathToFile = pathToFile.c_str();
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

    this->_noCorrelationForward = histogramVector[3];
    this->_noCorrelationBackward = histogramVector[4];
    this->_noCorrelationBackToBack = histogramVector[5];

    


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




    //Loads in the first numberOlderEventsToSave events satisfying our conditions
    Int_t startCounter = 0;
    while (static_cast<int>(oldPhiTracksTPC.size()) < numberOlderEventsToSave) {
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

        //Single event end




        //Event Mixing Start
        //TPC-FMD correlations



        
        for (int oldEventNumber = 0; oldEventNumber < static_cast<int>(oldPhiTracksTPC.size()); oldEventNumber++) {
            calculateCorrelation(backgroundForward, oldPhiTracksTPC[oldEventNumber], oldEtaTracksTPC[oldEventNumber],
                            forwardTracksPhi, forwardTracksEta,
                            forwardTracksMult);

            calculateCorrelation(backgroundBackward, oldPhiTracksTPC[oldEventNumber], oldEtaTracksTPC[oldEventNumber],
                            backwardTracksPhi, backwardTracksEta,
                            backwardTracksMult);
           /* for (int oldTrackNumberTPC = 0; oldTrackNumberTPC < static_cast<int>(oldPhiTracksTPC[oldEventNumber].size()); oldTrackNumberTPC++) {
                phiValTPC = oldPhiTracksTPC[oldEventNumber][oldTrackNumberTPC];
                etaValTPC = oldEtaTracksTPC[oldEventNumber][oldTrackNumberTPC];

                //Forward
                for (int trackNumberFMD = 0; trackNumberFMD < static_cast<int>(forwardTracksPhi.size()); trackNumberFMD++) {
                    phiValFMD = forwardTracksPhi[trackNumberFMD];
                    etaValFMD = forwardTracksEta[trackNumberFMD];
                    fmdMultiplicity = forwardTracksMult[trackNumberFMD];

                    phiDiff = phiValTPC - phiValFMD;
                    etaDiff = etaValTPC - etaValFMD;

                    backgroundForward.Fill(phiDiff, etaDiff, fmdMultiplicity);

                }

                //Backward
                for (int trackNumberFMD = 0; trackNumberFMD < static_cast<int>(backwardTracksPhi.size()); trackNumberFMD++) {
                    phiValFMD = backwardTracksPhi[trackNumberFMD];
                    etaValFMD = backwardTracksEta[trackNumberFMD];
                    fmdMultiplicity = backwardTracksMult[trackNumberFMD];

                    phiDiff = phiValTPC - phiValFMD;
                    etaDiff = etaValTPC - etaValFMD;

                    backgroundBackward.Fill(phiDiff, etaDiff, fmdMultiplicity);

                }


            } */
        }



        //FMD-FMD for backwards tracks with old forward tracks


        for (int oldEventNumber = 0; oldEventNumber < static_cast<int>(oldPhiTracksForwardFMD.size()); oldEventNumber++) {       
            calculateCorrelation(backgroundBackToBack, oldPhiTracksForwardFMD[oldEventNumber], oldEtaTracksForwardFMD[oldEventNumber],
                                backwardTracksPhi, backwardTracksEta,
                                oldMultiplicityTracksForwardFMD[oldEventNumber], backwardTracksMult);
            /*for (int oldTrackNumberForwardFMD = 0; oldTrackNumberForwardFMD < static_cast<int>(oldPhiTracksForwardFMD[oldEventNumber].size()); oldTrackNumberForwardFMD++) {
                phiForwardFMD = oldPhiTracksForwardFMD[oldEventNumber][oldTrackNumberForwardFMD];
                etaForwardFMD = oldEtaTracksForwardFMD[oldEventNumber][oldTrackNumberForwardFMD];
                forwardMult = oldMultiplicityTracksForwardFMD[oldEventNumber][oldTrackNumberForwardFMD];

                for (int trackNumberBackwardFMD = 0; trackNumberBackwardFMD < static_cast<int>(backwardTracksPhi.size()); trackNumberBackwardFMD++) {
                    phiBackwardFMD = backwardTracksPhi[trackNumberBackwardFMD];
                    etaBackwardFMD = backwardTracksEta[trackNumberBackwardFMD];
                    backwardMult = backwardTracksMult[trackNumberBackwardFMD];

                    phiDiffBackToBack = phiForwardFMD - phiBackwardFMD;
                    etaDiffBackToBack = etaForwardFMD - etaBackwardFMD;


                    backgroundBackToBack.Fill(phiDiffBackToBack, etaDiffBackToBack, forwardMult*backwardMult);

                }
            }*/
        }





        //Updates the stored old events with the current one and removes the oldest one
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
    returnVector.push_back(histogramForward);
    returnVector.push_back(histogramBackward);
    returnVector.push_back(histogramBackToBack);
    returnVector.push_back(backgroundForward);
    returnVector.push_back(backgroundBackward);
    returnVector.push_back(backgroundBackToBack);

    dataFile.Close();
    delete event;
    delete tpcTrack;
    delete fmdTrack;


    return returnVector;


}



/*

std::vector<TH2D> storeInHist::loadBackground(std::string pathToFile, Short_t cutOption, 
                                            Double_t centralityMin, Double_t centralityMax,
                                            Double_t ptMin, Double_t ptMax,
                                            Double_t etaMin, Double_t etaMax,
                                            Int_t countsPhi, Int_t countsEta,
                                            Int_t start, Int_t stop) {
    //How many old events to save
    Int_t numberOlderEventsToSave = 5;

    
    //Creates the histograms which will be filled
    TH2D histogramForward("histogramForwardBackground", "Counts", countsPhi, 0, 2*TMath::Pi(), countsEta, etaMin, etaMax);
    TH2D histogramBackward("histogramBackWardBackground", "Counts", countsPhi, 0, 2*TMath::Pi(), countsEta, etaMin, etaMax);
    TH2D histogramBackToBack("histogramBackToBackBackground", "Counts", countsPhi, 0, 2*TMath::Pi(), countsEta, etaMin, etaMax);
    
    
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
    Double_t phiDiff; //For TPC-FMD correlations
    Double_t etaDiff;
    
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




    std::cout << "1" << std::endl;

    //Loads in the first 5 events satisfying our conditions
    Int_t startCounter = 0;
    while (static_cast<int>(oldPhiTracksTPC.size()) < numberOlderEventsToSave) {
        //Reads in information about the event
        dataTree->GetEntry(startCounter);
        trackCountTPC = tpcTrack->GetEntries(); 
        trackCountFMD = fmdTrack->GetEntries();


        //Skips an event if it has a non-desired centrality
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


        //Loops through all tracks in the FMD
        for (Int_t fmdTrackNumber = 0; fmdTrackNumber < trackCountFMD; fmdTrackNumber++) {
            //Gets details about the track
            currentTrackFMD = static_cast<AliLWFMDTrack*>((*fmdTrack)[fmdTrackNumber]);    
            phiValFMD = currentTrackFMD->fPhi;
            etaValFMD = currentTrackFMD->fEta;
            fmdMultiplicity = currentTrackFMD->fMult;


            //Skips the track if it is in an unwanted region with low resolution
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


                //Skips the track if it has the wrong flag
                if (cutOption == 3) {
                    //Intentionally does nothing
                } else {
                    if (cutFlag != cutOption) {
                        continue;
                    }
                }

                //Skips the track if the pT is not in the wanted region
                if ((pT < ptMin) || (pT > ptMax)) {
                    continue;
                }


                //Skips data if it is from a low resolution region
                if ((etaValTPC < -0.75) || (etaValTPC > 0.75)) { 
                    continue;
                } 
                
                //Stores the read-in and approved values
                tracksPhiTPC.push_back(phiValTPC);
                tracksEtaTPC.push_back(etaValTPC);

            } //TPC-loop end
        } //FMD-loop end


        //Stores all approved tracks from this event
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


    std::cout << "2" << std::endl;



    //Now that the old events' tracks have been saved, they may be correlated to other events
    for (Int_t eventNumber = start+startCounter; eventNumber < stop; eventNumber++) {
        //Reads in information about the event
        dataTree->GetEntry(startCounter);
        trackCountTPC = tpcTrack->GetEntries(); 
        trackCountFMD = fmdTrack->GetEntries();


        //Skips the event if the centrality is not in the desired range
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


        //Loops through all tracks in the FMD
        for (Int_t fmdTrackNumber = 0; fmdTrackNumber < trackCountFMD; fmdTrackNumber++) {
            //Gets details about the track
            currentTrackFMD = static_cast<AliLWFMDTrack*>((*fmdTrack)[fmdTrackNumber]); 
            phiValFMD = currentTrackFMD->fPhi;
            etaValFMD = currentTrackFMD->fEta;
            fmdMultiplicity = currentTrackFMD->fMult;


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



            //Loops through all tracks in the TPC
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





        //TPC-FMD correlations
        for (int oldEventNumber = 0; oldEventNumber < static_cast<int>(oldPhiTracksTPC.size()); oldEventNumber++) {
            for (int oldTrackNumberTPC = 0; oldTrackNumberTPC < static_cast<int>(oldPhiTracksTPC[oldEventNumber].size()); oldTrackNumberTPC++) {
                phiValTPC = oldPhiTracksTPC[oldEventNumber][oldTrackNumberTPC];
                etaValTPC = oldEtaTracksTPC[oldEventNumber][oldTrackNumberTPC];

                //Forward
                for (int trackNumberFMD = 0; trackNumberFMD < static_cast<int>(forwardTracksPhi.size()); trackNumberFMD++) {
                    phiValFMD = forwardTracksPhi[trackNumberFMD];
                    etaValFMD = forwardTracksEta[trackNumberFMD];
                    fmdMultiplicity = forwardTracksMult[trackNumberFMD];

                    phiDiff = phiValTPC - phiValFMD;
                    etaDiff = etaValTPC - etaValFMD;

                    histogramForward.Fill(phiDiff, etaDiff, fmdMultiplicity);

                }

                //Backward
                for (int trackNumberFMD = 0; trackNumberFMD < static_cast<int>(backwardTracksPhi.size()); trackNumberFMD++) {
                    phiValFMD = backwardTracksPhi[trackNumberFMD];
                    etaValFMD = backwardTracksEta[trackNumberFMD];
                    fmdMultiplicity = backwardTracksMult[trackNumberFMD];

                    phiDiff = phiValTPC - phiValFMD;
                    etaDiff = etaValTPC - etaValFMD;

                    histogramBackward.Fill(phiDiff, etaDiff, fmdMultiplicity);

                }


            }
        }


        //FMD-FMD for backwards tracks with old forward tracks
        for (int oldEventNumber = 0; oldEventNumber < static_cast<int>(oldPhiTracksForwardFMD.size()); oldEventNumber++) {
            for (int oldTrackNumberForwardFMD = 0; oldTrackNumberForwardFMD < static_cast<int>(oldPhiTracksForwardFMD[oldEventNumber].size()); oldTrackNumberForwardFMD++) {
                phiForwardFMD = oldPhiTracksForwardFMD[oldEventNumber][oldTrackNumberForwardFMD];
                etaForwardFMD = oldEtaTracksForwardFMD[oldEventNumber][oldTrackNumberForwardFMD];
                forwardMult = oldMultiplicityTracksForwardFMD[oldEventNumber][oldTrackNumberForwardFMD];

                for (int trackNumberBackwardFMD = 0; trackNumberBackwardFMD < static_cast<int>(backwardTracksPhi.size()); trackNumberBackwardFMD++) {
                    phiBackwardFMD = backwardTracksPhi[trackNumberBackwardFMD];
                    etaBackwardFMD = backwardTracksEta[trackNumberBackwardFMD];
                    backwardMult = backwardTracksMult[trackNumberBackwardFMD];

                    phiDiffBackToBack = phiForwardFMD - phiBackwardFMD;
                    etaDiffBackToBack = etaForwardFMD - etaBackwardFMD;


                    histogramBackToBack.Fill(phiDiffBackToBack, etaDiffBackToBack, forwardMult*backwardMult);

                }
            }
        }

        //FMD-FMD for forward tracks with old backward tracks
        
        for (int oldEventNumber = 0; oldEventNumber < static_cast<int>(oldPhiTracksBackwardFMD.size()); oldEventNumber++) {
            for (int oldTrackNumberBackwardFMD = 0; oldTrackNumberBackwardFMD < static_cast<int>(oldPhiTracksBackwardFMD[oldEventNumber].size()); oldTrackNumberBackwardFMD++) {
                phiBackwardFMD = oldPhiTracksBackwardFMD[oldEventNumber][oldTrackNumberBackwardFMD];
                etaBackwardFMD = oldEtaTracksBackwardFMD[oldEventNumber][oldTrackNumberBackwardFMD];
                backwardMult = oldMultiplicityTracksBackwardFMD[oldEventNumber][oldTrackNumberBackwardFMD];

                for (int trackNumberForwardFMD = 0; trackNumberForwardFMD < static_cast<int>(forwardTracksPhi.size()); trackNumberForwardFMD++) {
                    phiForwardFMD = forwardTracksPhi[trackNumberForwardFMD];
                    etaForwardFMD = forwardTracksEta[trackNumberForwardFMD];
                    forwardMult = forwardTracksMult[trackNumberForwardFMD];

                    phiDiffBackToBack = phiForwardFMD - phiBackwardFMD;
                    etaDiffBackToBack = etaForwardFMD - etaBackwardFMD;


                    histogramBackToBack.Fill(phiDiffBackToBack, etaDiffBackToBack, forwardMult*backwardMult);

                }
            }
        }
        




        //Updates the stored old events with the current one and removes the oldest one
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

    } //End of looping over all events

    std::cout << "3" << std::endl;
    //Cleaning up
    dataFile.Close();
    delete event;
    delete tpcTrack;
    delete fmdTrack;
    
    //Returns the results
    std::vector<TH2D> returnVector;
    returnVector.push_back(histogramForward);
    returnVector.push_back(histogramBackward);
    returnVector.push_back(histogramBackToBack);

    return returnVector;

}

*/