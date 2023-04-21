#include "../include/storeInHist.h"

ClassImp(storeInHist); //For ROOT compatibility


//Getters and Setters
void storeInHist::setStorageName(std::string location) {
    this->_pathToFile = location;
}

std::string storeInHist::getFilePath() {
    return this->_pathToFile;
}

const std::vector<std::vector<TH2D>> storeInHist::getForwardHistograms() {
    return this->_storedForwardList;
}

const std::vector<std::vector<TH2D>> storeInHist::getBackwardHistograms() {
    return this->_storedBackwardList;
}

const std::vector<std::vector<TH2D>> storeInHist::getBackToBackHistograms() {
    return this->_storedBackToBackList;
}

const std::vector<std::vector<TH2D>> storeInHist::getForwardBackgrounds() {
    return this->_noCorrelationForwardList;
}

const std::vector<std::vector<TH2D>> storeInHist::getBackwardBackgrounds() {
    return this->_noCorrelationBackwardList;
}

const std::vector<std::vector<TH2D>> storeInHist::getBackToBackBackgrounds() {
    return this->_noCorrelationBackToBackList;
}

const std::vector<std::vector<TH2D>> storeInHist::getForwardProcessed() {
    return this->_processedForwardList;
}

const std::vector<std::vector<TH2D>> storeInHist::getBackwardProcessed() {
    return this->_processedBackwardList;
}

const std::vector<std::vector<TH2D>> storeInHist::getBackToBackProcessed() {
    return this->_processedBackToBackList;
}

const std::vector<std::vector<int>> storeInHist::getEventNumberList() {
    return this->_eventNumberList;
}

const std::vector<std::vector<int>> storeInHist::getEventNumberListFMD() {
    return this->_eventNumberListFMD;
}



//Methods
    //Various methods to save space later when many correlations have to be calculated
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
            
            phiDiff = phi1[detector1Track] - phi2[detector2Track] - 0.0001; //This is for the FMD-FMD correlation so -0.001 is necessary to avoid binning problems
            if (phiDiff < 0) {
                phiDiff += 2*TMath::Pi();
            }

            etaDiff = eta1[detector1Track] - eta2[detector2Track] - 0.0001;
            multiplicity = mult1[detector1Track] * mult2[detector2Track];
            myHistogram.Fill(phiDiff, etaDiff, multiplicity);


        }

    }


}
        




//FMD-FMD
void storeInHist::calculateSingleCorrelation(TH2D& myHistogram, const Double_t& phi1, const Double_t& eta1,
                                  const std::vector<Double_t>& phi2, const std::vector<Double_t>& eta2,
                                  const Int_t& mult1, const std::vector<Int_t>& mult2) {


    if (phi2.size() != eta2.size()) {
        std::cout << "Unequal Sizes detect" << std::endl;


    }
    

    Double_t phiDiff;
    Double_t etaDiff;
    Double_t multiplicity;


    for (int detectorTrack = 0; detectorTrack < static_cast<int>(phi2.size()); detectorTrack++) {
        phiDiff = phi1 - phi2[detectorTrack] -0.0001; //-0.0001 Necessary to avoid binning problems in FMD-FMD correlations
        
        if (phiDiff < 0) {
            phiDiff += 2*TMath::Pi();
        }

        etaDiff = eta1 - eta2[detectorTrack] - 0.0001; //-0.0001 Necessary to avoid binning problems in FMD-FMD correlations
        multiplicity = mult1 * mult2[detectorTrack];
        myHistogram.Fill(phiDiff, etaDiff, multiplicity);


    }



}


//FMD-TPC
void storeInHist::calculateSingleCorrelation(TH2D& myHistogram, const Double_t& phi1, const Double_t& eta1,
                                  const std::vector<Double_t>& phi2, const std::vector<Double_t>& eta2,
                                  const Int_t& mult1) {
    if (phi2.size() != eta2.size()) {
        std::cout << "Unequal Sizes detect" << std::endl;


    }
    

    Double_t phiDiff;
    Double_t etaDiff;
    Double_t multiplicity;


    for (int detectorTrack = 0; detectorTrack < static_cast<int>(phi2.size()); detectorTrack++) {
        
        phiDiff = phi1 - phi2[detectorTrack];
        if (phiDiff < 0) {
            phiDiff += 2*TMath::Pi();
        }

        etaDiff = eta1 - eta2[detectorTrack];
        multiplicity = mult1;
        myHistogram.Fill(phiDiff, etaDiff, multiplicity);


    }


}



    //Adds class instances of storeInHist, in hindsight maybe I should had use operator+ instead
