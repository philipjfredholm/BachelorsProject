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
    
    TRint app("app", 0, &myCharPtr);
    TCanvas canvas("canvas", "Title", 0, 0 ,800,600);


    //Reads in the histograms
    std::string pathToFile = argv[1];
    std::string drawOption = argv[2];
    std::string drawStyle = argv[3];

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
        myHistogram.addHistogram(dummyHistogram);


    } 


    //Stores the histogram
    myHistogram.setStorageName("totalData");
    myHistogram.storeHistogramInFile();


    //Plots the histogram if the proper argument has been given
    if (drawOption == "forward") {
        TH2D histogramForward = myHistogram.getForwardHistogram();
        histogramForward.Draw(drawStyle.c_str());

        canvas.Modified(); 
        canvas.Update();
        app.Run();


    }

    if (drawOption == "forwardBackground") {
        TH2D histogramForward = myHistogram.getForwardBackground();
        histogramForward.Draw(drawStyle.c_str());

        canvas.Modified(); 
        canvas.Update();
        app.Run();


    }

    if (drawOption == "backward") {
        TH2D histogramBackward = myHistogram.getBackwardHistogram();
        histogramBackward.Draw(drawStyle.c_str());

        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }

    if (drawOption == "backwardBackground") {
        TH2D histogramBackward = myHistogram.getBackwardBackground();
        histogramBackward.Draw(drawStyle.c_str());

        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }



    if (drawOption == "backToBack") {
        TH2D histogramBackToBack = myHistogram.getBackToBackHistogram();
        histogramBackToBack.Draw(drawStyle.c_str());

        canvas.Modified(); 
        canvas.Update();
        app.Run();
    }



    if (drawOption == "backToBackBackground") {
        TH2D histogramBackToBack = myHistogram.getBackToBackBackground();
        histogramBackToBack.Draw(drawStyle.c_str());

        canvas.Modified(); 
        canvas.Update();
        app.Run();
    }







    if (drawOption == "none") {
        //Does nothing
    }

    if (drawOption == "all") {
        TH2D histogramForward = myHistogram.getForwardHistogram();
        TH2D histogramBackward = myHistogram.getBackwardHistogram();
        TH2D histogramBackToBack = myHistogram.getBackToBackHistogram();
        
        
        histogramForward.Add(&histogramBackward);
        histogramForward.Add(&histogramBackToBack);
        histogramForward.Draw(drawStyle.c_str());


        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }

    if (drawOption == "allBackground") {
        TH2D histogramForward = myHistogram.getForwardBackground();
        TH2D histogramBackward = myHistogram.getBackwardBackground();
        TH2D histogramBackToBack = myHistogram.getBackToBackBackground();
        
        
        histogramForward.Add(&histogramBackward);
        histogramForward.Add(&histogramBackToBack);
        histogramForward.Draw(drawStyle.c_str());


        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }





    if (drawOption == "allNormalised") {
        TH2D histogramForward = myHistogram.getForwardHistogram();
        TH2D histogramBackward = myHistogram.getBackwardHistogram();
        TH2D histogramBackToBack = myHistogram.getBackToBackHistogram();

        TH2D histogramForwardBackground = myHistogram.getForwardBackground();
        TH2D histogramBackwardBackground = myHistogram.getBackwardBackground();
        TH2D histogramBackToBackBackground = myHistogram.getBackToBackBackground();
        
        Double_t maxvalForward = histogramForwardBackground.GetMaximum();
        Double_t maxvalBackward = histogramBackwardBackground.GetMaximum();
        Double_t maxvalBackToBack = histogramBackToBackBackground.GetMaximum();
        
        
        TH2D normalisedForwardBackground = (1/maxvalForward)*histogramForwardBackground;
        TH2D normalisedBackwardBackground = (1/maxvalBackward)*histogramBackwardBackground;
        TH2D normalisedBackToBackBackground = (1/maxvalBackToBack)*histogramBackToBackBackground;

        histogramForward.Divide(&normalisedForwardBackground);
        histogramBackward.Divide(&normalisedBackwardBackground);
        histogramBackToBack.Divide(&normalisedBackToBackBackground);

    
        histogramForward.Add(&histogramBackward);
        histogramForward.Add(&histogramBackToBack);
        histogramForward.Draw(drawStyle.c_str());


        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }

    if (drawOption == "forwardNormalised") {
        TH2D histogram = myHistogram.getForwardHistogram();
        TH2D histogramBackground = myHistogram.getForwardBackground();
        TH2D normalisedBackground;

        double maxval = histogramBackground.GetMaximum();
        normalisedBackground = (1/maxval)* histogramBackground;
        histogram.Divide(&normalisedBackground);
        
        
        histogram.Draw(drawStyle.c_str());

        
        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }
    

    if (drawOption == "backwardNormalised") {
        TH2D histogram = myHistogram.getBackwardHistogram();
        TH2D histogramBackground = myHistogram.getBackwardBackground();
        TH2D normalisedBackground;

        double maxval = histogramBackground.GetMaximum();
        normalisedBackground = (1/maxval)* histogramBackground;
        histogram.Divide(&normalisedBackground);
        
        
        histogram.Draw(drawStyle.c_str());

        
        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }

    if (drawOption == "backToBackNormalised") {
        TH2D histogram = myHistogram.getBackToBackHistogram();
        TH2D histogramBackground = myHistogram.getBackToBackBackground();
        TH2D normalisedBackground;

        double maxval = histogramBackground.GetMaximum();
        normalisedBackground = (1/maxval)* histogramBackground;
        histogram.Divide(&normalisedBackground);
        
        
        histogram.Draw(drawStyle.c_str());

        
        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }
    
    



    (void)argc;

}


