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



double v2Extractor(double* values, double* parameters) {

    int numberOfHarmonics = sizeof(parameters)-2;
    double backgroundNumber = parameters[0]*ppHistogramValue(values[0]);
    double ellipticFlow = 1 ;
    for (int harmonic = 2; harmonic < numberOfHarmonics; harmonic++) {
        ellipticFlow += 2*parameters[harmonic]*std::cos(harmonic*values[0]);
    }
    ellipticFlow *= parameters[1];


    
    return backgroundNumber+ellipticFlow;

} 





TH1D projectHistogram(TH2D histogram) {
    TH1D returnHistogram("projectedPhi", "Counts", histogram.GetNbinsX(), 0, 2*TMath::Pi()); 

    double numerator;
    double denominator;
    double errorFactor = 1;


    for (int phiBin = 1; phiBin <= histogram.GetNbinsX(); phiBin++) { //not 0 and < because of ROOT's bin convention
        numerator = 0;
        denominator = 0;

        //Weighted average
        for (int etaBin = 1; etaBin <= histogram.GetNbinsY(); etaBin++) {
            errorFactor = std::pow(histogram.GetBinError(phiBin, etaBin), 2);
            numerator += histogram.GetBinContent(phiBin, etaBin) * errorFactor;
            denominator += errorFactor;
            
        }

        returnHistogram.SetBinContent(phiBin, numerator/denominator);
        returnHistogram.SetBinError(phiBin, std::sqrt(denominator));


    }

    return returnHistogram; 
    
}




int main(int argc, char **argv) {
    //Argument Processing
    std::string pathToFile = argv[1];
    std::string pathToBackground = argv[2];

    std::vector<double> startOfCentralityIntervals {50, 60, 65, 70, 75, 80, 85, 90};
    std::vector<double> startOfPtIntervals {1, 1.5, 2, 2.5, 3, 3.5, 4, 5, 6};

    //Creates the application
    char myChar = 'a'; //ROOT requires argv but I do not want to give it.
    char* myCharPtr = &myChar;
    TRint app("app", 0, &myCharPtr);
    TCanvas canvas("canvas", "Title", 0, 0 ,800,600);


    //Reads in the data
    storeInHist dataHistograms(pathToFile);
    storeInHist backgroundHistograms(pathToBackground);
    dataHistograms.setErrors(); //Calls Sumw2 for all of the histograms.
    backgroundHistograms.setErrors(); //This leads to printed error messages of the sum of weights structure 
                                            //already having been created, but I was told to ignore this warning. 

    dataHistograms.loadProcessed();
    backgroundHistograms.loadProcessed();


    std::vector<std::vector<TH2D>> dataVectorForward = dataHistograms.getForwardProcessed();
    std::vector<std::vector<TH2D>> backgroundVectorForward = backgroundHistograms.getForwardProcessed();
    std::vector<std::vector<TH2D>> dataVectorBackward = dataHistograms.getBackwardProcessed();
    std::vector<std::vector<TH2D>> backgroundVectorBackward = backgroundHistograms.getBackwardProcessed();
    std::vector<std::vector<TH2D>> dataVectorBackToBack = dataHistograms.getBackToBackProcessed();
    std::vector<std::vector<TH2D>> backgroundVectorBackToBack = backgroundHistograms.getBackToBackProcessed();



    //Vectors for storing the results    
    std::vector<std::vector<double>> v2ForwardList;
    std::vector<std::vector<double>> v2BackwardList;
    std::vector<std::vector<double>> v2BackToBackList;

    std::vector<std::vector<double>> v2ErrorForwardList;
    std::vector<std::vector<double>> v2ErrorBackwardList;
    std::vector<std::vector<double>> v2ErrorBackToBackList;

    std::vector<double> placeHolder;



    
    //Defines the fitting function
    TF1 v2Finder("v2", v2Extractor, 0, 2*TMath::Pi()-0.0001, 4);
    v2Finder.SetParNames("Background Amplitude", "Scaling", "v2", "v3", "v4", "v5", "v6", "v7");
    v2Finder.SetParameter(2,0.5); //Initial Guess
    v2Finder.SetParameter(2,0.05); //Initial Guess
    v2Finder.SetParLimits(2, 0, 1);
    v2Finder.SetParLimits(3, 0, 1);


    //Loops over all different cases
    TH2D dataTemp;
    TH2D backgroundTemp;
    double v2;
    double v2Error;
    
    for (int ptNumber = 0; ptNumber < static_cast<int>(dataVectorForward.size()); ptNumber++) {
        v2ForwardList.push_back(placeHolder);
        v2BackwardList.push_back(placeHolder);

        v2ErrorForwardList.push_back(placeHolder);
        v2ErrorBackwardList.push_back(placeHolder);

        if (ptNumber == 0) {
            v2BackToBackList.push_back(placeHolder);
            v2ErrorBackToBackList.push_back(placeHolder);
        }


        for (int centralityNumber = 0; centralityNumber < static_cast<int>(dataVectorForward[0].size()); centralityNumber++) {  
            //Forward
            dataTemp = dataVectorForward[ptNumber][centralityNumber];
            backgroundTemp = backgroundVectorForward[ptNumber][0];
            data = projectHistogram(dataTemp);
            background = projectHistogram(backgroundTemp);
            
            data.Fit("v2", "RQ0");
            v2 = v2Finder.GetParameter(2);
            v2Error = v2Finder.GetParError(2);
        

            v2ForwardList[ptNumber].push_back(v2);
            v2ErrorForwardList[ptNumber].push_back(v2Error);

            //Backward
            dataTemp = dataVectorBackward[ptNumber][centralityNumber];
            backgroundTemp = backgroundVectorBackward[ptNumber][0];
            data = projectHistogram(dataTemp);
            background = projectHistogram(backgroundTemp);
            
            data.Fit("v2", "RQ0");
            v2 = v2Finder.GetParameter(2);
            v2Error = v2Finder.GetParError(2);
            

            v2BackwardList[ptNumber].push_back(v2);
            v2ErrorBackwardList[ptNumber].push_back(v2Error);

            //BackToBack
            if (ptNumber == 0 ) {
    
                dataTemp = dataVectorBackToBack[0][centralityNumber];
                backgroundTemp = backgroundVectorBackToBack[ptNumber][0];
                data = projectHistogram(dataTemp);
                background = projectHistogram(backgroundTemp);
                
                data.Fit("v2", "RQ0");
                v2 = v2Finder.GetParameter(2);
                v2Error = v2Finder.GetParError(2);
                

                v2BackToBackList[ptNumber].push_back(v2);
                v2ErrorBackToBackList[ptNumber].push_back(v2Error);
            }


        }
    }


    //Saves the data
    TFile results("results.root", "RECREATE");
    results.WriteObject(&v2ForwardList, "v2ForwardList");
    results.WriteObject(&v2BackwardList, "v2BackwardList");
    results.WriteObject(&v2BackToBackList, "v2BackToBackList");

    results.WriteObject(&v2ErrorForwardList, "v2ErrorForwardList");
    results.WriteObject(&v2ErrorBackwardList, "v2ErrorBackwardList");
    results.WriteObject(&v2ErrorBackToBackList, "v2ErrorBackToBackList");

    results.WriteObject(&startOfCentralityIntervals, "startOfCentralityIntervals");
    results.WriteObject(&startOfPtIntervals, "startOfPtIntervals");




    results.Close();
    //Runs the application
    canvas.Modified(); 
    canvas.Update();
    app.Run();
    (void)argc;


}