void storeInHist::addHistograms(storeInHist secondHistogram) {
    if (this->_initialised == 0) { //For compatibility with the dummy default constructor that is implmented later
        //Raw
        this->_storedForwardList = secondHistogram.getForwardHistograms();
        this->_storedBackwardList = secondHistogram.getBackwardHistograms();
        this->_storedBackToBackList = secondHistogram.getBackToBackHistograms();


        //Background
        this->_noCorrelationForwardList = secondHistogram.getForwardBackgrounds();
        this->_noCorrelationBackwardList = secondHistogram.getBackwardBackgrounds();
        this->_noCorrelationBackToBackList = secondHistogram.getBackToBackBackgrounds();


        //Normalised
        this->_processedForwardList = secondHistogram.getForwardProcessed();
        this->_processedBackwardList = secondHistogram.getBackwardProcessed();
        this->_processedBackToBackList = secondHistogram.getBackToBackProcessed(); 
        this->_eventNumberList = secondHistogram.getEventNumberList();
        this->_eventNumberListFMD = secondHistogram.getEventNumberListFMD();

        //Initialisation
        this->_pathToFile = secondHistogram.getFilePath();
        this->_initialised = 1;



    } else { 
        
        /*
        The combination of ROOT and C++ is quite annoying here. ROOT only accepts pointers/addresses for the native
        .Add() method instead of something nice like passing by reference. Since the compiler complains about
        'reference to r-value' when I try to directly take the address in shorter syntax, I have to make completely
         new objects just to use the native .Add() method for the TH2D class.
        */

        std::vector<std::vector<int>> secondEventNumberList = secondHistogram.getEventNumberList();
        std::vector<std::vector<int>> secondEventNumberListFMD = secondHistogram.getEventNumberListFMD();
     

        std::vector<std::vector<TH2D>> secondForwardHistogramCopy = secondHistogram.getForwardHistograms(); 
        std::vector<std::vector<TH2D>> secondBackwardHistogramCopy = secondHistogram.getBackwardHistograms();
        std::vector<std::vector<TH2D>> secondBackToBackHistogramCopy = secondHistogram.getBackToBackHistograms();

        std::vector<std::vector<TH2D>> secondForwardBackgroundCopy = secondHistogram.getForwardBackgrounds();
        std::vector<std::vector<TH2D>> secondBackwardBackgroundCopy = secondHistogram.getBackwardBackgrounds();
        std::vector<std::vector<TH2D>> secondBackToBackBackgroundCopy = secondHistogram.getBackToBackBackgrounds();  
        
        std::vector<std::vector<TH2D>> secondForwardProcessed = secondHistogram.getForwardProcessed();
        std::vector<std::vector<TH2D>> secondBackwardProcessed = secondHistogram.getBackwardProcessed();
        std::vector<std::vector<TH2D>> secondBackToBackProcessed = secondHistogram.getBackToBackProcessed();
   

        //Unfortunately I did not get std::transform() (from include <algorithm>) to work
        //for element wise addition of nested vectors to work so I go with a nested for loop.
        for (int ptNumber = 0; ptNumber < static_cast<int>(this->_storedForwardList.size()); ptNumber++) {

            int numberOfEntries = static_cast<int>(this->_storedForwardList[ptNumber].size());
            for (int centralityNumber = 0 ; centralityNumber < numberOfEntries; centralityNumber++) {
           
                secondEventNumberList[ptNumber][centralityNumber] = secondEventNumberList[ptNumber][centralityNumber] + this->_eventNumberList[ptNumber][centralityNumber];
     
                //Raw
                this->_storedForwardList[ptNumber][centralityNumber].Add(&secondForwardHistogramCopy[ptNumber][centralityNumber]);
                this->_storedBackwardList[ptNumber][centralityNumber].Add(&secondBackwardHistogramCopy[ptNumber][centralityNumber]);
                
                //Combinatorial and defficiency background (event mixing)
                this->_noCorrelationForwardList[ptNumber][centralityNumber].Add(&secondForwardBackgroundCopy[ptNumber][centralityNumber]);
                this->_noCorrelationBackwardList[ptNumber][centralityNumber].Add(&secondBackwardBackgroundCopy[ptNumber][centralityNumber]);
                

                //Normalised data
                this->_processedForwardList[ptNumber][centralityNumber].Add(&secondForwardProcessed[ptNumber][centralityNumber]);
                this->_processedBackwardList[ptNumber][centralityNumber].Add(&secondBackwardProcessed[ptNumber][centralityNumber]);
                

                if (ptNumber == 0 ) {
                    this->_storedBackToBackList[0][centralityNumber].Add(&secondBackToBackHistogramCopy[0][centralityNumber]);
                    this->_noCorrelationBackToBackList[0][centralityNumber].Add(&secondBackToBackBackgroundCopy[0][centralityNumber]);
                    this->_processedBackToBackList[0][centralityNumber].Add(&secondBackToBackProcessed[0][centralityNumber]);
                    secondEventNumberListFMD[0][centralityNumber] = secondEventNumberListFMD[0][centralityNumber] + this->_eventNumberList[0][centralityNumber];
                }





            }
        }
        this->_eventNumberList = secondEventNumberList;



    }

    


}


    //Stores the various hitograms in a file for later read-in
