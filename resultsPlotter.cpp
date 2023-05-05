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
#include <TGraphErrors.h>
#include <TStyle.h>
#include <TSystem.h>



int main(int argc, char** argv) {
    //Argument processing
    std::string pathToFile = argv[1];
    std::string centralityIntervalString = argv[2];
    int centralityInterval = std::stoi(centralityIntervalString);


    //Creates the application
    char myChar = 'a'; //ROOT requires argv but I do not want to give it.
    char* myCharPtr = &myChar;
    TRint app("app", 0, &myCharPtr);
    TCanvas canvas("canvas", "Title", 0, 0 ,800,600);

    //Reads in the data
    TFile dataset1(pathToFile.c_str(), "READ");

    std::vector<std::vector<double>> v2ForwardList = *(std::vector<std::vector<double>>*)dataset1.Get("v2ForwardList");
    std::vector<std::vector<double>> v2BackwardList = *(std::vector<std::vector<double>>*)dataset1.Get("v2BackwardList");;
    std::vector<std::vector<double>> v2BackToBackList = *(std::vector<std::vector<double>>*)dataset1.Get("v2BackToBackList");

    std::vector<std::vector<double>> v2ErrorForwardList = *(std::vector<std::vector<double>>*)dataset1.Get("v2ErrorForwardList");
    std::vector<std::vector<double>> v2ErrorBackwardList = *(std::vector<std::vector<double>>*)dataset1.Get("v2ErrorBackwardList");;
    std::vector<std::vector<double>> v2ErrorBackToBackList = *(std::vector<std::vector<double>>*)dataset1.Get("v2ErrorBackToBackList");

    std::vector<double> startOfCentralityIntervals = *(std::vector<double>*)dataset1.Get("startOfCentralityIntervals");
    std::vector<double> startOfPtIntervals = *(std::vector<double>*)dataset1.Get("startOfPtIntervals");

    dataset1.Close();

    if (centralityInterval > static_cast<int>(startOfCentralityIntervals.size())-2) {
        throw std::invalid_argument("That centrality interval is not available!");
    }


    //Calculates the final values
    std::vector<std::vector<double>> v2FinalList;
    std::vector<std::vector<double>> v2ErrorFinalList;
    std::vector<double> placeHolder;

    double v2Forward;
    double v2Backward;
    double v2BackToBack;

    double v2ErrorForward;
    double v2ErrorBackward;
    double v2ErrorBackToBack;

    double v2Final;
    double v2ErrorFinal;


    //Calculates the actual v2 values
    for (int ptNumber = 0; ptNumber < static_cast<int>(startOfPtIntervals.size()) - 1; ptNumber++) {
        v2FinalList.push_back(placeHolder);
        v2ErrorFinalList.push_back(placeHolder);

        for (int centralityNumber = 0; centralityNumber < static_cast<int>(startOfCentralityIntervals.size()) - 1; centralityNumber++) {
            v2Forward = v2ForwardList[ptNumber][centralityNumber];
            v2Backward = v2BackwardList[ptNumber][centralityNumber];
            v2BackToBack = v2BackToBackList[0][centralityNumber];
            
            v2ErrorForward = v2ErrorForwardList[ptNumber][centralityNumber];
            v2ErrorBackward = v2ErrorBackwardList[ptNumber][centralityNumber];
            v2ErrorBackToBack = v2ErrorBackToBackList[0][centralityNumber];
            
            //Final value and error propagation
            v2Final = std::sqrt(v2Forward*v2Backward/v2BackToBack);
            v2ErrorFinal = 0.5*v2Final*std::sqrt(std::pow(v2ErrorForward/v2Forward, 2)+std::pow(v2ErrorBackward/v2Backward, 2)+std::pow(v2ErrorBackToBack/v2BackToBack, 2));

            v2FinalList[ptNumber].push_back(v2Final);
            v2ErrorFinalList[ptNumber].push_back(v2ErrorFinal);


        }

    }

    

    //Plotting
    TGraphErrors finalPlot;
    finalPlot.SetName("finalPlot");
    finalPlot.SetTitle("Shows values of #it{v_{2}} for various values of #it{p_{T}}; #it{p_{T}} (GeV); #it{v_{2}}");


    int counter = 0;
    double ptMid;
    double ptDiff;
    double v2value;
    double v2ErrorValue;

    for (int ptNumber = 0; ptNumber < static_cast<int>(startOfPtIntervals.size()) - 1; ptNumber++) {
        ptMid = (startOfPtIntervals[ptNumber+1] + startOfPtIntervals[ptNumber])/2;
        ptDiff = (startOfPtIntervals[ptNumber+1] - startOfPtIntervals[ptNumber])/2;

        v2value = v2FinalList[ptNumber][centralityInterval];
        v2ErrorValue = v2ErrorFinalList[ptNumber][centralityInterval];

        finalPlot.AddPoint(ptMid, v2value);
        finalPlot.SetPointError(counter, ptDiff, v2ErrorValue);

        counter++;


        

        

    }




    gPad->SetGrid();
    gStyle->SetCanvasPreferGL(kTRUE);
    finalPlot.SetMarkerColor(2);
    finalPlot.SetMarkerStyle(8);
    finalPlot.SetFillColorAlpha(kRed,0.4);

    
 
    TLegend myLegend(0.6, 0.70, 0.87, 0.85);
    std::string centralityStart = std::to_string(startOfCentralityIntervals[centralityInterval]);
    centralityStart = centralityStart.substr(0,2);
    std::string centralityStop = std::to_string(startOfCentralityIntervals[centralityInterval+1]);
    centralityStop = centralityStop.substr(0,2);
    std::string stringCentralityIntervalString2 =centralityStart+"% - "+centralityStop + "% Centrality";
    myLegend.AddEntry(&finalPlot, stringCentralityIntervalString2.c_str(), "pf");
    finalPlot.Draw("AP2");
    myLegend.Draw();


    
    std::string filename = "resultsCentrality" + std::to_string(startOfCentralityIntervals[centralityInterval]).substr(0,4) + ".pdf";
    gPad->Print(filename.c_str());

    //Runs the application
    canvas.Modified(); 
    canvas.Update();
    app.Run();



    (void)argc;
    (void)argv;
}