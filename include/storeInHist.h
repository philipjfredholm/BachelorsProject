#ifndef __storeInHistFredholm__
#define __storeInHistFredholm__

#include <iostream>
#include <vector>
#include <string>

#include "AliLWUtils.h"

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TObject.h>
#include <TClonesArray.h>
#include <TMath.h>


class storeInHist : TObject {
    private:
        const std::string _pathToFile;
        //std::string _pathToFiles;
        /*
        const std::string _pathToOutput;
        const Double_t _startHist;
        const Double_t _endHist;
        const Int_t _cuts;
        */

    public:
        storeInHist(std::string pathToFile);



        ClassDef(storeInHist, 1);
        




};



#endif //__storeInHistFredholm__