void storeInHist::storeHistogramInFile() {
    //Sets the new filename
    const std::string filename = _pathToFile.substr(_pathToFile.find_last_of("/")+1, _pathToFile.length());
    std::string storageLocation = "processedData/"+filename.substr(0, filename.find_last_of(".")) +"Processed"+ ".root"; 

    //Stores the histogram
    std::vector<std::vector<TH2D>>* histogramForwardPointer = &(this->_storedForwardList);
    std::vector<std::vector<TH2D>>* histogramBackwardPointer = &(this->_storedBackwardList);
    std::vector<std::vector<TH2D>>* histogramBackToBackPointer = &(this->_storedBackToBackList);

    std::vector<std::vector<TH2D>>* backgroundForwardPointer = &(this->_noCorrelationForwardList);
    std::vector<std::vector<TH2D>>* backgroundBackwardPointer = &(this->_noCorrelationBackwardList);
    std::vector<std::vector<TH2D>>* backgroundBackToBackPointer = &(this->_noCorrelationBackToBackList);

    std::vector<std::vector<TH2D>>* processedForwardPointer = &(this->_processedForwardList);
    std::vector<std::vector<TH2D>>* processedBackwardPointer = &(this->_processedBackwardList);
    std::vector<std::vector<TH2D>>* processedBackToBackPointer = &(this->_processedBackToBackList);

    std::vector<std::vector<int>>* eventNumberListPointer = &(this->_eventNumberList);
    std::vector<std::vector<int>>* eventNumberListFMDPointer = &(this->_eventNumberListFMD);


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

    writeData.WriteObject(eventNumberListPointer, "eventNumberList");
    writeData.WriteObject(eventNumberListFMDPointer, "eventNumberListFMD");

    writeData.Close();

}



    //Performs a normalisation w.r.t event mixing and number of tracks 
    //for the raw data and stores the results in a separate member variable.
    //See the general explanation in the readme file for why [0] instead of [ptIndex]
    //is used in some places
void storeInHist::loadProcessed() {
    if (this->_initialised == 0) {
        throw(std::logic_error("Error: Trying to process unloaded histograms"));
    }

    std::vector<std::vector<TH2D>> histogramForward = this->getForwardHistograms();
    std::vector<std::vector<TH2D>> histogramBackward = this->getBackwardHistograms();
    std::vector<std::vector<TH2D>> histogramBackToBack = this->getBackToBackHistograms();

    std::vector<std::vector<TH2D>> histogramForwardBackground = this->getForwardBackgrounds();
    std::vector<std::vector<TH2D>> histogramBackwardBackground = this->getBackwardBackgrounds();
    std::vector<std::vector<TH2D>> histogramBackToBackBackground = this->getBackToBackBackgrounds();

    std::vector<std::vector<TH2D>> processedForward;
    std::vector<std::vector<TH2D>> processedBackward;
    std::vector<std::vector<TH2D>> processedBackToBack;

    std::vector<TH2D> placeHolderVector;
    TH2D placeHolderHistogram;


    for (int ptNumber = 0; ptNumber < static_cast<int>(this->_storedForwardList.size()); ptNumber++) {
        processedForward.push_back(placeHolderVector);
        processedBackward.push_back(placeHolderVector);
        if (ptNumber == 0) {
            processedBackToBack.push_back(placeHolderVector);
        }

        int numberOfEntries = static_cast<int>(this->_storedForwardList[ptNumber].size());
        for (int centralityNumber = 0 ; centralityNumber < numberOfEntries; centralityNumber++) {
            Double_t maxValueForward = histogramForwardBackground[ptNumber][centralityNumber].GetMaximum();
            Double_t maxValueBackward = histogramBackwardBackground[ptNumber][centralityNumber].GetMaximum();
            
            
            // *= syntax does not seem to be implemented for TH2D
            TH2D normalisedForwardBackground = (1/maxValueForward)*histogramForwardBackground[ptNumber][centralityNumber];
            TH2D normalisedBackwardBackground = (1/maxValueBackward)*histogramBackwardBackground[ptNumber][centralityNumber];
            

            histogramForward[ptNumber][centralityNumber].Divide(&normalisedForwardBackground);
            histogramBackward[ptNumber][centralityNumber].Divide(&normalisedBackwardBackground);
            

            int tpcTracksNormalisation = this->_eventNumberList[ptNumber][centralityNumber];
            int fmdTracksNormalisation = this->_eventNumberListFMD[0][centralityNumber]; //The [0] is intentional
            double both = tpcTracksNormalisation*fmdTracksNormalisation;
            double fmdSquare = fmdTracksNormalisation*fmdTracksNormalisation;
            (void)both;
            (void)fmdSquare;

            processedForward[ptNumber].push_back(histogramForward[ptNumber][centralityNumber]*(1.0/both));
            processedBackward[ptNumber].push_back(histogramBackward[ptNumber][centralityNumber]*(1.0/both));

            //Stores the processed histograms

            if (ptNumber == 0) {
                Double_t maxValueBackToBack = histogramBackToBackBackground[ptNumber][centralityNumber].GetMaximum();
                

                TH2D normalisedBackToBackBackground = (1/maxValueBackToBack)*histogramBackToBackBackground[ptNumber][centralityNumber];
                histogramBackToBack[ptNumber][centralityNumber].Divide(&normalisedBackToBackBackground);
  
                processedBackToBack[ptNumber].push_back(histogramBackToBack[ptNumber][centralityNumber]*(1.0/fmdSquare)); 

            }


        }

    }

    this->_processedForwardList = processedForward;
    this->_processedBackwardList = processedBackward;
    this->_processedBackToBackList = processedBackToBack;

    


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

    std::vector<std::vector<TH2D>>* histogramForward = (std::vector<std::vector<TH2D>>*)dataFile.Get("dataForwardHistogram");
    std::vector<std::vector<TH2D>>* histogramBackward = (std::vector<std::vector<TH2D>>*)dataFile.Get("dataBackwardHistogram");
    std::vector<std::vector<TH2D>>* histogramBackToBack = (std::vector<std::vector<TH2D>>*)dataFile.Get("dataBackToBackHistogram");
    
    std::vector<std::vector<TH2D>>* histogramForwardBackground = (std::vector<std::vector<TH2D>>*)dataFile.Get("dataForwardHistogramBackground");
    std::vector<std::vector<TH2D>>* histogramBackwardBackground = (std::vector<std::vector<TH2D>>*)dataFile.Get("dataBackwardHistogramBackground");
    std::vector<std::vector<TH2D>>* histogramBackToBackBackground = (std::vector<std::vector<TH2D>>*)dataFile.Get("dataBackToBackHistogramBackground");
    std::vector<std::vector<int>>* eventNumberListPtr = (std::vector<std::vector<int>>*)dataFile.Get("eventNumberList");
    std::vector<std::vector<int>>* eventNumberListFMDPtr = (std::vector<std::vector<int>>*)dataFile.Get("eventNumberListFMD");
    
    this->_storedForwardList = *histogramForward;
    this->_storedBackwardList = *histogramBackward;
    this->_storedBackToBackList = *histogramBackToBack;

    this->_noCorrelationForwardList = *histogramForwardBackground;
    this->_noCorrelationBackwardList = *histogramBackwardBackground;
    this->_noCorrelationBackToBackList = *histogramBackToBackBackground;
    
    this->_eventNumberList = *eventNumberListPtr;
    this->_eventNumberListFMD = *eventNumberListFMDPtr;


    //Old versions of the program did not save the processed histograms as member variables 
    // nor in the file. Not reading in processed histograms directly is for backwards compatibility.
    try {
        std::vector<std::vector<TH2D>>* histogramForwardProcessed = (std::vector<std::vector<TH2D>>*)dataFile.Get("dataForwardHistogramProcessed");
        std::vector<std::vector<TH2D>>* histogramBackwardProcessed = (std::vector<std::vector<TH2D>>*)dataFile.Get("dataBackwardHistogramProcessed");
        std::vector<std::vector<TH2D>>* histogramBackToBackProcessed = (std::vector<std::vector<TH2D>>*)dataFile.Get("dataBackToBackHistogramProcessed");

        this->_processedForwardList = *histogramForwardProcessed;
        this->_processedBackwardList = *histogramBackwardProcessed;
        this->_processedBackToBackList = *histogramBackToBackProcessed;


    } catch (...) {

        this->loadProcessed();

    }



    this->_pathToFile = pathToFile.c_str();
    this->_initialised = 1;
    dataFile.Close();

    
}





    //Primary Constructor
