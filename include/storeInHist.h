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
        Int_t _unInitialised;
        TH2D _storedHistogram; //I might define addition for histograms 
                               // later so this is not marked as const

        //Member functions
        TH2D loadHistogram(std::string pathToFile, Int_t start, Int_t stop,
                                                 Int_t countsX, Int_t countsY, Double_t etaMin, Double_t etaMax);

        


    public:

        //Member Functions
        storeInHist(std::string pathToFile, Int_t binsPhi, Int_t binsEta, Double_t etaMin, Double_t etaMax);
        storeInHist(std::string pathToFile);
        //A default constructor is necessary in a later part of the code. However,
        //ROOT's TObjects interferes with this so I just make one with a number instead.
        storeInHist(Int_t number);
        const TH2D getHistogram();
        void addHistogram(storeInHist secondHistogram);
        void setStorageName(std::string location);
        std::string getFilePath();
        void storeHistogramInFile();



        ClassDef(storeInHist, 1);
        




};



#endif //__storeInHistFredholm__