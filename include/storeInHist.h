#ifndef __storeInHistFredholm__
#define __storeInHistFredholm__

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <filesystem>

#include "AliLWUtils.h"

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TObject.h>
#include <TClonesArray.h>
#include <TMath.h>


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
        void addHistogram(storeInHist secondHistogram);
        void setStorageName(std::string location);
        std::string getFilePath();
        void storeHistogramInFile();



        ClassDef(storeInHist, 1);
        




};



#endif //__storeInHistFredholm__