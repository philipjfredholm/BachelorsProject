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
storeInHist::storeInHist(std::string pathToFile, Int_t binsPhi, Int_t binsEta, Double_t etaMin, Double_t etaMax, Short_t cutOption) : _pathToFile{pathToFile} {
    //Gets the number of entries in the tree
    TFile dataFile(pathToFile.c_str(), "dataFile", "READ");
    TTree* dataTree = (TTree*)dataFile.Get("LWTree");
    Int_t dataCount = dataTree->GetEntries();
    dataFile.Close();
    

    //loadHistogram creates the three histograms that are wanted. Separate method as it is very lengthy.
    std::vector<TH2D> histogramVector = loadHistogram(pathToFile, 0, dataCount, binsPhi, binsEta, etaMin, etaMax, cutOption);
    this->_storedForwardHistogram = histogramVector[0];
    this->_storedBackwardHistogram = histogramVector[1];
    this->_storedBackToBackHistogram = histogramVector[2];
    this->_initialised = 1;
    storeHistogramInFile();



}


//Helper-method to a constructor to read in the data properly into the correct histograms
std::vector<TH2D> storeInHist::loadHistogram(std::string pathToFile, Int_t start, Int_t stop, 
                                Int_t countsX, Int_t countsY, Double_t etaMin, Double_t etaMax, Short_t cutOption) {

    //Opens the data
    TFile dataFile(pathToFile.c_str(), "dataFile", "READ");
    TTree* dataTree = (TTree*)dataFile.Get("LWTree");
    

    //Creates variables to write the read-in variables to.
    //Not great with raw pointers, but ROOT requires pointers and refuses smart pointers
    TClonesArray* tpcTrack = new TClonesArray("AliLWTPCTrack");
    TClonesArray* fmdTrack = new TClonesArray("AliLWFMDTrack");
    dataTree->SetBranchAddress("TPCTracks", &tpcTrack);
    dataTree->SetBranchAddress("FMDTracks", &fmdTrack);


    //Variables needed in the for-loops for TPC-FMD correlatons
    Short_t cutFlag;
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

    //Variables needed for FMD-FMD correlations
    Double_t phiForwardFMD;
    Double_t phiBackwardFMD;
    Double_t etaForwardFMD;
    Double_t etaBackwardFMD;
    Int_t forwardMult;
    Int_t backwardMult;

    Double_t phiDiffBackToBack;
    Double_t etaDiffBackToBack;


    std::vector<Double_t> forwardTracksEta;
    std::vector<Double_t> backwardTracksEta;
    std::vector<Double_t> forwardTracksPhi;
    std::vector<Double_t> backwardTracksPhi;
    std::vector<Int_t> forwardTracksMult;
    std::vector<Int_t> backwardTracksMult;


    //Creates the histograms which will be filled
    TH2D histogramForward("histogramForward", "Counts", countsX, 0, 2*TMath::Pi(), countsY, etaMin, etaMax);
    TH2D histogramBackward("histogramBackWard", "Counts", countsX, 0, 2*TMath::Pi(), countsY, etaMin, etaMax);
    TH2D histogramBackToBack("histogramBackToBack", "Counts", countsX, 0, 2*TMath::Pi(), countsY, etaMin, etaMax);

    /*
    Explanation of the nested for-loops:
    The first loop loops through each event and reads out a TClonesArray (basically an array) with the
    tracks in both the FMD:s and the TPC. The first nested loop then loops through each FMD event. Here,
    it does two things. First, it calculates the eta and phi differences between the TPC and FMD
    and then places it into a histogram for correlations with either the forward or backwards FMD with 
    the tpc. It also, for each event, reads out all the phi and eta values (as well as the multiplicities).
    
    In the second for loop in the loop for the events, these values are used to take FMD-FMD differences
    between the forwards and backwards FMD. This is in turn a nested loop since all tracks in the forward
    FMD need to be correlated with the backwards FMD.

    */

   //Event-loop
    for (Int_t eventNumber = start; eventNumber < stop; eventNumber++) {
        //Reads in the tracks for an event
        dataTree->GetEntry(eventNumber);
        trackCountTPC = tpcTrack->GetEntries(); 
        trackCountFMD = fmdTrack->GetEntries();

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
            fmdMult = currentTrackFMD->fMult;
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
                forwardTracksMult.push_back(fmdMult);
                
            } else { 
                backwardTracksEta.push_back(etaValFMD);
                backwardTracksPhi.push_back(phiValFMD);
                backwardTracksMult.push_back(fmdMult);

            }

            //Loops through all tracks in the TPC so TPC-FMD correlations can be calculated
            for (Int_t tpcTrackNumber = 0; tpcTrackNumber < trackCountTPC; tpcTrackNumber++) {
                currentTrackTPC = static_cast<AliLWTPCTrack*>((*tpcTrack)[tpcTrackNumber]);
                phiValTPC = currentTrackTPC->fPhi;
                etaValTPC = currentTrackTPC->fEta;
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


                //Cutting away data where the resolution is low
                if ((etaValTPC < -0.75) || (etaValTPC > 0.75)) { 
                    continue;
                } 


                //Makes sures all values are positive (we want all values within one period)
                if (phiDiff < 0) {phiDiff += 2*TMath::Pi();}


                //Fills the correct histogram. The sign determines if it is the forwards or backwards FMD
                if (etaValFMD > 0) {
                    histogramForward.Fill(phiDiff, etaDiff, fmdMult);

                } else {
                    histogramBackward.Fill(phiDiff, etaDiff, fmdMult);

                }


            }


        }




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



        
    }


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



