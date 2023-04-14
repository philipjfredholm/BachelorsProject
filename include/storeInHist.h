#ifndef __storeInHistFredholm__
#define __storeInHistFredholm__

#include <iostream>
#include <vector>
#include <string>
#include <thread>

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

        TH2D _storedForwardHistogram;
        TH2D _storedBackwardHistogram;
        TH2D _storedBackToBackHistogram;
        
        TH2D _noCorrelationForward; 
        TH2D _noCorrelationBackward;
        TH2D _noCorrelationBackToBack;

        TH2D _processedForward;
        TH2D _processedBackward;
        TH2D _processedBackToBack;


        //Member functions
        std::vector<TH2D> loadHistogram(std::string pathToFile, Short_t cutOption, 
                                        Double_t centralityMin, Double_t centralityMax,
                                        Double_t ptMin, Double_t ptMax,
                                        Double_t etaMin, Double_t etaMax,
                                        Int_t countsPhi, Int_t countsEta,
                                        Int_t start, Int_t stop);

        std::vector<TH2D> loadBackground(std::string pathToFile, Short_t cutOption, 
                                        Double_t centralityMin, Double_t centralityMax,
                                        Double_t ptMin, Double_t ptMax,
                                        Double_t etaMin, Double_t etaMax,
                                        Int_t countsPhi, Int_t countsEta,
                                        Int_t start, Int_t stop);

        void loadProcessed();
        
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
                                        Int_t countsPhi, Int_t countsEta);
        storeInHist(std::string pathToFile);
        //A default constructor is necessary in a later part of the code. However,
        //ROOT's TObjects interferes with this so I just make one with a number instead.
        storeInHist(Int_t number);

        const TH2D getForwardHistogram();
        const TH2D getBackwardHistogram();
        const TH2D getBackToBackHistogram();

        const TH2D getForwardBackground();
        const TH2D getBackwardBackground();
        const TH2D getBackToBackBackground();

        const TH2D getForwardProcessed();
        const TH2D getBackwardProcessed();
        const TH2D getBackToBackProcessed();

        void addHistogram(storeInHist secondHistogram);
        void setStorageName(std::string location);
        std::string getFilePath();
        void storeHistogramInFile();
        void storeBackgroundInFile();



        ClassDef(storeInHist, 1);
        




};



#endif //__storeInHistFredholm__