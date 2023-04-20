//Core C++
#include <iostream>
#include <string>
#include <vector>
#include <thread>
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
    
    TRint app("app", 0, &myCharPtr);
    TCanvas canvas("canvas", "Title", 0, 0 ,800,600);


    //Reads in the histograms
    std::string pathToFile = argv[1];
    std::string drawOption = argv[2];
    std::string drawStyle = argv[3];
    std::string ptRegionString = argv[4];
    int ptRegion= std::stoi(ptRegionString);
    std::string centralityRegionString = argv[5]; 
    int centralityRegion = std::stoi(centralityRegionString);
    

    if (drawStyle == "") {
        drawStyle = "surf1";
    }


    storeInHist myHistogram(0); //The number '0' is random, I needed to change the signature since TObject already does a default constructor and I need another one here.
    for (const auto& file : std::filesystem::directory_iterator(pathToFile)) {
        std::string filename = file.path();
        if (filename == "totalDataProcessed.root") {
            continue;
        }
        storeInHist dummyHistogram (file.path());
        myHistogram.addHistograms(dummyHistogram);


    } 


    //Stores the histogram
    myHistogram.setStorageName("totalData");
    myHistogram.storeHistogramInFile();


    //Plots the histogram if the proper argument has been given
    if (drawOption == "forward") {
        std::vector<std::vector<TH2D>> histogramForwardvector = myHistogram.getForwardHistograms();
        TH2D histogramForward = histogramForwardvector[ptRegion][centralityRegion];
        histogramForward.Draw(drawStyle.c_str());

        canvas.Modified(); 
        canvas.Update();
        app.Run();


    }

    if (drawOption == "forwardBackground") {
        std::vector<std::vector<TH2D>> histogramForwardvector = myHistogram.getForwardBackgrounds();
        TH2D histogramForward = histogramForwardvector[ptRegion][centralityRegion];
        histogramForward.Draw(drawStyle.c_str());

        canvas.Modified(); 
        canvas.Update();
        app.Run();


    }

    if (drawOption == "backward") {
        std::vector<std::vector<TH2D>> histogramBackwardvector = myHistogram.getBackwardHistograms();
        TH2D histogramBackward = histogramBackwardvector[ptRegion][centralityRegion];
        histogramBackward.Draw(drawStyle.c_str());

        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }

    if (drawOption == "backwardBackground") {
        std::vector<std::vector<TH2D>> histogramBackwardvector = myHistogram.getBackwardBackgrounds();
        TH2D histogramBackward = histogramBackwardvector[ptRegion][centralityRegion];
        histogramBackward.Draw(drawStyle.c_str());

        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }



    if (drawOption == "backToBack") {
        std::vector<std::vector<TH2D>> histogramBackToBackvector = myHistogram.getBackToBackHistograms();
        TH2D histogramBackToBack = histogramBackToBackvector[ptRegion][centralityRegion];
        histogramBackToBack.Draw(drawStyle.c_str());

        canvas.Modified(); 
        canvas.Update();
        app.Run();
    }



    if (drawOption == "backToBackBackground") {
        std::vector<std::vector<TH2D>> histogramBackToBackvector = myHistogram.getBackToBackBackgrounds();
        TH2D histogramBackToBack = histogramBackToBackvector[ptRegion][centralityRegion];
        histogramBackToBack.Draw(drawStyle.c_str());
        

        canvas.Modified(); 
        canvas.Update();
        app.Run();
    }







    if (drawOption == "none") {
        //Does nothing
    }

    if (drawOption == "all") {
        std::vector<std::vector<TH2D>> histogramForwardvector = myHistogram.getForwardHistograms();
        std::vector<std::vector<TH2D>> histogramBackwardvector = myHistogram.getBackwardHistograms();
        //std::vector<std::vector<TH2D>> histogramBackToBackvector = myHistogram.getBackToBackHistograms();

        TH2D histogramForward = histogramForwardvector[ptRegion][centralityRegion];
        TH2D histogramBackward = histogramBackwardvector[ptRegion][centralityRegion];
        //TH2D histogramBackToBack = histogramBackToBackvector[0][centralityRegion];
        
        
        histogramForward.Add(&histogramBackward);
        //histogramForward.Add(&histogramBackToBack);
        histogramForward.Draw(drawStyle.c_str());


        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }

    if (drawOption == "allBackground") {
        std::vector<std::vector<TH2D>> histogramForwardvector = myHistogram.getForwardBackgrounds();
        std::vector<std::vector<TH2D>> histogramBackwardvector = myHistogram.getBackwardBackgrounds();
        std::vector<std::vector<TH2D>> histogramBackToBackvector = myHistogram.getBackToBackBackgrounds();
        
        TH2D histogramForward = histogramForwardvector[ptRegion][centralityRegion];
        TH2D histogramBackward = histogramBackwardvector[ptRegion][centralityRegion];
        TH2D histogramBackToBack = histogramBackToBackvector[0][centralityRegion];
        
        
        histogramForward.Add(&histogramBackward);
        histogramForward.Add(&histogramBackToBack);
        histogramForward.Draw(drawStyle.c_str());


        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }





    if (drawOption == "allNormalised") {
        myHistogram.loadProcessed();
        std::vector<std::vector<TH2D>> histogramForwardvector = myHistogram.getForwardProcessed();
        std::vector<std::vector<TH2D>> histogramBackwardvector = myHistogram.getBackwardProcessed();
        std::vector<std::vector<TH2D>> histogramBackToBackvector = myHistogram.getBackToBackProcessed();

        TH2D histogramForward = histogramForwardvector[ptRegion][centralityRegion];
        TH2D histogramBackward = histogramBackwardvector[ptRegion][centralityRegion];
        TH2D histogramBackToBack = histogramBackToBackvector[0][centralityRegion];

        histogramForward.Add(&histogramBackward);
        histogramForward.Add(&histogramBackToBack);
        histogramForward.Draw(drawStyle.c_str());


        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }

    if (drawOption == "forwardNormalised") {
        myHistogram.loadProcessed();
        std::vector<std::vector<TH2D>> histogramvector = myHistogram.getForwardProcessed();
        TH2D histogram = histogramvector[ptRegion][centralityRegion];
        
        
        histogram.Draw(drawStyle.c_str());

        
        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }
    

    if (drawOption == "backwardNormalised") {
        myHistogram.loadProcessed();
        std::vector<std::vector<TH2D>> histogramvector = myHistogram.getBackwardProcessed();
        TH2D histogram = histogramvector[ptRegion][centralityRegion];
        
        histogram.Draw(drawStyle.c_str());

        
        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }

    if (drawOption == "backToBackNormalised") {
        myHistogram.loadProcessed();
        std::vector<std::vector<TH2D>> histogramvector = myHistogram.getBackToBackProcessed();
        TH2D histogram = histogramvector[0][centralityRegion];

        
        histogram.Draw(drawStyle.c_str());

        
        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }
    
    

    if (drawOption == "forwardFinished") {
        myHistogram.loadProcessed();
        std::vector<std::vector<TH2D>> histogramvector = myHistogram.getForwardProcessed();
        TH2D histogram = histogramvector[ptRegion][centralityRegion];

        TH1D phiProjection = *histogram.ProjectionX();
        
        
        phiProjection.SetTitle("TPC-ForwardFMD");
        phiProjection.Draw();

        
        
        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }


    if (drawOption == "backwardFinished") {
        myHistogram.loadProcessed();
        std::vector<std::vector<TH2D>> histogramvector = myHistogram.getBackwardProcessed();
        TH2D histogram = histogramvector[ptRegion][centralityRegion];

        TH1D phiProjection = *histogram.ProjectionX();
        phiProjection.Draw();
        phiProjection.SetTitle("TPC-BackwardFMD");

        
        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }

    if (drawOption == "backToBackFinished") {
        myHistogram.loadProcessed();
        std::vector<std::vector<TH2D>> histogramvector = myHistogram.getBackToBackProcessed();
        TH2D histogram = histogramvector[0][centralityRegion];

        TH1D phiProjection = *histogram.ProjectionX();
        phiProjection.SetTitle("FMD-FMD");
        phiProjection.Draw();

        
        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }


    (void)argc;

}


