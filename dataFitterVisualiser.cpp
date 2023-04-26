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





int main(int argc, char **argv) {
    //Argument Processing
    std::string pathToFile = argv[1];
    std::string pathToBackground = argv[2];
    std::string ptIndexString = argv[3];
    std::string centralityIndexString = argv[4];
    int ptIndex = std::stoi(ptIndexString);
    int centralityIndex = std::stoi(centralityIndexString);

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

    
    TH2D dataTemp = dataVectorForward[ptIndex][centralityIndex];
    TH2D backgroundTemp = backgroundVectorForward[ptIndex][0];
    
    data = *dataTemp.ProjectionX();
    background = *backgroundTemp.ProjectionX();

    // Necessary for plotting
    double dataMinimum = data.GetMinimum(); 
    TH1D dataCopy = data; 
    dataCopy.SetName("dataCopy");
    TH1D protonBackground = background; 

    
    


    //Plotting
    gROOT->ForceStyle();
    gStyle->SetOptStat(0);
    dataCopy.SetFillColorAlpha(kBlue, 0.5);
    
    
    data.GetXaxis()->CenterTitle(true);
    data.GetYaxis()->CenterTitle(true);
    data.GetXaxis()->SetTitle("#Delta #varphi");
    data.GetYaxis()->SetTitle("Scaled Counts");
    data.GetXaxis()->SetTitleSize(0.04);
    data.GetYaxis()->SetTitleSize(0.04);
    data.SetTitle("Shows the Measured Signal and the Fit");
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
    fourierBackground.SetFillColorAlpha(kOrange, 0.2);
    fourierBackground.SetLineColor(kOrange);

    TGraph fourierBackgroundv3;
    fourierBackgroundv3.SetFillColorAlpha(kMagenta, 0.2);
    fourierBackgroundv3.SetLineColor(kMagenta);
    


    //Fitting
    TF1 fitFunctionROOT("fitFunctionROOT", fitFunction, 0.0001, 2*TMath::Pi()-0.0001, 4); // +- 0001 to avoid underflow and overflow bins
    TF1 fitFunctionROOTBackground("fitFunctionROOTBackground", fitFunction, 0.0001, 2*TMath::Pi()-0.0001, 4); // +- 0001 to avoid underflow and overflow bins
    fitFunctionROOT.SetParNames("Background Scale Factor", "Fourier Harmonic Scale Factor", "v2", "v3", "v4", "v5");
    


    fitFunctionROOT.SetParameter(0, 1); // Proton background initial guess
    fitFunctionROOT.SetParameter(1, 1); // Fourier harmonics initial guess
    fitFunctionROOT.SetParameter(2 ,0.0005); // v2 initial guess
    fitFunctionROOT.SetParameter(3, 0.0001); // v3 initial guess
 
    
    dataCopy.Fit("fitFunctionROOTBackground", "RQ0 same");


    
    double protonScale = fitFunctionROOTBackground.GetParameter(0);
    double fourierScale = fitFunctionROOTBackground.GetParameter(1);
    std::cout << fourierScale << std::endl;
    double v2 = fitFunctionROOTBackground.GetParameter(2);
    double v3 = fitFunctionROOTBackground.GetParameter(3);
    

    protonBackground = protonScale * protonBackground;
    double backgroundMinimum = protonBackground.GetMinimum();
    int backgroundBins = protonBackground.GetNbinsX();
  
    
    for (int bin = 0; bin <= backgroundBins; bin++) {
        protonBackground.AddBinContent(bin ,dataMinimum - backgroundMinimum);
    }


    
    int numberOfPoints = 1000;
    double stepLength = 2*TMath::Pi()/1000;
    double fourierValue;
    double fourierValuev3;

    for (int position = 0; position < numberOfPoints; position ++) {
        fourierValue = protonScale*ppHistogramValue(stepLength*position) + fourierScale*(1+2*v2*std::cos(2*stepLength*position)); 
        fourierValuev3 = fourierValue + fourierScale*(2*v3*std::cos(3*position*stepLength));
        fourierBackground.AddPoint(stepLength*position, fourierValue);
        fourierBackgroundv3.AddPoint(stepLength*position, fourierValuev3-0.1);
    }

    fourierBackground.SetPoint(0,0, 0);
    fourierBackground.SetPoint(1000, 2*TMath::Pi()+2, 0);
    
    fourierBackgroundv3.SetPoint(0,0, 0);
    fourierBackgroundv3.SetPoint(1000, 2*TMath::Pi()+2, 0);
    

    

    dataCopy.Draw("HIST same");
    protonBackground.Draw("HIST same");
    fourierBackground.Draw("F same");
    //fourierBackgroundv3.Draw("F same");
    
    
    data.Draw("same ");
    
    
    data.Fit("fitFunctionROOT", "R same");
    

    
    TLegend myLegend(0.62, 0.7, 0.82, 0.9);
    myLegend.AddEntry(&data, "Measured Data", "l");
    myLegend.AddEntry(&fitFunctionROOT, "Full Fit", "l");
    myLegend.AddEntry(&fourierBackground, "pp Background + Elliptic Flow Note y axis", "l");
    myLegend.SetTextSize(0.03);
    

    myLegend.AddEntry(&protonBackground, "pp Background", "l");
    myLegend.Draw("same");   



    //Runs the application
    canvas.Print("test2.pdf");
    canvas.Modified(); 
    canvas.Update();
    app.Run();
    (void)argc;


}





