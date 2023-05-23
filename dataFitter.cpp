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
#include <TRandom2.h>




/* Purpose:
This program is intended to able to extract the the various flow harmonic from
correlations taken from the lead-lead (PbPb) collisions by fitting them to Fourier harmonics
and background from proton-proton (pp) collisions to account for non flow effects of 
QCD.
 */


//ROOT is a bit messy so it is easier to do things with global variables in this case.
TH1D data;
TH1D background;
TRandom2 randomValue(1);


//Returns the pp-background value for a given angle
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



//Function that the data will be fitted to.
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


//Projects 2D histograms to 1D and does the proper weighted-average/error propagation
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



//My own implementation of the standard deviation function because reading C++ documentation
//is a lot more time consuming than just writing my own function.
double calculateStandardDev(std::vector<double> values) {
    int entries = static_cast<int>(values.size());

    double average = 0;
    for (auto entry : values) {
        average += entry;
    }

    
    average /= entries;
    double sumOfSquares = 0;
    for (auto entry : values) {
        sumOfSquares += std::pow(entry - average , 2);
    }

    return std::sqrt(sumOfSquares/entries);

}



//Randomises the value of each bin entry
//according to a normal distribution centred on its value
//with its own uncertainty.
TH1D randomiseHistogram(TH1D histogram) {
    TH1D returnHistogram = histogram;
    int entries = histogram.GetNbinsX();
    double localValue;
    double standardDev;
    double newValue;

    

    for (int entry = 1; entry <= entries; entry++) {
        localValue = histogram.GetBinContent(entry);
        standardDev = histogram.GetBinError(entry);

        newValue = randomValue.Gaus(localValue, standardDev);
        returnHistogram.SetBinContent(entry, newValue);
        /*
        The histogram is randomised with the help of the 
        standard deviation and we then get the uncertainty in 
        v2 from doing multiple fits to randomised histograms.
        Since we do not want ROOT to go vigilante
        mode and take the uncertainty into account when it is doing
        its own fitting, we set it to 0 manually here.
        */
        returnHistogram.SetBinError(entry, 0.001); 

    }

    


    return returnHistogram;

}


//My own mean value function because reading C++ documentation
//is more time consuming than making my own version.
double meanValue(std::vector<double> vector) {
    /*
    Writing my own function is faster than learning how to use the
    functions in the <algorithm> header :)
    */

   double average = 0;
   for (auto entry : vector) {
        average += entry;
   }

   return average/static_cast<int>(vector.size());


}



