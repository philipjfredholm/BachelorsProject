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
#include <TVirtualFitter.h>


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



double fitFunction(double* values, double* parameters) {
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
    std::string throwAwayString; 
    /*
    For some odd reason ROOt needs a string as a name
    and stores that internally instead of just the object name.
    It results in memory leaks if I name a histogram the same thing twice,
    so I need a new name for every histogram. The easiest way to get
    a random throwaway garbage name is to deliberately leave a string uninitialised.
    */
    TH1D returnHistogram(throwAwayString.c_str(), "Counts", histogram.GetNbinsX(), 0, 2*TMath::Pi()); 
    double numerator;
    double denominator;
    double errorFactor;

    for (int phiBin = 1; phiBin <= histogram.GetNbinsX(); phiBin++) { //not 0 and < because of ROOT's bin convention
        numerator = 0;
        denominator = 0;
        errorFactor = 0;

        //Weighted average
        for (int etaBin = 1; etaBin <= histogram.GetNbinsY(); etaBin++) {
            if (histogram.GetBinContent(phiBin, etaBin) == 0) {
                continue;
            }
            errorFactor = 1/std::pow(histogram.GetBinError(phiBin, etaBin)/histogram.GetBinContent(phiBin, etaBin), 2);
            numerator += histogram.GetBinContent(phiBin, etaBin) * errorFactor;
            denominator += errorFactor;
            
        }

        if (denominator == 0) {continue;}
        returnHistogram.SetBinContent(phiBin, numerator/denominator);
        
        returnHistogram.SetBinError(phiBin, std::sqrt(1/denominator)*returnHistogram.GetBinContent(phiBin));

    }

    return returnHistogram; 
  
}



