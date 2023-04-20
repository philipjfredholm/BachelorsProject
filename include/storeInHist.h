#ifndef __storeInHistFredholm__
#define __storeInHistFredholm__

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <exception>
#include <algorithm>
#include <tuple>

#include "AliLWUtils.h"

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TObject.h>
#include <TClonesArray.h>
#include <TMath.h>
#include <TClonesArray.h>


class storeInHist : TObject {
    private:
        //Member Variablies
        std::string _pathToFile;
        Int_t _initialised;

        std::vector<std::vector<TH2D>> _storedForwardList;
        std::vector<std::vector<TH2D>> _storedBackwardList;
        std::vector<std::vector<TH2D>> _storedBackToBackList;
        
        std::vector<std::vector<TH2D>> _noCorrelationForwardList;
        std::vector<std::vector<TH2D>> _noCorrelationBackwardList;
        std::vector<std::vector<TH2D>> _noCorrelationBackToBackList;

        std::vector<std::vector<TH2D>> _processedForwardList;
        std::vector<std::vector<TH2D>> _processedBackwardList;
        std::vector<std::vector<TH2D>> _processedBackToBackList;

        std::vector<std::vector<int>> _eventNumberList; 
        std::vector<std::vector<int>> _eventNumberListFMD; 


        //Member functions
        std::tuple< std::vector<std::vector<std::vector<TH2D>>>, std::vector<std::vector<int>>, std::vector<std::vector<int>> > loadHistograms(std::string pathToFile, Short_t cutOption, 
                                        Double_t centralityMin, Double_t centralityMax,
                                        Double_t ptMin, Double_t ptMax,
                                        Double_t etaMin, Double_t etaMax,
                                        Int_t countsPhi, Int_t countsEta,
                                        Int_t start, Int_t stop);


        
        
        //FMD-FMD
        void calculateCorrelation(TH2D& myHistogram, const std::vector<Double_t>& phi1, const std::vector<Double_t>& eta1,
                                  const std::vector<Double_t>& phi2, const std::vector<Double_t>& eta2,
                                  const std::vector<Int_t>& mult1, const std::vector<Int_t>& mult2);
        
        //FMD-TPC
        void calculateCorrelation(TH2D& myHistogram, const std::vector<Double_t>& phi1, const std::vector<Double_t>& eta1,
                                  const std::vector<Double_t>& phi2, const std::vector<Double_t>& eta2,
                                  const std::vector<Int_t>& mult2);


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
        storeInHist(std::string pathToFile, Short_t cutOption, 
                                        Double_t centralityMin, Double_t centralityMax,
                                        Double_t ptMin, Double_t ptMax,
                                        Double_t etaMin, Double_t etaMax,
                                        Int_t countsPhi, Int_t countsEta,
                                        Int_t start, Int_t stop);
        storeInHist(std::string pathToFile);

        //A default constructor is necessary in a later part of the code. However,
        //ROOT's TObjects interferes with this so I just make one with a number instead.
        storeInHist(Int_t number);

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

        void addHistograms(storeInHist secondHistogram);
        void setStorageName(std::string location);
        std::string getFilePath();
        void storeHistogramInFile();
        void loadProcessed();



        ClassDef(storeInHist, 1);
        




};



#endif //__storeInHistFredholm__