std::vector<TH2D> storeInHist::loadBackgroundHistogram(std::string pathToFile, Int_t start, Int_t stop,
                                                Int_t countsX, Int_t countsY, Double_t etaMin, Double_t etaMax, Short_t cutOption) {

    //Opens the data
    TFile dataFile(pathToFile.c_str(), "dataFile", "READ");
    TTree* dataTree = (TTree*)dataFile.Get("LWTree");
    
    //Creates variables to write the read-in variables to.
    //Not great with raw pointers, but ROOT requires pointers and refuses smart pointers
    TClonesArray* event = new TClonesArray("AliLWEvent");
    TClonesArray* tpcTrack = new TClonesArray("AliLWTPCTrack");
    TClonesArray* fmdTrack = new TClonesArray("AliLWFMDTrack");
    dataTree->SetBranchAddress("TPCTracks", &tpcTrack);
    dataTree->SetBranchAddress("FMDTracks", &fmdTrack);
    dataTree->SetBranchAddress("Event", &event);


    //Variables needed in the for-loops for TPC-FMD correlatons
    Short_t cutFlag;
    Int_t trackCountTPC;
    Int_t trackCountFMD;
    Double_t phiValTPC;
    Double_t phiValFMD;
    Double_t etaValTPC;
    Double_t etaValFMD;
    Double_t ptValue;
    Double_t etaDiff;
    Int_t fmdMult;
    Double_t phiDiff;
    AliLWTPCTrack* currentTrackTPC;
    AliLWFMDTrack* currentTrackFMD;

    //Variables needed for FMD-FMD correlations
    Double_t phiForwardFMD;
    Double_t phiBackwardFMD;
    Double_t etaForwardFMD;
    Double_t etaBackwardFMD;
    Int_t forwardMult;
    Int_t backwardMult;

    Double_t phiDiffBackToBack;
    Double_t etaDiffBackToBack;


    std::vector<Double_t> forwardTracksEta;
    std::vector<Double_t> backwardTracksEta;
    std::vector<Double_t> forwardTracksPhi;
    std::vector<Double_t> backwardTracksPhi;
    std::vector<Int_t> forwardTracksMult;
    std::vector<Int_t> backwardTracksMult;


    //Creates the histograms which will be filled
    TH2D histogramForward("histogramForward", "Counts", countsX, 0, 2*TMath::Pi(), countsY, etaMin, etaMax);
    TH2D histogramBackward("histogramBackWard", "Counts", countsX, 0, 2*TMath::Pi(), countsY, etaMin, etaMax);
    TH2D histogramBackToBack("histogramBackToBack", "Counts", countsX, 0, 2*TMath::Pi(), countsY, etaMin, etaMax);

    /*
    Explanation of the nested for-loops:
    The first loop loops through each event and reads out a TClonesArray (basically an array) with the
    tracks in both the FMD:s and the TPC. The first nested loop then loops through each FMD event. Here,
    it does two things. First, it calculates the eta and phi differences between the TPC and FMD
    and then places it into a histogram for correlations with either the forward or backwards FMD with 
    the tpc. It also, for each event, reads out all the phi and eta values (as well as the multiplicities).
    
    In the second for loop in the loop for the events, these values are used to take FMD-FMD differences
    between the forwards and backwards FMD. This is in turn a nested loop since all tracks in the forward
    FMD need to be correlated with the backwards FMD.

    */

   //Event-loop
    for (Int_t eventNumber = start; eventNumber < stop; eventNumber++) {
        //Reads in the tracks for an event
        dataTree->GetEntry(eventNumber);
        trackCountTPC = tpcTrack->GetEntries(); 
        trackCountFMD = fmdTrack->GetEntries();

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
            fmdMult = currentTrackFMD->fMult;
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
                forwardTracksMult.push_back(fmdMult);
                
            } else { 
                backwardTracksEta.push_back(etaValFMD);
                backwardTracksPhi.push_back(phiValFMD);
                backwardTracksMult.push_back(fmdMult);

            }

            //Loops through all tracks in the TPC so TPC-FMD correlations can be calculated
            for (Int_t tpcTrackNumber = 0; tpcTrackNumber < trackCountTPC; tpcTrackNumber++) {
                currentTrackTPC = static_cast<AliLWTPCTrack*>((*tpcTrack)[tpcTrackNumber]);
                phiValTPC = currentTrackTPC->fPhi;
                etaValTPC = currentTrackTPC->fEta;
                cutFlag = currentTrackTPC->fTrFlag;
                ptValue = currentTrackTPC->fPt;
                (void)ptValue;
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


                //Cutting away data where the resolution is low
                if ((etaValTPC < -0.75) || (etaValTPC > 0.75)) { 
                    continue;
                } 


                //Makes sures all values are positive (we want all values within one period)
                if (phiDiff < 0) {phiDiff += 2*TMath::Pi();}


                //Fills the correct histogram. The sign determines if it is the forwards or backwards FMD
                if (etaValFMD > 0) {
                    histogramForward.Fill(phiDiff, etaDiff, fmdMult);

                } else {
                    histogramBackward.Fill(phiDiff, etaDiff, fmdMult);

                }


            }


        }




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



        
    }


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




