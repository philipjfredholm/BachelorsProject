//Core C++
#include <iostream>
#include <string>
#include <vector>
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
#include <TLegend.h>
#include <TStyle.h>


/* Purpose
This program is made to plot various types of stored data for debugging purposes and to
combine all data into a single file.

It is able to plot both FMD-FMD and FMD-TPC correlations for the forward and backwards
FMD:s and the TPC. It can also plot the calculated event mixing background as well
as dividing by the background to get the true signal. It is also able to
project the data to the phi-axis and ignore the pseudorapidity values (eta)
once event mixing has been handled.
 */


TH1D projectHistogram(TH2D histogram) {
    std::string throwAwayName;
    /*
    For some odd reason ROOT needs a string as a name
    and stores that internally instead of just the object name.
    It results in memory leaks if I name a histogram the same thing twice,
    so I need a new name for every histogram. The easiest way to get
    a random throwaway garbage name is to deliberately leave a string uninitialised.
    */
    TH1D returnHistogram(throwAwayName.c_str(), "Counts", histogram.GetNbinsX(), 0, 2*TMath::Pi()); 
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
    //Makes the application
    char myChar = 'a'; //ROOT requires argv but I do not want to give it.
    char* myCharPtr = &myChar;
    TRint app("app", 0, &myCharPtr);
    TCanvas canvas("canvas", "Title", 0, 0 ,800,600);


    //Reads in the parameters
    std::string pathToFile = argv[1];
    std::string drawOption = argv[2];
    std::string drawStyle = argv[3];
    std::string ptRegionString = argv[4];
    std::string centralityRegionString = argv[5]; 
    int ptRegion= std::stoi(ptRegionString);
    int centralityRegion = std::stoi(centralityRegionString);
    

    //Reads in the data in the histograms
        //The number '0' in 'myHistogram(0)' is random, I needed to change the signature since 
        //the inheritance from TObject already does a default constructor and another default constructor
        // is needed here.

    

    storeInHist* myHistogram = new storeInHist(0);

    if (std::filesystem::exists("totalDataProcessed.root")) {
        storeInHist* myHistogram2 = new storeInHist("totalDataProcessed.root");
        *myHistogram = *myHistogram2; 
        delete myHistogram2;

    } else {
        for (const auto& file : std::filesystem::directory_iterator(pathToFile)) {
            std::string filename = file.path();
            if (filename == "totalDataProcessed.root") { 
                //A file is later stored with this name and this avoids double counting
                continue;
            }

            //Empties unused data to save RAM
            storeInHist dummyHistogram {filename}; 



            myHistogram->addHistograms(dummyHistogram);




        } 
    }






    //Stores the merged read in data
    myHistogram->setStorageName("totalData");
        //All cases have stored data with normalised values after their
        // respective event mixings have been accounted for, but this is just to
        //be able to debug individual files. In order to get the proper statistics,
        //the normalisation w.r.t the event mixing needs to be done after the
        //measured data and the event mixing have all been added up, only then
        //can the actual normalisation take place properly.
        //.loadProcessed() is the member function to do this
  
    myHistogram->loadProcessed(); 
    myHistogram->storeHistogramInFile();



    //Checks which plotting option has been given
    if (drawOption == "forward") { //ForwardFMD-TPC correlations
        std::vector<std::vector<TH2D>> histogramForwardvector = myHistogram->getForwardHistograms();
        TH2D histogramForward = histogramForwardvector[ptRegion][centralityRegion];
        histogramForward.Draw(drawStyle.c_str());

        canvas.Modified(); 
        canvas.Update();
        app.Run();


    }

    if (drawOption == "forwardBackground") { //ForwardFMD-TPC event mixing
        std::vector<std::vector<TH2D>> histogramForwardvector = myHistogram->getForwardBackgrounds();
        TH2D histogramForward = histogramForwardvector[ptRegion][centralityRegion];
        histogramForward.Draw(drawStyle.c_str());

        canvas.Modified(); 
        canvas.Update();
        app.Run();


    }

    if (drawOption == "backward") { //BackwardFMD-TPC correlations
        std::vector<std::vector<TH2D>> histogramBackwardvector = myHistogram->getBackwardHistograms();
        TH2D histogramBackward = histogramBackwardvector[ptRegion][centralityRegion];
        histogramBackward.Draw(drawStyle.c_str());

        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }

    if (drawOption == "backwardBackground") { //BackwardFMD-TPC event mixing
        std::vector<std::vector<TH2D>> histogramBackwardvector = myHistogram->getBackwardBackgrounds();
        TH2D histogramBackward = histogramBackwardvector[ptRegion][centralityRegion];
        histogramBackward.Draw(drawStyle.c_str());

        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }



    if (drawOption == "backToBack") { //ForwardFMD-BackwardFMD correlations
        std::vector<std::vector<TH2D>> histogramBackToBackvector = myHistogram->getBackToBackHistograms();
        TH2D histogramBackToBack = histogramBackToBackvector[ptRegion][centralityRegion];
        histogramBackToBack.Draw(drawStyle.c_str());

        canvas.Modified(); 
        canvas.Update();
        app.Run();
    }



    if (drawOption == "backToBackBackground") { //ForwardFMD-BackwardFMD event mixing
        std::vector<std::vector<TH2D>> histogramBackToBackvector = myHistogram->getBackToBackBackgrounds();
        TH2D histogramBackToBack = histogramBackToBackvector[ptRegion][centralityRegion];
        histogramBackToBack.Draw(drawStyle.c_str());
        

        canvas.Modified(); 
        canvas.Update();
        app.Run();
    }







    if (drawOption == "none") { //In case one only wants to combine the data files without plotting
        //Does nothing
    }

    if (drawOption == "all") { //Plots all the different correlations, amplitudes may be skewed due to having different
                                //number of tracks
        std::vector<std::vector<TH2D>> histogramForwardvector = myHistogram->getForwardHistograms();
        std::vector<std::vector<TH2D>> histogramBackwardvector = myHistogram->getBackwardHistograms();
        std::vector<std::vector<TH2D>> histogramBackToBackvector = myHistogram->getBackToBackHistograms();

        TH2D histogramForward = histogramForwardvector[ptRegion][centralityRegion];
        TH2D histogramBackward = histogramBackwardvector[ptRegion][centralityRegion];
        //The FMD:s have no pT-data (pT = transverse momentum), so all pT:s 
        //are in the same region and hence the [0] index. Nesting the data in another vector 
        //anyhow is just for purposes of consistency
        TH2D histogramBackToBack = histogramBackToBackvector[0][centralityRegion];
        
        
        histogramForward.Add(&histogramBackward);
        histogramForward.Add(&histogramBackToBack);
        histogramForward.Draw(drawStyle.c_str());


        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }

    if (drawOption == "allBackground") {//Plots all the different event mixings, amplitudes may be skewed due to having different
                                        //number of tracks
        std::vector<std::vector<TH2D>> histogramForwardvector = myHistogram->getForwardBackgrounds();
        std::vector<std::vector<TH2D>> histogramBackwardvector = myHistogram->getBackwardBackgrounds();
        std::vector<std::vector<TH2D>> histogramBackToBackvector = myHistogram->getBackToBackBackgrounds();
        
        TH2D histogramForward = histogramForwardvector[ptRegion][centralityRegion];
        TH2D histogramBackward = histogramBackwardvector[ptRegion][centralityRegion];
        //The FMD:s have no pT-data (pT = transverse momentum), so all pT:s 
        //are in the same region and hence the [0] index. Nesting the data in another vector 
        //anyhow is just for purposes of consistency
        TH2D histogramBackToBack = histogramBackToBackvector[0][centralityRegion];
        
        
        histogramForward.Add(&histogramBackward);
        histogramForward.Add(&histogramBackToBack);
        histogramForward.Draw(drawStyle.c_str());


        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }





    if (drawOption == "allNormalised") { //Plots the calculated data for all cases once
                                        // event mixing has been taken care of.
        
        //See the comment earlier in the file about the 
        //purpose of .loadProcessed()
        myHistogram->loadProcessed();
        std::vector<std::vector<TH2D>> histogramForwardvector = myHistogram->getForwardProcessed();
        std::vector<std::vector<TH2D>> histogramBackwardvector = myHistogram->getBackwardProcessed();
        std::vector<std::vector<TH2D>> histogramBackToBackvector = myHistogram->getBackToBackProcessed();

        TH2D histogramForward = histogramForwardvector[ptRegion][centralityRegion];
        TH2D histogramBackward = histogramBackwardvector[ptRegion][centralityRegion];
        //The FMD:s have no pT-data (pT = transverse momentum), so all pT:s 
        //are in the same region and hence the [0] index. Nesting the data in another vector 
        //anyhow is just for purposes of consistency
        TH2D histogramBackToBack = histogramBackToBackvector[0][centralityRegion];

        histogramForward.Add(&histogramBackward);
        histogramForward.Add(&histogramBackToBack);
        histogramForward.Draw(drawStyle.c_str());


        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }

    if (drawOption == "forwardNormalised") {//Plots the calculated data for forwardFMD-TPC correlations,
                                            //once event mixing has been taken care of.
        
        //See the comment earlier in the file about the 
        //purpose of .loadProcessed()
        myHistogram->loadProcessed();
        std::vector<std::vector<TH2D>> histogramvector = myHistogram->getForwardProcessed();
        TH2D histogram = histogramvector[ptRegion][centralityRegion];
        
        
        histogram.Draw(drawStyle.c_str());

        
        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }
    

    if (drawOption == "backwardNormalised") {//Plots the calculated data for backwardFMD-TPC correlations,
                                            //once event mixing has been taken care of.
        
        //See the comment earlier in the file about the 
        //purpose of .loadProcessed()        
        myHistogram->loadProcessed();
        std::vector<std::vector<TH2D>> histogramvector = myHistogram->getBackwardProcessed();
        TH2D histogram = histogramvector[ptRegion][centralityRegion];
        
        histogram.Draw(drawStyle.c_str());

        
        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }

    if (drawOption == "backToBackNormalised") {//Plots the calculated data for forwardFMD-backwardFMD correlations,
                                            //once event mixing has been taken care of.

        //See the comment earlier in the file about the 
        //purpose of .loadProcessed()        
        myHistogram->loadProcessed();
        std::vector<std::vector<TH2D>> histogramvector = myHistogram->getBackToBackProcessed();
        //The FMD:s have no pT-data (pT = transverse momentum), so all pT:s 
        //are in the same region and hence the [0] index. Nesting the data in another vector 
        //anyhow is just for purposes of consistency
        TH2D histogram = histogramvector[0][centralityRegion];

        
        histogram.Draw(drawStyle.c_str());

        
        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }
    
    

    if (drawOption == "forwardFinished") {//Plots the calculated data for forwardFMD-TPC correlations,
                                        //once event mixing has been taken care of and projects it onto the
                                        //the phi-axis    
        
        //See the comment earlier in the file about the 
        //purpose of .loadProcessed() 
        myHistogram->loadProcessed();
        myHistogram->setErrors();
        std::vector<std::vector<TH2D>> histogramvector = myHistogram->getForwardProcessed();
        TH2D histogram = histogramvector[ptRegion][centralityRegion];
        TH1D phiProjection = projectHistogram(histogram);
        
        gPad->SetGrid();
        gStyle->SetOptStat(0);
        phiProjection.GetXaxis()->SetTitle("#Delta #varphi");
        phiProjection.GetYaxis()->SetTitle("Scaled Counts");
        phiProjection.GetXaxis()->SetTitleSize(0.04);
        phiProjection.GetYaxis()->SetTitleSize(0.04);
        phiProjection.SetTitle("Shows TPC-FMD1 Correlations");
        phiProjection.SetFillColorAlpha(kBlue, 0.5);
        phiProjection.Draw("HIST E");


        TLegend myLegend(0.65, 0.7, 0.89, 0.9);
        myLegend.SetTextSize(0.03);
        myLegend.AddEntry(&phiProjection, "#it{p_{T}} 2.0 - 2.5 (GeV)", "l");
        myLegend.Draw("same");   

       
        canvas.SetLeftMargin(0.15);
        canvas.Print("combineForwardExample.pdf");
        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }


    if (drawOption == "backwardFinished") {//Plots the calculated data for bacmwardFMD-TPC correlations,
                                        //once event mixing has been taken care of and projects it onto the
                                        //the phi-axis   
        
        //See the comment earlier in the file about the 
        //purpose of .loadProcessed() 
        myHistogram->loadProcessed();
        myHistogram->setErrors();
        std::vector<std::vector<TH2D>> histogramvector = myHistogram->getBackwardProcessed();
        TH2D histogram = histogramvector[ptRegion][centralityRegion];
        TH1D phiProjection = projectHistogram(histogram);


        gPad->SetGrid();
        gStyle->SetOptStat(0);
        phiProjection.GetXaxis()->SetTitle("#Delta #varphi");
        phiProjection.GetYaxis()->SetTitle("Scaled Counts");
        phiProjection.GetXaxis()->SetTitleSize(0.04);
        phiProjection.GetYaxis()->SetTitleSize(0.04);
        phiProjection.SetTitle("Shows TPC-FMD2 Correlations");
        phiProjection.SetFillColorAlpha(kBlue, 0.5);
        phiProjection.Draw("HIST E");


        TLegend myLegend(0.65, 0.7, 0.89, 0.9);
        myLegend.SetTextSize(0.03);
        myLegend.AddEntry(&phiProjection, "#it{p_{T}} 5.0 - 6.0 (GeV)", "l");
        myLegend.Draw("same");   

       
        canvas.SetLeftMargin(0.15);
        canvas.Print("combineBackwardExample.pdf");

        
        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }

    if (drawOption == "backToBackFinished") {//Plots the calculated data for forwardFMD-backwardFMD correlations,
                                        //once event mixing has been taken care of and projects it onto the
                                        //the phi-axis   
        
        //See the comment earlier in the file about the 
        //purpose of .loadProcessed() 
        myHistogram->loadProcessed();
        myHistogram->setErrors();
        std::vector<std::vector<TH2D>> histogramvector = myHistogram->getBackToBackProcessed();
        //The FMD:s have no pT-data (pT = transverse momentum), so all pT:s 
        //are in the same region and hence the [0] index. Nesting the data in another vector 
        //anyhow is just for purposes of consistency
        TH2D histogram = histogramvector[0][centralityRegion];
        TH1D phiProjection = projectHistogram(histogram);



        gPad->SetGrid();
        gStyle->SetOptStat(0);
        phiProjection.GetXaxis()->SetTitle("#Delta #varphi");
        phiProjection.GetYaxis()->SetTitle("Scaled Counts");
        phiProjection.GetXaxis()->SetTitleSize(0.04);
        phiProjection.GetYaxis()->SetTitleSize(0.04);
        phiProjection.SetTitle("Shows FMD1-FMD2 Correlations");
        phiProjection.SetFillColorAlpha(kBlue, 0.5);
        phiProjection.Draw("HIST E");


        TLegend myLegend(0.65, 0.7, 0.89, 0.9);
        myLegend.SetTextSize(0.03);
        myLegend.AddEntry(&phiProjection, "#splitline{#it{p_{T}} 1.0 - 1.5 (GeV)}{Centrality 50%-60%}", "l");
        //myLegend.Draw("same");   

       
        canvas.SetLeftMargin(0.15);
        canvas.Print("combineBackToBackExample.pdf");
        canvas.Modified(); 
        canvas.Update();
        app.Run();

    }

    delete myHistogram;

    (void)argc;

}


