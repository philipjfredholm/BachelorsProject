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



    /*
    These values below are just left for backwards compatiblity, and 
    changing them might have unintended consequences. If I had more time
    I could had cleaned up the syntax more. The real values are set 
    in the file src/storeInHist.cxx in the member function loadHistograms().
    */
    Short_t cutOption = 3; 
    double centralityStart = 50; 
    double centralityStop = 90;
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


