#ifndef __storeInHistFredholm__
#define __storeInHistFredholm__

#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <algorithm>
#include <tuple>
#include <exception>


#include "AliLWUtils.h"

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TObject.h>
#include <TClonesArray.h>
#include <TMath.h>
#include <TClonesArray.h>


class storeInHist {
    private:
        //Member Variablies
        std::string _pathToFile; //Where the file is to be stored if storeHistogramInFile() is called
        Int_t _initialised; //For re-implmenting a default constructor since inheritance from
                            //TObject steals it.

        //Raw correlation data
        std::vector<std::vector<TH2D>> _storedForwardList;
        std::vector<std::vector<TH2D>> _storedBackwardList;
        std::vector<std::vector<TH2D>> _storedBackToBackList;
        
        //Backgrounds due to event-mixing
        std::vector<std::vector<TH2D>> _noCorrelationForwardList;
        std::vector<std::vector<TH2D>> _noCorrelationBackwardList;
        std::vector<std::vector<TH2D>> _noCorrelationBackToBackList;

        //Raw correlation data which has been normalised w.r.t
        //event-mixing and number of tracks. Note that
        //the data stored here is not acutally useful
        //until combine.cpp reads calculates things
        //properly with respect to all tracks
        std::vector<std::vector<TH2D>> _processedForwardList;
        std::vector<std::vector<TH2D>> _processedBackwardList;
        std::vector<std::vector<TH2D>> _processedBackToBackList;

        //Stores number of tracks for different categories so 
        //proper normalisation may take place in loadProcessed()
        std::vector<std::vector<int>> _eventNumberList; //TPC
        std::vector<std::vector<int>> _eventNumberListFMD; 


        //Member functions
            //This is the main 'work horse' of the program
        std::tuple< std::vector<std::vector<std::vector<TH2D>>>, std::vector<std::vector<int>>, std::vector<std::vector<int>> > loadHistograms(std::string pathToFile, Short_t cutOption, 
                                        Double_t centralityMin, Double_t centralityMax,
                                        Double_t ptMin, Double_t ptMax,
                                        Double_t etaMin, Double_t etaMax,
                                        Int_t countsPhi, Int_t countsEta,
                                        Int_t start, Int_t stop);


        
        //Various methods for calculting correlations for different cases
            //FMD-FMD
        void calculateCorrelation(TH2D& myHistogram, const std::vector<Double_t>& phi1, const std::vector<Double_t>& eta1,
                                  const std::vector<Double_t>& phi2, const std::vector<Double_t>& eta2,
                                  const std::vector<Int_t>& mult1, const std::vector<Int_t>& mult2);
        


            //FMD-FMD
        void calculateSingleCorrelation(TH2D& myHistogram, const Double_t& phi1, const Double_t& eta1,
                                  const std::vector<Double_t>& phi2, const std::vector<Double_t>& eta2,
                                  const Int_t& mult1, const std::vector<Int_t>& mult2);
        
            //FMD-TPC
        void calculateSingleCorrelation(TH2D& myHistogram, const Double_t& phi1, const Double_t& eta1,
                                  const std::vector<Double_t>& phi2, const std::vector<Double_t>& eta2,
                                  const Int_t& mult2);
                                        


    public:

        //Member Functions
            //Primary constructor
        storeInHist(std::string pathToFile, Short_t cutOption, 
                                        Double_t centralityMin, Double_t centralityMax,
                                        Double_t ptMin, Double_t ptMax,
                                        Double_t etaMin, Double_t etaMax,
                                        Int_t countsPhi, Int_t countsEta,
                                        Int_t start, Int_t stop);
            //Constructs by reading in old saved data
        storeInHist(std::string pathToFile);

        //A default constructor is necessary in a later part of the code. However,
        //inheritance from ROOT's TObject interferes with this so I just make one with a number instead.
        storeInHist(Int_t number);

        //Getters
        std::string getFilePath();
        const std::vector<std::vector<TH2D>> getForwardHistograms();
        const std::vector<std::vector<TH2D>> getBackwardHistograms();
        const std::vector<std::vector<TH2D>> getBackToBackHistograms();

        const std::vector<std::vector<TH2D>> getForwardBackgrounds();
        const std::vector<std::vector<TH2D>> getBackwardBackgrounds();
        const std::vector<std::vector<TH2D>> getBackToBackBackgrounds();

        const std::vector<std::vector<TH2D>> getForwardProcessed();
        const std::vector<std::vector<TH2D>> getBackwardProcessed();
        const std::vector<std::vector<TH2D>> getBackToBackProcessed();

        const std::vector<std::vector<int>> getEventNumberList();
        const std::vector<std::vector<int>> getEventNumberListFMD();

        void setForwardHistograms(std::vector<std::vector<TH2D>> newVector);
        void setBackwardHistograms(std::vector<std::vector<TH2D>> newVector);
        void setBackToBackHistograms(std::vector<std::vector<TH2D>> newVector);

        void setForwardBackgrounds(std::vector<std::vector<TH2D>> newVector);
        void setBackwardBackgrounds(std::vector<std::vector<TH2D>> newVector);
        void setBackToBackBackgrounds(std::vector<std::vector<TH2D>> newVector);

        void setForwardProcessed(std::vector<std::vector<TH2D>> newVector);
        void setBackwardProcessed(std::vector<std::vector<TH2D>> newVector);
        void setBackToBackProcessed(std::vector<std::vector<TH2D>> newVector);

        void setEventNumberList(std::vector<std::vector<int>> newVector);
        void setEventNumberListFMD(std::vector<std::vector<int>> newVector);

        

        //Setters
        void setStorageName(std::string location);

        //Other
        void addHistograms(storeInHist secondHistogram);
        void storeHistogramInFile();
        void loadProcessed();




        //ClassDef(storeInHist, 1); //For ROOT compatibility
        




};



#endif //__storeInHistFredholm__