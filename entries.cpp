//Core C++
#include <iostream>
#include <string>
#include <vector>

//Other
#include "AliLWUtils.h"

//ROOT
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TObject.h>
#include <TBranch.h>
#include <TClonesArray.h>


/* Purpose:
The only purpose of this program is to give the bash script
runProgram.sh the number of entries (events) in a given 
file so that it may properly divide (and hence parallelise) the intervals
that the readData.cpp file will handle.
*/

int main(int argc, char** argv) {
    //Reads in input data
    std::string pathToFile = argv[1];

    //Reads in the file
    TFile dataFile(pathToFile.c_str(), "dataFile", "READ");
    TTree* dataTree = (TTree*)dataFile.Get("LWTree");
    Int_t dataCount = dataTree->GetEntries();
    dataFile.Close();


    //Returns the value to the bash script
    std::cout << dataCount << std::endl;
        //Apparently the only way to return a value to a
        //bash-script is with 'cout' and not with a return statement for some reason
    //return dataCount;
    


    //Deliberately not using static_cast. I do not want 
    //to turn off compiler warnings about
    //unused variables but this is actually 
    //supposed to be discarded.
    (void)argc;
}
    