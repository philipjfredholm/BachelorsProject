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


class storeInHist : TObject {
    private:
        //Member Variablies
        const std::string _pathToFile;
        TH2D _storedHistogram; //I might define addition for histograms 
                               // later so this is not marked as const

        //Member functions
        const TH2D loadHistogram(std::string pathToFile, Int_t start, Int_t stop,
                                                 Int_t countsX, Int_t countsY);

        void storeHistogramInFile();


    public:

        //Member Functions
        storeInHist(std::string pathToFile);
        const TH2D getHist();



        ClassDef(storeInHist, 1);
        




};



#endif //__storeInHistFredholm__