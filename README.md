# Philip Fredholm's Bachelor's Project
* Requirements: 
    * You must have ROOT installed (tested with v6.26/10) and have sourced the 'thisroot.sh'-file.
            
    * You must have Make installed.

    * You must have at least C++17 available in your compiler (your ROOT-compilation must also be C++17 compatible).




# Instructions
This repository contains the files used in my (Philip Fredholm's) bachelor's project. For the proper context of what all the things do, please see the thesis/report available at [ADD LINK HERE]. The 
original files which I were given were ROOT-files storing ROOT-trees according to the structure shown in Figure 7 of the report. The various objects were declared and implemented according to the files in the folder `raw`. The end output are the figures seen in the report. Here, I outline the general steps taken to produce these plots. 

1. First, the command `make all` should be entered into the terminal. This will create all the necessary executables according to the naming scheme that a file called i.e. `combine.cpp`
has its executable simply named `combine`. Then, the script `runProgram.sh` should be used to begin processing data. It takes two arguments; the first of which is the path to the folder
which contains **only** the previously described ROOT-files. Note that it is important that there is a forward slash '/' at the the end of the path, e.g. `/path/to/my/file/` and not `/path/to/my/file`.
The second argument is the number of cores that should be used. Note that due to implementation reasons, one should be careful with not using an excessive number of cores for files containing a very small
number of events. The output will then be stored in a folder in the present working directory called `processedData`.

Note that `runProgram.sh` is mostly just a wrapper. It simply starts multiple instances of the program associated with `readData.cpp`. This is in turn also a sort of wrapper. Since the code for data processing got somewhat complicated, the processing is done by instantiating an instance of a class called `storeInHist` (defined in `include/storeInHist.h` and implemented in `src/storeInHist.cxx`) which's constructor does all of the data processing and stores the results in a file. It also has some other functionality used later, see the actual code files for that. However, it is important to note that while some parameters are defined in `readData.cpp`, a lot are defined in the primary function `loadHistograms()` in the implementation of the `storeInHist` class. Some variables are even overriden so it is important that variables are changed in **both** places to get the correct result. The reason for this is that it was necessary during the development process for backwards compatibility reason and due to time constraints it has not been a prioirty to update it (since there are a lot of dependencies on these things in `src/storeInHist.cxx` it would take a lot of work). 

2. Since `runProgram.sh` creates one file per core used, and since results from running the script on different files also creates different files, the data must be merged together. The file `combine.cpp` handles this. It also fills a secondary purpose in being able to plot one example out of the files combined. Note that some plotting options will have i.e. titles and axis labels and these are not updated automatically and must be changed manually. The arguments that `combine.cpp` takes are the following; firstly the path to the folder containing **only** the files to be combined (this time it is without the forwrad slash '/' at the end so be carefuly with the syntax), secondly which correlations are to be plotted where `none` only does the combining and no plotting (see the actual code for the different options), tertiary a drawing option which is passed the the `Draw()` method of the ROOT class `TH2D` (i.e `colz` or `surf1`), the fourth argument is the index of pT interval defined in the std::vector<double> array in the `loadHistograms()` method and the final argument is dito for the centrality interval. The program then outputs the combined results into a fiel called `totalDataProcessed.root`.

Note that there is are some other peculiarities about the `combine.cpp` file. One is that due to time efficiency, the program checks if there is already a file called `totalDataProcessed.root` before beginning the combining process. If it exists, it opens that file instead of performing the combining at the entered file path. This was implemented so that the plotting functionality could be used instantly without having to wait for the combining process every time one wanted to see a different plot. The second pecularity is a bug, which I suspect is related to ROOT. Regardless of cause, the issue is that when combining files of i.e. 2 GB, a computer with 12 GB of RAM somehow manages to run out of memeory. I have looked through my code for a possible cause of this bug but I have been unable to find it. I suspect that it has to do with ROOT. Customarily, ROOT is not used in a stand-alone mode but with its own custom interpreter. Typically, a lot of ROOT code is written with a lot of raw pointers placing things on the heap. When using ROOT with its own interpreter, it cleans up things behind the scenes to prevent memory leak. I discovered early on in the project that there is definitely no such clean up behind the scenes going on in the stand-alone C++ version, so one has to be careful to not throw around pointers carelessly. I suspect that the issue is due to some internal implementation of ROOT objects expecting this 'magic clean up' of the compiler to happen and when it does not in the stand-alone version, a memory leak occurs. The solution is simply to split up the file one wishes to combine into different subfolders and combine those separately. Then, the `totalDataProcessed.root` files may be renamed and placed in another folder, on which `combine.cpp` may be used again to combine these files.


3. The next step is to calculate $\text{v}_2$ as described in the report. This is done using the file called `dataFitter.cpp`. It takes two arguments, which are the paths to the combined data from the lead-lead data and the proton-proton data. Note that as described in the report, the previous steps have to be repeated so that there is both a file for the lead-lead data and the proton-proton data for this to work. Note that this file also computes the proper uncertainties, which uses the method described in the report. The output file is a file called `results.root`. 

Note that one very important step here is that the pT and centrality intervals which were defined in the vectors in `loadHistograms()` have to be manually changed to the correct values in the code before running for things to work correctly. The reason for this is the usage of multiple files as explained in the next paragraph.


4. The final step is to make the plots. This is doen by the file called `resultsPlotter.cpp`. It also has pecularity due to the development process. Due to time constraints, all of the previous steps were first performed for data in the pT intervals between $1.0$ GeV and $6.0$ GeV and then for the intervals between $0.2$ GeV and $1.0$ GeV. This resulted in two `results.root` files (which were of course properly later renamed). Hence, `resultsPlotter.cpp` takes the path to the file with the $1.0$ GeV to $6.0$ GeV as its first argument and the other file as its second argument. As the third argument, it takes the index of the centrality interval (as defined in the vector in `loadHistograms()`) as an argument for which centrality interval to plot.


Finally, there are some other files in the repository, some of which are for producing additional plots or other dependencies. See the next section







# Other
There are some files which were not mentioned above. Here, brief descriptions are given of them. For a more comprenhensive view, please see the actual code in the files.

* `entries.cpp` : Takes one argument which is a path to a ROOT file with the structure as shown in Figure 7 of the report. Returns the number of events stored in the file so that
`runProgram.sh` can do parallelism properly.
* `dataFitterVisualiser` : Used for creating plots for the report. Various different options are available from the command line, but some options like the title of the graph,
the legend and the axis labels have to be changed manually in the code and do not update automatically.
* `protonSubtracter` : The same as `dataFitterVisualiser` but it subtracts the scaled (based on the fitting results) proton background and creates such plots for the report.
* `raw` : This folder contains the original versions of `AliLWUtils.cxx` and `AliLWUtils.h` given to me by my supervisor. They contain the definition and implementation of the classes
stored in the ROOT tree with the structure as shown in Figure 7 in the report.
* `include/AliLWUtils.h` nnd `src/AliLWUtils.cxx` the same files as in `raw` but slightly modified to be compatible with some other functionality which I use.
* `LinkDef.h` : Necessary for making ROOT work in the stand-alone C++ with its own custom interpreter. See the comments in `Makefile` for some more comments on how this works.
* `Makefile` : Configuration file used by the program `make` to easily compile all of the executable files correctly.
