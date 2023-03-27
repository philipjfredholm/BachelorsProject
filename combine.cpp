//Core C++
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <future>
#include <mutex>


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


    storeInHist myHistogram(0); //The numbers are random, I needed to change the signature since TObject already does a default constructor.
    for (const auto& file : std::filesystem::directory_iterator(pathToFile)) {
        
        std::cout << file.path() << std::endl;
        storeInHist dummyHistogram (file.path());
        myHistogram.addHistogram(dummyHistogram);


    } 


    //Stores the histogram
    myHistogram.setStorageName("totalData");
    myHistogram.storeHistogramInFile();


    //Plots the histogram
    TH2D histogram = myHistogram.getHistogram();
    histogram.Draw("surf1");




    //Runs the application
    canvas.Modified(); 
    canvas.Update();
    app.Run();


    (void)argc;

}


