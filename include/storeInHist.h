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

        //Member functions
        std::vector<TH2D> loadHistogram(std::string pathToFile, Int_t start, Int_t stop,
                                                 Int_t countsX, Int_t countsY, Double_t etaMin, Double_t etaMax, Short_t cutOption);

        


    public:

        //Member Functions
        storeInHist(std::string pathToFile, Int_t binsPhi, Int_t binsEta, Double_t etaMin, Double_t etaMax, Short_t cutOption);
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