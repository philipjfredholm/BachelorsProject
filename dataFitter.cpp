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
#include <TF1.h>
#include <TLegend.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TSystem.h>


/* Purpose:
This program is intended to able to extract the the various flow harmonic from
correlations taken from the lead-lead (PbPb) collisions by fitting them to Fourier harmonics
and background from proton-proton (pp) collisions to account for non flow effects of 
QCD.
 */



TH1D data;
TH1D background;



double ppHistogramValue(Double_t x) {
    //Check that the input is valid
    if ((x < 0) || (x > 2*TMath::Pi())) {
        throw(std::invalid_argument("The value must be between 0 and 2pi"));
    }



    //Finds the correct value to return
    int resultIndex = background.FindBin(x);
    double returnValue = background.GetBinContent(resultIndex);




    return returnValue;
}



double v2Extractordouble* values, double* parameters) {

    int numberOfHarmonics = sizeof(parameters)-2;
    double backgroundNumber = parameters[0]*ppHistogramValue(values[0]);
    double ellipticFlow = 1 ;
    for (int harmonic = 2; harmonic < numberOfHarmonics; harmonic++) {
        ellipticFlow += 2*parameters[harmonic]*std::cos(harmonic*values[0]);
    }
    ellipticFlow *= parameters[1];


    
    return backgroundNumber+ellipticFlow;

} 





int main(int argc, char **argv) {
    //Argument Processing
    std::string pathToFile = argv[1];
    std::string pathToBackground = argv[2];
/*     std::string ptIndexString = argv[3];
    std::string centralityIndexString = argv[4]; 
    int ptIndex = std::stoi(ptIndexString);
    int centralityIndex = std::stoi(centralityIndexString);
 */
    //Creates the application
    char myChar = 'a'; //ROOT requires argv but I do not want to give it.
    char* myCharPtr = &myChar;
    TRint app("app", 0, &myCharPtr);
    TCanvas canvas("canvas", "Title", 0, 0 ,800,600);


    //Reads in the data
    storeInHist dataHistograms(pathToFile);
    storeInHist backgroundHistograms(pathToBackground);
    dataHistograms.loadProcessed();
    backgroundHistograms.loadProcessed();

    //std::vector<Double_t> startOfCentralityIntervals {50, 60, 65, 70, 75, 80, 85, 90};
    //std::vector<Double_t> startOfPtIntervals {1, 1.5, 2, 2.5, 3, 3.5, 4, 5, 6};
    

    std::vector<std::vector<TH2D>> dataVectorForward = dataHistograms.getForwardProcessed();
    std::vector<std::vector<TH2D>> backgroundVectorForward = backgroundHistograms.getForwardProcessed();
    std::vector<std::vector<TH2D>> dataVectorBackward = dataHistograms.getBackwardProcessed();
    std::vector<std::vector<TH2D>> backgroundVectorBackward = backgroundHistograms.getBackwardProcessed();
    std::vector<std::vector<TH2D>> dataVectorBackToBack = dataHistograms.getBackToBackProcessed();
    std::vector<std::vector<TH2D>> backgroundVectorBackToBack = backgroundHistograms.getBackToBackProcessed();

    std::vector<std::vector<double>> v2List;


    TF1 v2Finder("v2", v2Extractor, 0, 2*TMath::Pi()-0.0001, 4);
    v2Finder.SetParNames("Background Amplitude", "Scaling", "v2", "v3", "v4", "v5");
    v2Finder.SetParameter(2,0.5); //Initial Guess


    data.GetXaxis()->CenterTitle(true);
    data.GetYaxis()->CenterTitle(true);
    data.GetXaxis()->SetTitle("#Delta #varphi");
    data.GetYaxis()->SetTitle("Scaled Counts");
    data.GetXaxis()->SetTitleSize(0.04);
    data.GetYaxis()->SetTitleSize(0.04);

    data.SetTitle("#splitline{Shows the scaled down number of}{     counts as a function of #Delta#varphi}");
    
    gPad->SetGrid();
    gStyle->SetTitleFontSize(0.04);
    
    
    

    TLegend myLegend(0.62, 0.7, 0.82, 0.9);
    myLegend.AddEntry(&data, "Measured Data", "l");
    myLegend.AddEntry(&v2Finder, "Fit", "l");
    myLegend.SetTextSize(0.03);
    

    std::cout << "loop" << std::endl;
    for (int centralityNumber = 0; centralityNumber < static_cast<int>(dataVectorForward[0].size())-2; centralityNumber++) {
        for (int ptNumber = 0; ptNumber < static_cast<int>(dataVectorForward.size())-2; ptNumber++) {
            TH2D dataTemp = dataVectorForward[ptNumber][centralityNumber];
            TH2D backgroundTemp = backgroundVectorForward[ptNumber][0];
    
            data = *dataTemp.ProjectionX();
            background = *backgroundTemp.ProjectionX();
            
            data.Fit("v2", "RQ");


            double v2 = v2Finder.GetParameter(2);
            std::cout << "v2 is " << v2 << std::endl;
            std::cout << "square root is" << std::sqrt(v2) << std::endl;

        }
    }




    data.SetStats(0);
    data.Draw();
    myLegend.Draw();
    




    //Runs the application
    canvas.Modified(); 
    canvas.Update();
    app.Run();
    (void)argc;


}