storeInHist::storeInHist(std::string pathToFile, Short_t cutOption, 
                                                    Double_t centralityMin, Double_t centralityMax,
                                                    Double_t ptMin, Double_t ptMax,
                                                    Double_t etaMin, Double_t etaMax,
                                                    Int_t countsPhi, Int_t countsEta,
                                                    Int_t start, Int_t stop) : _pathToFile{pathToFile} {
    //Gets the number of entries in the tree
    TFile dataFile(pathToFile.c_str(), "dataFile", "READ");
    TTree* dataTree = (TTree*)dataFile.Get("LWTree");
    Int_t dataCount = dataTree->GetEntries();
    dataFile.Close();
    if (stop > dataCount) {
        stop = dataCount; //To avoid segmentation errors
        throw(std::invalid_argument("The stop number cannot be greater than the number of events. Please compile entries.cpp to check the number of entries"));
    }

    if (start < 0) {
        start = 0;
        throw(std::invalid_argument("The start number cannot be smaller than 0"));

    }
    

    //loadHistogram creates the histograms that are wanted for the different cases. See the readme file for
    //a more detailed explanation. It is a separate method as it is very long and involved.
    std::tuple< std::vector<std::vector<std::vector<TH2D>>>, 
        std::vector<std::vector<int>>, std::vector<std::vector<int>> > returnVector = loadHistograms(pathToFile, cutOption,
                                                                                                    centralityMin, centralityMax,
                                                                                                    ptMin, ptMax,
                                                                                                    etaMin, etaMax,
                                                                                                    countsPhi, countsEta,
                                                                                                    start, stop);
                                                    

    

    this->_storedForwardList = std::get<0>(returnVector)[0];
    this->_storedBackwardList = std::get<0>(returnVector)[1];
    this->_storedBackToBackList = std::get<0>(returnVector)[2];

    this->_noCorrelationForwardList = std::get<0>(returnVector)[3];
    this->_noCorrelationBackwardList = std::get<0>(returnVector)[4];
    this->_noCorrelationBackToBackList = std::get<0>(returnVector)[5];

    this->_eventNumberList = std::get<1>(returnVector);
    this->_eventNumberListFMD = std::get<2>(returnVector);


    
    this->_initialised = 1;
    this->loadProcessed(); //This needs to go after _initialised is set to 1 
                           //since loadProcessed() checks the initialisation status.
    storeHistogramInFile();


}




