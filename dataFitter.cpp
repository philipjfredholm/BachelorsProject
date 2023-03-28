//Core C++
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <future>
#include <mutex>
#include <filesystem>


//Other
#include "include/storeInHist.h"
#include "AliLWUtils.h"

//ROOT
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TObject.h>
#include <TRint.h>
#include <TApplication.h>
#include <TBranch.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TClonesArray.h>
#include <TMath.h>


Double_t ppHistogramValue(Double_t x) {

    return 1;
}


Double_t v2Extractor(Double_t* values, Double_t* parameters) {
    /*
    Possible Parameters:
    pp-background
    Fourier background
    v_1, v_2, v_3, v_4 etc (Should v_1 be included?)
    \Psi_{RP}
    */
    Double_t background = parameters[0]*ppHistogramValue(values[0]) 
    Double_t ellipticFlow = parameters[1] *(1+parameters[2]*std::cos(2*values[0]) +parameters[3]*std::cos(3*values[]))

    for (int k = 0; k < parameters.sizeof(); k++) {
        ellipticFlow += parameters[k] * std::cos(k*values[0]);
    }
    
    return background+ellipticFlow;

}



int main() {

    









}