int main(int argc, char **argv) {
    //Argument Processing
    std::string pathToFile = argv[1];
    std::string pathToBackground = argv[2];
    std::string plotOption = argv[3];
    std::string ptIndexString = argv[4];
    std::string centralityIndexString = argv[5];
    int ptIndex = std::stoi(ptIndexString);
    int centralityIndex = std::stoi(centralityIndexString);


    std::vector<Double_t> startOfCentralityIntervals {50, 60, 65, 70, 75, 80, 85, 90};
    std::vector<Double_t> startOfPtIntervals {1, 1.5, 2, 2.5, 3, 3.5, 4, 5, 6};


    //Creates the application
    char myChar = 'a'; //ROOT requires argv but I do not want to give it.
    char* myCharPtr = &myChar;
    TRint app("app", 0, &myCharPtr);
    TCanvas canvas("canvas", "Title", 0, 0 ,800,600);


    //Reads in the data
    storeInHist dataHistograms(pathToFile);
    storeInHist backgroundHistograms(pathToBackground);
    dataHistograms.setErrors();
    backgroundHistograms.setErrors();
    dataHistograms.loadProcessed();
    backgroundHistograms.loadProcessed();

    
    if (plotOption == "forward") {
        std::vector<std::vector<TH2D>> dataVectorForward = dataHistograms.getForwardProcessed();
        std::vector<std::vector<TH2D>> backgroundVectorForward = backgroundHistograms.getForwardProcessed();

        data = projectHistogram(dataVectorForward[ptIndex][centralityIndex]);
        background = projectHistogram(backgroundVectorForward[ptIndex][0]);

    }
    if (plotOption == "backward") {
        std::vector<std::vector<TH2D>> dataVectorBackward = dataHistograms.getBackwardProcessed();
        std::vector<std::vector<TH2D>> backgroundVectorBackward = backgroundHistograms.getBackwardProcessed();

        data = projectHistogram(dataVectorBackward[ptIndex][centralityIndex]);
        background = projectHistogram(backgroundVectorBackward[ptIndex][0]);

    }
    if (plotOption == "backToBack") {
        std::vector<std::vector<TH2D>> dataVectorBackToBack = dataHistograms.getBackToBackProcessed();
        std::vector<std::vector<TH2D>> backgroundVectorBackToBack = backgroundHistograms.getBackToBackProcessed();

        data = projectHistogram(dataVectorBackToBack[ptIndex][centralityIndex]);
        background = projectHistogram(backgroundVectorBackToBack[ptIndex][0]);

    }

    if ((plotOption != "forward") && (plotOption != "backward") && (plotOption != "backToBack")) {
        throw(std::invalid_argument("Valid options for the third argument are 'forward', 'backward' and 'backToBack'!"));
    }


    //std::cout <<  data.GetBinError(10)/data.GetBinContent(10)  << std::endl;

    //Necessary for plotting
    double dataMinimum = data.GetMinimum(); 
    double dataMaximum = data.GetMaximum();
    data.SetMinimum(dataMinimum*0);
    data.SetMaximum(dataMaximum*1.1);
    TH1D dataCopy = data; 
    dataCopy.SetName("dataCopy");
    TH1D protonBackground = background; 

    
    
    //Plotting Options
    gROOT->ForceStyle();
    gStyle->SetOptStat(0);
    dataCopy.SetFillColorAlpha(kBlue, 0.5);
    

    dataCopy.GetXaxis()->CenterTitle(true);
    dataCopy.GetYaxis()->CenterTitle(true);
    dataCopy.GetXaxis()->SetTitle("#Delta #varphi");
    dataCopy.GetYaxis()->SetTitle("Scaled Counts");
    dataCopy.GetXaxis()->SetTitleSize(0.04);
    dataCopy.GetYaxis()->SetTitleSize(0.04);
    dataCopy.SetTitle("Shows the Measured Signal and the Fit");

    protonBackground.SetFillColorAlpha(kGreen, 0.2);
    protonBackground.SetTitle("protonBackground");
    protonBackground.SetLineColor(kGreen);
    
    gPad->SetGrid();
    gStyle->SetTitleFontSize(0.04);

    TGraph fourierBackground;
    fourierBackground.SetLineColor(kOrange);
    fourierBackground.SetLineWidth(2);




  
    //Fitting
    TF1 fitFunctionROOTBackground("fitFunctionROOTBackground", fitFunction, 0.0001, 2*TMath::Pi()-0.0001, 4); // +- 0001 to avoid underflow and overflow bins
    fitFunctionROOTBackground.SetParNames("Background Scale Factor", "Fourier Harmonic Scale Factor", "v2", "v3", "v4", "v5");
    fitFunctionROOTBackground.SetRange(0.0001, 2*TMath::Pi()-0.0001);
    
    fitFunctionROOTBackground.SetParameters(1, 1, 0.05, 0.005);
    fitFunctionROOTBackground.SetParLimits(2, 0, 1);
    fitFunctionROOTBackground.SetParLimits(3, 0, 1);
    for (int n = 0; n < 4; n++) {
        fitFunctionROOTBackground.SetParError(n, 0);
    }



    dataCopy.Fit("fitFunctionROOTBackground", "R same");


    
    double protonScale = fitFunctionROOTBackground.GetParameter(0);
    double fourierScale = fitFunctionROOTBackground.GetParameter(1);
    std::cout << fourierScale << std::endl;
    double v2 = fitFunctionROOTBackground.GetParameter(2);
    //double v3 = fitFunctionROOTBackground.GetParameter(3);
    

    protonBackground = protonScale * protonBackground;
    int numberOfPoints = 5000;
    double stepLength = 2*TMath::Pi()/numberOfPoints;
    double fourierValue;


    for (int position = 0; position < numberOfPoints; position ++) {
        fourierValue = protonScale*ppHistogramValue(stepLength*position) + fourierScale*(1+2*v2*std::cos(2*stepLength*position));//+2*v3*std::cos(3*stepLength*position)); 
        fourierBackground.AddPoint(stepLength*position, fourierValue);
    }



    dataCopy.Draw("HIST same");
    protonBackground.Draw("HIST same");
    fourierBackground.Draw("same");


    TLegend myLegend(0.62, 0.7, 0.82, 0.9);
    myLegend.AddEntry(&data, "Measured Data", "l");
    myLegend.AddEntry(&fitFunctionROOTBackground, "#splitline{Full Fit}{#mbox{}}", "l");
    myLegend.AddEntry(&fourierBackground, "#splitline{pp Background +}{Only Elliptic Flow}", "l");
    myLegend.SetTextSize(0.03);
    

    myLegend.AddEntry(&protonBackground, "pp Background", "l");
    myLegend.Draw("same");   



    //Runs the application
    std::string filename = "fitExamplePT" + std::to_string(startOfPtIntervals[ptIndex]).substr(0,4) + "Centrality" + std::to_string(startOfCentralityIntervals[centralityIndex]).substr(0,4) + ".pdf";
    canvas.Print(filename.c_str());
    canvas.Modified(); 
    canvas.Update();
    app.Run();
    (void)argc;


}





