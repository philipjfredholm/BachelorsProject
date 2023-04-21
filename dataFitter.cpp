//Core C++
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <exception>

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


/* Purpose:
This program is intended to able to extract the the various flow harmonic from
correlations taken from the lead-lead (PbPb) collisions by fitting them to Fourier harmonics
and background from proton-proton (pp) collisions to account for non flow effects of 
QCD.
 */



int main(int argc, char **argv) {
    //Argument Processing
    std::string pathToFile = argv[1];
    std::string ptIndexString = argv[2];
    std::string centralityIndexString = argv[3]; 

    int ptIndex = std::stoi(ptIndexString);
    int centralityIndex = std::stoi(centralityIndexString);

    //Creates the application
    char myChar = 'a'; //ROOT requires argv but I do not want to give it.
    char* myCharPtr = &myChar;
    TRint app("app", 0, &myCharPtr);
    TCanvas canvas("canvas", "Title", 0, 0 ,800,600);




    //Runs the application
    canvas.Modified(); 
    canvas.Update();
    app.Run();


}



double v2ExtractorForward(double* values, double* parameters) {

    double background = parameters[0]*ppHistogramValueForward(values[0]);
    double ellipticFlow = parameters[1] *(1+parameters[2]*std::cos(2*values[0]) +parameters[3]*std::cos(3*values[]));

    
    return background+ellipticFlow;

} 



double ppHistogramValueForward(Double_t x, storeInHist myHistogram, int ptIndex, int centralityIndex) {
    //Check that the input is valid
    if ((x < 0) || (x >= 2*TMath::Pi())) {
        throw(std::invalid_argument("The value must be between 0 and 2pi"));
    }

    //Read in histograms
    std::vector<std::vector<TH2D>> ppBackgroundVector = myHistogram.getForwardProcessed();
    TH2D ppBackground = ppBackgroundVector[ptIndex][centralityIndex];
    TH1D phiProjection = *ppBackground.ProjectionX();

    //Finds the correct value to return
    int resultIndex = phiProjection.FindBin(x);
    double returnValue = phiProjection.GetBinContent(resultIndex);




    return returnValue;
}



double ppHistogramValueBackward(Double_t x, storeInHist myHistogram, int ptIndex, int centralityIndex) {
    //Check that the input is valid
    if ((x < 0) || (x >= 2*TMath::Pi())) {
        throw(std::invalid_argument("The value must be between 0 and 2pi"));
    }

    //Read in histograms
    std::vector<std::vector<TH2D>> ppBackgroundVector = myHistogram.getBackwardProcessed();
    TH2D ppBackground = ppBackgroundVector[ptIndex][centralityIndex];
    TH1D phiProjection = *ppBackground.ProjectionX();

    //Finds the correct value to return
    int resultIndex = phiProjection.FindBin(x);
    double returnValue = phiProjection.GetBinContent(resultIndex);




    return returnValue;
}



double ppHistogramValueBackToBack(Double_t x, storeInHist myHistogram, int ptIndex, int centralityIndex) {
    //Check that the input is valid
    if ((x < 0) || (x >= 2*TMath::Pi())) {
        throw(std::invalid_argument("The value must be between 0 and 2pi"));
    }

    //Read in histograms
    std::vector<std::vector<TH2D>> ppBackgroundVector = myHistogram.getBackToBackProcessed();
    TH2D ppBackground = ppBackgroundVector[ptIndex][centralityIndex];
    TH1D phiProjection = *ppBackground.ProjectionX();

    //Finds the correct value to return
    int resultIndex = phiProjection.FindBin(x);
    double returnValue = phiProjection.GetBinContent(resultIndex);




    return returnValue;
}

