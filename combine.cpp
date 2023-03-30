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







int main(int argc, char **argv) {
    //Makes the application
    char myChar = 'a'; //ROOT requires argv but I do not want to give it.
    char* myCharPtr = &myChar;
    
    TApplication app("app", 0, &myCharPtr);
    TCanvas canvas("canvas", "Title", 0, 0 ,800,600);



    //Reads in the histograms
    std::string pathToFile = argv[1];
    std::string drawOption = argv[2];


    storeInHist myHistogram(0); //The numbers are random, I needed to change the signature since TObject already does a default constructor.
    for (const auto& file : std::filesystem::directory_iterator(pathToFile)) {
        
        std::string filename = file.path();
        if (filename == "totalDataProcessed.root") {
            continue;
        }
        storeInHist dummyHistogram (file.path());
        myHistogram.addHistogram(dummyHistogram);


    } 


    //Stores the histogram
    myHistogram.setStorageName("totalData");
    myHistogram.storeHistogramInFile();


    //Plots the histogram if the proper argument has been given
    if (drawOption == "forward") {
        TH2D histogramForward = myHistogram.getForwardHistogram();
        histogramForward.Draw("colz");

        canvas.Modified(); 
        canvas.Update();
        app.Run();


    }

    if (drawOption == "backward") {
        TH2D histogramBackward = myHistogram.getBackwardHistogram();
        histogramBackward.Draw("colz");

        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }

    if (drawOption == "backToBack") {
        TH2D histogramBackToBack = myHistogram.getBackToBackHistogram();
        histogramBackToBack.Draw("colzs");

        canvas.Modified(); 
        canvas.Update();
        app.Run();
    }

    if (drawOption == "none") {

    }

    if (drawOption == "all") {
        TH2D histogramForward = myHistogram.getForwardHistogram();
        TH2D histogramBackward = myHistogram.getBackwardHistogram();
        histogramForward.Add(&histogramBackward);
        TH2D histogramBackToBack = myHistogram.getBackToBackHistogram();
        histogramForward.Add(&histogramBackToBack);
        histogramForward.Draw("surf1");


        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }

    if (drawOption == "background") {
        TH2D histogramForward = myHistogram.getForwardBackground();

        histogramForward.Draw("surf1");


        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }
    





    (void)argc;

}


