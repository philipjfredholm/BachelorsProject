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



/* Purpose:
The purpose of this file is to be able to read in parts of files
and process data as well as the event mixing background.
These are just some simple running instructions, and a lot of them
are left for backwards compatibility reasons.

The input parameters are handled by runProgram.sh
*/


int main(int argc, char **argv) {
    //Reads in the input variables
    std::string pathToFile = argv[1];
    std::string startString = argv[2];
    std::string stopString = argv[3];
    int start = std::stoi(startString);
    int stop = std::stoi(stopString);

    //Program Parameters
    int phiBins = 20; //20 bins for the FMD:s
    int etaBins = 320; //400 bins over -10 to 10 in the FMD:s
    double etaMin = -6; //eta is the pseudorapidity
    double etaMax = 10;

        //cutOption is just left for backwards compatiblity,
        //changing it will not change the actual cut option when the 
        //program runs. 
    Short_t cutOption = 3; 

        //These centrality values are just minimum and maximum bounds for
        // quickly discarding data so that the program may run faster,
        //the actual regions are set in the implementation of .loadHistograms()
        //in src/storeInHist.cxx.
        //This is simply left for backwards compatibility with earlier versions.
    double centralityStart = 50; //Centralty is a number between 0 and 100
    double centralityStop = 90;

        //The same discussion above as for the centralities holds for 
        //the pT (transverse momentum) values here as well. 
    double ptStart = 0.2; 
    double ptStop = 6;


    //Runs the program
    storeInHist myHistogram {pathToFile, cutOption, 
                             centralityStart, centralityStop,
                             ptStart, ptStop,
                             etaMin, etaMax,
                             phiBins, etaBins,
                             start, stop};



    (void)argc;

}