int main(int argc, char **argv) {
    //Argument Processing
    std::string pathToFile = argv[1];
    std::string pathToBackground = argv[2];

    std::vector<double> startOfCentralityIntervals {50, 60, 65, 70, 75, 80, 85, 90};
    std::vector<double> startOfPtIntervals {1, 1.5, 2, 2.5, 3, 3.5, 4, 5, 6}; //{0.2, 0.5, 1.0}; //


    //Reads in the data
    storeInHist dataHistograms(pathToFile);
    storeInHist backgroundHistograms(pathToBackground);
    dataHistograms.setErrors();  //Calculates the correct errors
    backgroundHistograms.setErrors(); 
                                            
    dataHistograms.loadProcessed(); //Handles the event mixing properly
    backgroundHistograms.loadProcessed();


    std::vector<std::vector<TH2D>> dataVectorForward = dataHistograms.getForwardProcessed();
    std::vector<std::vector<TH2D>> backgroundVectorForward = backgroundHistograms.getForwardProcessed();
    std::vector<std::vector<TH2D>> dataVectorBackward = dataHistograms.getBackwardProcessed();
    std::vector<std::vector<TH2D>> backgroundVectorBackward = backgroundHistograms.getBackwardProcessed();
    std::vector<std::vector<TH2D>> dataVectorBackToBack = dataHistograms.getBackToBackProcessed();
    std::vector<std::vector<TH2D>> backgroundVectorBackToBack = backgroundHistograms.getBackToBackProcessed();



    //Vectors for storing the results    
    std::vector<std::vector<std::vector<double>>> v2ForwardList;
    std::vector<std::vector<std::vector<double>>> v2BackwardList;
    std::vector<std::vector<std::vector<double>>> v2BackToBackList;

    std::vector<std::vector<double>> v2ForwardFinalList;
    std::vector<std::vector<double>> v2BackwardFinalList;
    std::vector<std::vector<double>> v2BackToBackFinalList;

    std::vector<std::vector<double>> v2ErrorForwardList;
    std::vector<std::vector<double>> v2ErrorBackwardList;
    std::vector<std::vector<double>> v2ErrorBackToBackList;

    std::vector<std::vector<double>> placeHolder;
    std::vector<double> placeHolder2;



    //Defines the fitting function
    TF1 v2Finder("v2Finder", fitFunction, 0, 2*TMath::Pi()-0.0001, 4);
    v2Finder.SetParNames("Background Amplitude", "Scaling", "v2", "v3", "v4", "v5", "v6", "v7");
    v2Finder.SetParameter(2,0.5); //Initial Guesses
    v2Finder.SetParameter(2,0.05); 
    v2Finder.SetParLimits(2, 0, 1);
    v2Finder.SetParLimits(3, 0, 1);


    //Loops over all different cases
    int numberOfDraws = 5;
    TH2D dataTemp;
    TH2D backgroundTemp;
    

    //Calculates v2 many times for many different random variations to
    //so that the uncertainty of the final value/fit may be 
    //determined
    for (int ptNumber = 0; ptNumber < static_cast<int>(dataVectorForward.size()); ptNumber++) {
        v2ForwardList.push_back(placeHolder);
        v2BackwardList.push_back(placeHolder);

        if (ptNumber == 0) {
            v2BackToBackList.push_back(placeHolder);
        }

        for (int centralityNumber = 0; centralityNumber < static_cast<int>(dataVectorForward[0].size()); centralityNumber++) { 
            v2ForwardList[ptNumber].push_back(placeHolder2);
            v2BackwardList[ptNumber].push_back(placeHolder2);

            if (ptNumber == 0) {
                v2BackToBackList[ptNumber].push_back(placeHolder2);
            }


            //Forward
            dataTemp = dataVectorForward[ptNumber][centralityNumber];
            backgroundTemp = backgroundVectorForward[ptNumber][0];
            for (int pull = 0; pull < numberOfDraws; pull++) {
                data = randomiseHistogram(projectHistogram(dataTemp));
                background = randomiseHistogram(projectHistogram(backgroundTemp));
                data.Fit("v2Finder", "RQ0");
                v2ForwardList[ptNumber][centralityNumber].push_back(v2Finder.GetParameter(2));

            }
        

            //Backward
            dataTemp = dataVectorBackward[ptNumber][centralityNumber];
            backgroundTemp = backgroundVectorBackward[ptNumber][0];
            for (int pull = 0; pull < numberOfDraws; pull++) {
                data = randomiseHistogram(projectHistogram(dataTemp));
                background = randomiseHistogram(projectHistogram(backgroundTemp));
                
                data.Fit("v2Finder", "RQ0");
                v2BackwardList[ptNumber][centralityNumber].push_back(v2Finder.GetParameter(2));

            }

    
            //BackToBack
            if (ptNumber == 0 ) {
                dataTemp = dataVectorBackToBack[ptNumber][centralityNumber];
                backgroundTemp = backgroundVectorBackToBack[ptNumber][0];
                for (int pull = 0; pull < numberOfDraws; pull++) {
                    data = randomiseHistogram(projectHistogram(dataTemp));
                    background = randomiseHistogram(projectHistogram(backgroundTemp));
                    
                    data.Fit("v2Finder", "RQ0");
                    v2BackToBackList[ptNumber][centralityNumber].push_back(v2Finder.GetParameter(2));


                }
            }

       



        }
    }


    //Calculates the mean values and standard deviations. 
    for (int ptNumber = 0; ptNumber < static_cast<int>(dataVectorForward.size()); ptNumber++) {
        //Initialisation
        v2ForwardFinalList.push_back(placeHolder2);
        v2BackwardFinalList.push_back(placeHolder2);

        v2ErrorForwardList.push_back(placeHolder2);
        v2ErrorBackwardList.push_back(placeHolder2);

        if (ptNumber == 0) {
            v2BackToBackFinalList.push_back(placeHolder2);
            v2ErrorBackToBackList.push_back(placeHolder2);
        } 


        for (int centralityNumber = 0; centralityNumber < static_cast<int>(dataVectorForward[ptNumber].size()); centralityNumber++) { 
            v2ForwardFinalList[ptNumber].push_back(meanValue(v2ForwardList[ptNumber][centralityNumber]));
            v2BackwardFinalList[ptNumber].push_back(meanValue(v2BackwardList[ptNumber][centralityNumber]));

            v2ErrorForwardList[ptNumber].push_back(calculateStandardDev(v2ForwardList[ptNumber][centralityNumber]));
            v2ErrorBackwardList[ptNumber].push_back(calculateStandardDev(v2BackwardList[ptNumber][centralityNumber]));

            if (ptNumber == 0) {
                v2BackToBackFinalList[ptNumber].push_back(meanValue(v2BackToBackList[ptNumber][centralityNumber]));
                v2ErrorBackToBackList[ptNumber].push_back(calculateStandardDev(v2BackToBackList[ptNumber][centralityNumber]));
            } 
            

        }
    }



    //Saves the data
    TFile results("results.root", "RECREATE");
    results.WriteObject(&v2ForwardFinalList, "v2ForwardList");
    results.WriteObject(&v2BackwardFinalList, "v2BackwardList");
    results.WriteObject(&v2BackToBackFinalList, "v2BackToBackList");

    results.WriteObject(&v2ErrorForwardList, "v2ErrorForwardList");
    results.WriteObject(&v2ErrorBackwardList, "v2ErrorBackwardList");
    results.WriteObject(&v2ErrorBackToBackList, "v2ErrorBackToBackList");

    results.WriteObject(&startOfCentralityIntervals, "startOfCentralityIntervals");
    results.WriteObject(&startOfPtIntervals, "startOfPtIntervals");

    results.Close();
    (void)argc;


}