//This is the main work horse of the program. It reads  in all of the desired histograms as well
//as the number of tracks. See the comments in readData.cpp if you want to know what the arguments do.
//See the readme file for an actual explanation of what is going on, since it is somewhat involved
//I will not put an explanation as inline comments.
std::tuple< std::vector<std::vector<std::vector<TH2D>>>, std::vector<std::vector<int>>, std::vector<std::vector<int>> > storeInHist::loadHistograms(std::string pathToFile, Short_t cutOption, 
                                            Double_t centralityMin, Double_t centralityMax,
                                            Double_t ptMin, Double_t ptMax,
                                            Double_t etaMin, Double_t etaMax,
                                            Int_t countsPhi, Int_t countsEta,
                                            Int_t start, Int_t stop) {

    //Parameters                                      
    Int_t numberOlderEventsToSave = 5;
    (void)cutOption; //outdated variable, left in case it should be reimplemented later. Deliberaltely not using static_cast
                    //as it is supposed to be discarded.

    
    //If there was more time these intervals should probably be determined by an argument
    //given in readData.cpp.
    std::vector<Double_t> startOfPtIntervals {1, 1.5, 2, 2.5, 3, 3.5, 4, 5, 6}; //{0.2, 0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 5, 6
    if (ptMin < startOfPtIntervals[0]) {
        ptMin = startOfPtIntervals[0];
    }
    if (ptMax > startOfPtIntervals[startOfPtIntervals.size()-1]) {
        ptMax = startOfPtIntervals[startOfPtIntervals.size()-1];
    }

    std::vector<Double_t> startOfCentralityIntervals {50, 60, 65, 70, 75, 80, 85, 90};
    int numberOfEntriesCentrality = static_cast<int>(startOfCentralityIntervals.size());
    int numberOfEntriesPt = static_cast<int>(startOfPtIntervals.size());
    
    //Sets up the correct structures of the vectors holding the data for the
    //various cases so that they may be handled more easily later.

        //Data to be stored
    std::vector<std::vector<TH2D>> forwardVector;
    std::vector<std::vector<TH2D>> backwardVector;
    std::vector<std::vector<TH2D>> backToBackVector;

    std::vector<std::vector<TH2D>> forwardBackgroundVector;
    std::vector<std::vector<TH2D>> backwardBackgroundVector;
    std::vector<std::vector<TH2D>> backToBackBackgroundVector;

    std::vector<std::vector<int>> eventNumbers;
    std::vector<std::vector<int>> eventNumbersFMD;

    std::vector<int> eventNumbersPlaceHolder;  
    std::vector<TH2D> placeHolderVector;
    TH2D placeHolderHistogram("histogram", "Counts", countsPhi, 0, 2*TMath::Pi(), countsEta, etaMin, etaMax);

        //Things needed for the event mixing
    std::vector<std::vector<std::vector<std::vector<Double_t>>>> oldPhiTracksTPCvector;
    std::vector<std::vector<std::vector<std::vector<Double_t>>>> oldEtaTracksTPCvector;
    std::vector<std::vector<std::vector<std::vector<Double_t>>>> oldPhiTracksBackwardFMDvector;
    std::vector<std::vector<std::vector<std::vector<Double_t>>>> oldEtaTracksBackwardFMDvector;
    std::vector<std::vector<std::vector<std::vector<Int_t>>>> oldMultiplicityTracksBackwardFMDvector;

    std::vector<std::vector<std::vector<Double_t>>> oldPhiTracksTPCvectorPlaceHolder;
    std::vector<std::vector<std::vector<Double_t>>> oldEtaTracksTPCvectorPlaceHolder;
    std::vector<std::vector<std::vector<Double_t>>> oldPhiTracksBackwardFMDvectorPlaceHolder;
    std::vector<std::vector<std::vector<Double_t>>> oldEtaTracksBackwardFMDvectorPlaceHolder;
    std::vector<std::vector<std::vector<Int_t>>> oldMultiplicityTracksBackwardFMDvectorPlaceHolder;
    

    std::vector<std::vector<Double_t>> oldPhiTracksTPC;
    std::vector<std::vector<Double_t>> oldEtaTracksTPC;
    std::vector<std::vector<Double_t>> oldPhiTracksBackwardFMD;
    std::vector<std::vector<Double_t>> oldEtaTracksBackwardFMD;
    std::vector<std::vector<Int_t>> oldMultiplicityTracksBackwardFMD;


    std::vector<std::vector<std::vector<Double_t>>> tracksPhiTPCvector;
    std::vector<std::vector<std::vector<Double_t>>> tracksEtaTPCvector;

    std::vector<std::vector<Double_t>> tracksPhiTPCvectorPlaceHolder;
    std::vector<std::vector<Double_t>> tracksEtaTPCvectorPlaceHolder;

    //To avoid excessive copying of the same data over and over again which takes time,
    //I predefine the length of the vectors to somewhat above what the largest number of tracks seem to be
    //in the FMD and TPC respectively
    int reserveNumberFMD = 500;
    int reserveNumberTPC = 150000;

    std::vector<Double_t> forwardTracksPhi;
    std::vector<Double_t> backwardTracksPhi;
    std::vector<Double_t> forwardTracksEta;
    std::vector<Double_t> backwardTracksEta; 
    std::vector<Int_t> forwardTracksMult;
    std::vector<Int_t> backwardTracksMult;
    forwardTracksPhi.reserve(reserveNumberFMD);
    backwardTracksPhi.reserve(reserveNumberFMD);
    forwardTracksEta.reserve(reserveNumberFMD);
    backwardTracksEta.reserve(reserveNumberFMD);
    forwardTracksMult.reserve(reserveNumberFMD);
    backwardTracksMult.reserve(reserveNumberFMD);

    std::vector<Double_t> tracksPhiTPC;
    std::vector<Double_t> tracksEtaTPC;
    tracksPhiTPC.reserve(reserveNumberTPC);   
    tracksEtaTPC.reserve(reserveNumberTPC);
    

    
    for (int ptNumber = 0; ptNumber < numberOfEntriesPt -1 ; ptNumber++) { //The last interval is 85-90, hence the -1 
        forwardVector.push_back(placeHolderVector);
        backwardVector.push_back(placeHolderVector);
        forwardBackgroundVector.push_back(placeHolderVector);
        backwardBackgroundVector.push_back(placeHolderVector);


        tracksPhiTPCvector.push_back(tracksPhiTPCvectorPlaceHolder);
        tracksEtaTPCvector.push_back(tracksEtaTPCvectorPlaceHolder);
        oldPhiTracksTPCvector.push_back(oldPhiTracksTPCvectorPlaceHolder);
        oldEtaTracksTPCvector.push_back(oldEtaTracksTPCvectorPlaceHolder);

        eventNumbers.push_back(eventNumbersPlaceHolder);


        if (ptNumber == 0) { //There are no pT cuts in the fmd:s, structure is kept for consistency
            backToBackVector.push_back(placeHolderVector);
            backToBackBackgroundVector.push_back(placeHolderVector);
            
            oldPhiTracksBackwardFMDvector.push_back(oldPhiTracksBackwardFMDvectorPlaceHolder);
            oldEtaTracksBackwardFMDvector.push_back(oldEtaTracksBackwardFMDvectorPlaceHolder);
            oldMultiplicityTracksBackwardFMDvector.push_back(oldMultiplicityTracksBackwardFMDvectorPlaceHolder);
            eventNumbersFMD.push_back(eventNumbersPlaceHolder);
        }



        for (int centralityNumber = 0 ; centralityNumber < numberOfEntriesCentrality -1; centralityNumber++) { //-1 is intentional

            forwardVector[ptNumber].push_back(placeHolderHistogram);
            backwardVector[ptNumber].push_back(placeHolderHistogram);
            forwardBackgroundVector[ptNumber].push_back(placeHolderHistogram);
            backwardBackgroundVector[ptNumber].push_back(placeHolderHistogram);


            oldPhiTracksTPCvector[ptNumber].push_back(oldPhiTracksTPC);
            oldEtaTracksTPCvector[ptNumber].push_back(oldEtaTracksTPC);
            tracksPhiTPCvector[ptNumber].push_back(tracksPhiTPC);
            tracksEtaTPCvector[ptNumber].push_back(tracksEtaTPC);

            
            eventNumbers[ptNumber].push_back(0);


            if (ptNumber == 0) {
                backToBackVector[0].push_back(placeHolderHistogram);
                backToBackBackgroundVector[0].push_back(placeHolderHistogram);

                oldPhiTracksBackwardFMDvector[0].push_back(oldPhiTracksBackwardFMD);
                oldEtaTracksBackwardFMDvector[0].push_back(oldEtaTracksBackwardFMD);
                oldMultiplicityTracksBackwardFMDvector[0].push_back(oldMultiplicityTracksBackwardFMD);

                eventNumbersFMD[ptNumber].push_back(0);
            }


        }



    }
    





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

    //Variables for keeping track of which category data is to be saved to
    int centralityIndex;
    int ptIndex;


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

    
    //Values to be stored in the histograms
    Double_t etaDiff; //For TPC-FMD correlations
    Double_t phiDiff;







    //Event-loop
    for (Int_t eventNumber = start; eventNumber < stop; eventNumber++) {
        //Reads in the data for the event
        dataTree->GetEntry(eventNumber);

        
        //Determines which interval the centrality belongs to and skips it if it isn't wanted.     
        centrality = event->fCent;
        if ((centrality < centralityMin) || (centrality > centralityMax)) {
            continue;
        } else {
            for (int centralityNumber = 0; centralityNumber < numberOfEntriesCentrality -1; centralityNumber++) {
                if ((centrality >= startOfCentralityIntervals[centralityNumber]) && (centrality < startOfCentralityIntervals[centralityNumber+1])) {
                    centralityIndex = centralityNumber;
                    break;
                }
            }

        }

        
        //Clears data from previous events
        forwardTracksEta.clear();
        backwardTracksEta.clear();
        forwardTracksPhi.clear();
        backwardTracksPhi.clear();
        forwardTracksMult.clear();
        backwardTracksMult.clear();
        for (int ptNumber = 0; ptNumber < numberOfEntriesPt - 1; ptNumber++) { //-1 is intentional
            tracksPhiTPCvector[ptNumber][centralityIndex].clear();
            tracksEtaTPCvector[ptNumber][centralityIndex].clear();
        }


        //Number of events to loop over in the FMD and TPC
        trackCountTPC = tpcTrack->GetEntries(); 
        trackCountFMD = fmdTrack->GetEntries();
        

        //FMD loop start
        for (Int_t fmdTrackNumber = 0; fmdTrackNumber < trackCountFMD; fmdTrackNumber++) {
            //Gets details about the track
            currentTrackFMD = static_cast<AliLWFMDTrack*>((*fmdTrack)[fmdTrackNumber]); 
            etaValFMD = currentTrackFMD->fEta;

            //Cutting away data where the resolution is low,
            //if there was more time this should probably had been defined in readData.cpp
            if ((etaValFMD < -3.1) || (etaValFMD > -2)) {
                if ((etaValFMD < 3.8) || (etaValFMD > 4.7)) {
                    if ((etaValFMD < 2.5) || (etaValFMD > 3.1)) {
                        continue;

                    }
                }
            }

            //Reads in these values after doing the eta check to save time 
            //if there are values which are skipped
            fmdMultiplicity = currentTrackFMD->fMult; 
            phiValFMD = currentTrackFMD->fPhi;

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


            /*
            The code grew sort of organically as more and more features had to be implemented.
            Originally, there was no event mixing so I made a nested for loop of FMD and TPC
            tracks. In the current implementation, tracks have to be saved anyhow since
            they are needed in the event mixing, so the faster option would be to 'un-nest'
            these loops and have both the TPC and event mixing in separate loops.
            Since the event mixing is by far the biggest time sink, and since that would take equally
            long unnested, I have not bothered with un-nesting the loops. If one were to write
            it from scatch with all the hindsight, these loops should of course be written in an unnested
            way as that would be much simpler.
            */
            
            //Loops through all tracks in the TPC so TPC-FMD correlations can be calculated
            for (Int_t tpcTrackNumber = 0; tpcTrackNumber < trackCountTPC; tpcTrackNumber++) {
                currentTrackTPC = static_cast<AliLWTPCTrack*>((*tpcTrack)[tpcTrackNumber]);
                cutFlag = currentTrackTPC->fTrFlag;
                //Cutting away data where with the wrong flag(s).
                if (!(cutFlag & 2)) {
                    continue;
                } 

                //Cutting away data where the resolution is low
                etaValTPC = currentTrackTPC->fEta;
                if ((etaValTPC < -0.75) || (etaValTPC > 0.75)) { 
                    continue;
                } 
                
                //Cuts away unwanted pT:s
                pT = currentTrackTPC->fPt;
                if ((pT < ptMin) || (pT > ptMax)) {
                    continue;
                }

                for (int ptNumber = 0; ptNumber < numberOfEntriesPt -1; ptNumber++) {
                    if ( (pT >= startOfPtIntervals[ptNumber]) && (pT < startOfPtIntervals[ptNumber+1]) ) {
                        ptIndex = ptNumber;
                        break;
                    }
                }
                
                

                phiValTPC = currentTrackTPC->fPhi;
                phiDiff = phiValFMD - phiValTPC;
                etaDiff = etaValFMD - etaValTPC;

                //Makes sures all values are positive (we want all values within one period)
                if (phiDiff < 0) {phiDiff += 2*TMath::Pi();}

                
                if (fmdTrackNumber == 0) {
                    eventNumbers[ptIndex][centralityIndex] += 1;
                }

                //Stores the read-in and approved values for later event mixing
            
                tracksPhiTPCvector[ptIndex][centralityIndex].push_back(phiValTPC);
                tracksEtaTPCvector[ptIndex][centralityIndex].push_back(etaValTPC);
                

                //Fills the correct histogram. The sign determines if it is the forwards or backwards FMD
                if (etaValFMD > 0) {
                    forwardVector[ptIndex][centralityIndex].Fill(phiDiff, etaDiff, fmdMultiplicity);

                } else {
                    backwardVector[ptIndex][centralityIndex].Fill(phiDiff, etaDiff, fmdMultiplicity);

                }

                

            } //TPC-loop end
  
        
            
            

            //Event mixing start
            //FMD-oldTPC correlations
            if (etaValFMD > 0) {
                for (int ptNumber = 0; ptNumber < numberOfEntriesPt-1; ptNumber++) {
                    for (int oldEventNumber = 0; oldEventNumber < static_cast<int>(oldPhiTracksTPCvector[ptNumber][centralityIndex].size()); oldEventNumber++) {     

                        calculateSingleCorrelation(forwardBackgroundVector[ptNumber][centralityIndex], phiValFMD, etaValFMD,
                                        oldPhiTracksTPCvector[ptNumber][centralityIndex][oldEventNumber], 
                                        oldEtaTracksTPCvector[ptNumber][centralityIndex][oldEventNumber],
                                        fmdMultiplicity);

                    }

                }
            } else {
                for (int ptNumber = 0; ptNumber < numberOfEntriesPt-1; ptNumber++) {
                    for (int oldEventNumber = 0; oldEventNumber < static_cast<int>(oldPhiTracksTPCvector[ptNumber][centralityIndex].size()); oldEventNumber++) {
                        calculateSingleCorrelation(backwardBackgroundVector[ptNumber][centralityIndex], phiValFMD, etaValFMD,
                                        oldPhiTracksTPCvector[ptNumber][centralityIndex][oldEventNumber], 
                                        oldEtaTracksTPCvector[ptNumber][centralityIndex][oldEventNumber],
                                        fmdMultiplicity); 
                    }

                }           
            }

            //forwardFMD-oldBackwardFMD
            if (etaValFMD > 0 ) {
                for (int oldEventNumber = 0; oldEventNumber < static_cast<int>(oldPhiTracksBackwardFMDvector[0][centralityIndex].size()); oldEventNumber++) {       
                    calculateSingleCorrelation(backToBackBackgroundVector[0][centralityIndex], phiValFMD, etaValFMD,
                                        oldPhiTracksBackwardFMDvector[0][centralityIndex][oldEventNumber], oldEtaTracksBackwardFMDvector[0][centralityIndex][oldEventNumber],
                                        fmdMultiplicity, oldMultiplicityTracksBackwardFMDvector[0][centralityIndex][oldEventNumber]);

                }                
            }

           
            //Event Mixing End

            //Keeping track of how many tracks that are approved for later normalisation
            eventNumbersFMD[0][centralityIndex] += 1;
        } //FMD-loop end



        //Calculates FMD-FMD correlations
        calculateCorrelation(backToBackVector[0][centralityIndex], forwardTracksPhi, forwardTracksEta, 
                            backwardTracksPhi, backwardTracksEta, 
                            forwardTracksMult, backwardTracksMult);

       
        


        

        //Updates the stored old events with the current one and removes the oldest one
        for (int ptNumber = 0; ptNumber < numberOfEntriesPt-1; ptNumber++) {
            if (static_cast<int>(tracksPhiTPCvector[ptNumber][centralityIndex].size()) >= 1) {
                oldPhiTracksTPCvector[ptNumber][centralityIndex].push_back(tracksPhiTPCvector[ptNumber][centralityIndex]);
                oldEtaTracksTPCvector[ptNumber][centralityIndex].push_back(tracksEtaTPCvector[ptNumber][centralityIndex]);
            }

            if (static_cast<int>(oldPhiTracksTPCvector[ptNumber][centralityIndex].size()) > numberOlderEventsToSave) {
                oldPhiTracksTPCvector[ptNumber][centralityIndex].erase(oldPhiTracksTPCvector[ptNumber][centralityIndex].begin());
                oldEtaTracksTPCvector[ptNumber][centralityIndex].erase(oldEtaTracksTPCvector[ptNumber][centralityIndex].begin());
            }
        }

        oldPhiTracksBackwardFMDvector[0][centralityIndex].push_back(backwardTracksPhi);    
        oldEtaTracksBackwardFMDvector[0][centralityIndex].push_back(backwardTracksEta);
        oldMultiplicityTracksBackwardFMDvector[0][centralityIndex].push_back(backwardTracksMult);


        if (static_cast<int>(oldPhiTracksBackwardFMDvector[0][centralityIndex].size()) > numberOlderEventsToSave) {
            oldPhiTracksBackwardFMDvector[0][centralityIndex].erase(oldPhiTracksBackwardFMDvector[0][centralityIndex].begin());
            oldEtaTracksBackwardFMDvector[0][centralityIndex].erase(oldEtaTracksBackwardFMDvector[0][centralityIndex].begin());
            oldMultiplicityTracksBackwardFMDvector[0][centralityIndex].erase(oldMultiplicityTracksBackwardFMDvector[0][centralityIndex].begin());

        }
        
    

     
    } //Event-loop end  


    




    //Returns the results
    std::vector<std::vector<std::vector<TH2D>>> returnVector;
    returnVector.push_back(forwardVector);
    returnVector.push_back(backwardVector);
    returnVector.push_back(backToBackVector);
    returnVector.push_back(forwardBackgroundVector);
    returnVector.push_back(backwardBackgroundVector);
    returnVector.push_back(backToBackBackgroundVector);

    //Sets the minimum number of track to 1 to avoid division by 0 errors later with having
    //a lot of if statements in .loadProcessed()
    for (int ptNumber = 0; ptNumber < numberOfEntriesPt -1 ; ptNumber++) {
        for (int centralityNumber = 0; centralityNumber < numberOfEntriesCentrality -1 ; centralityNumber++) {
            if (eventNumbers[ptNumber][centralityNumber] == 0) {
                eventNumbers[ptNumber][centralityNumber] = 1;
            }

            if (eventNumbersFMD[0][centralityNumber] == 0) {
                eventNumbersFMD[0][centralityNumber] = 1;
            }

        }
    }
    
    
    auto returnTuple = std::make_tuple(returnVector, eventNumbers, eventNumbersFMD);

    dataFile.Close();
    delete event;
    delete tpcTrack;
    delete fmdTrack;


    return returnTuple;



}


