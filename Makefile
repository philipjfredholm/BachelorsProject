#C++
CXX = g++
CXXVER =  #Handled by ROOT-flags below, at least C++17 is required for everything (including combine.cpp) to work
CXXWARS = -Wall -Wextra -Werror

#Linking
ROOT = `root-config --glibs --cflags --libs` #Takes care of ROOT-dependencies
ROOTYS = `root-config --incdir` 
INCLUDES = -I include   -I $(ROOTSYS)/include
LIBS = -L . -Wl,-rpath=.

#Shortcuts
RUN = $(CXX) $(CXXVER) $(CXXWARS) $(INCLUDES)
RUNL = $(CXX) $(CXXVER) $(CXXWARS) $(INCLUDES) $(LIBS)

#Needed for ROOT
HEADERS = include/AliLWUtils.h include/storeInHist.h
SOURCES = src/AliLWUtils.cxx src/storeInHist.cxx


#I think the 'root-config --incdirs' command is bugged. In the terminal it gives the path to the include/ directory of the ROOT
#installation, but in this Makefile it just gives the path to the ROOT-installation, hence the 
#$(ROOTSYS)/include in the actual commands. I suspect that this is a bug, and if it is patched
#in the future it might cause issues.


#Commands for running the analysis
all: combine.cpp readData.cpp AliLWUtils.o storeInHist.o rootDict.cxx libROOTlibs.so
	make readData
	make entries
	make dataFitter
	make combine 

#The "make combine" above might need to be commented out on machines which do
#not have compilers which C++17. ./combine is quite fast so it may run
#on a laptop without problem even if the workstation is old.
	
readData: readData.cpp AliLWUtils.o storeInHist.o rootDict.cxx libROOTlibs.so
	$(RUNL) -o readData readData.cpp AliLWUtils.o storeInHist.o -lROOTlibs $(ROOT) -O3

combine: combine.cpp AliLWUtils.o storeInHist.o rootDict.cxx libROOTlibs.so
	$(RUNL) -o combine combine.cpp AliLWUtils.o storeInHist.o -lROOTlibs $(ROOT) -O3 

resultsPlotter: resultsPlotter.cpp AliLWUtils.o storeInHist.o rootDict.cxx libROOTlibs.so
	$(RUNL) -o resultsPlotter resultsPlotter.cpp AliLWUtils.o storeInHist.o -lROOTlibs $(ROOT) -O3 

mergeSame: mergeSame.cpp AliLWUtils.o storeInHist.o rootDict.cxx libROOTlibs.so
	$(RUNL) -o mergeSame mergeSame.cpp AliLWUtils.o storeInHist.o -lROOTlibs $(ROOT) -O3 

entries: entries.cpp AliLWUtils.o rootDict.cxx libROOTlibs.so
	$(RUNL) -o entries entries.cpp AliLWUtils.o -lROOTlibs $(ROOT) -O3

dataFitter: dataFitter.cpp AliLWUtils.o storeInHist.o rootDict.cxx libROOTlibs.so
	$(RUNL) -o dataFitter dataFitter.cpp AliLWUtils.o storeInHist.o -lROOTlibs $(ROOT) -O3


dataFitterNew: dataFitterNew.cpp AliLWUtils.o storeInHist.o rootDict.cxx libROOTlibs.so
	$(RUNL) -o dataFitterNew dataFitterNew.cpp AliLWUtils.o storeInHist.o -lROOTlibs $(ROOT) -O3


dataFitterVisualiser: dataFitterVisualiser.cpp AliLWUtils.o storeInHist.o rootDict.cxx libROOTlibs.so
	$(RUNL) -o dataFitterVisualiser dataFitterVisualiser.cpp AliLWUtils.o storeInHist.o -lROOTlibs $(ROOT) -O3




#Object Files
AliLWUtils.o: include/AliLWUtils.h src/AliLWUtils.cxx
	$(RUN) -c include/AliLWUtils.h src/AliLWUtils.cxx  $(ROOT)


storeInHist.o: include/storeInHist.h src/storeInHist.cxx 
	$(RUN) -c include/storeInHist.h src/storeInHist.cxx  $(ROOT)



#ROOT Compatibility Things

#I have no idea what -p does, but just included it as the ROOT documentation said to do so.
#From what I can see the flag is not listed on rootcling:s man-page.

# $@ refers to the name of the Make-command and $^ refers to the listed dependencies of said Make-command

#-fPIC seems to do something about "position indepent code". I don't know why it is included,
#but the compiler gives me an error message and tells me to include it when the command is ran without it.

rootDict.cxx: $(HEADERS) include/LinkDef.h
	rootcling -f $@ -c -p $^




#It would be nice to place this inside ./libs/ , but the way the dictionary is created with paths
#that does not work.
libROOTlibs.so: rootDict.cxx $(SOURCES)
	$(CXX) -shared -o$@ -I $(ROOTSYS)/include $^ $(ROOT) -fPIC 



#Cleaning Options
clean1: 
	rm *.o
	rm *.so
	rm include/*.gch
	rm rootDict.cxx
	


clean2:
	rm readData
	rm combine
	rm dataFitter
	rm bias
	rm bias2
	rm biasGenereal
	rm *.pcm
	rm *.so
	rm main
	rm dataFitterOld
	rm resultsPlotter
	rm entries
	rm


clean: 
	clean1
	clean2





#Leftovers from earlier experimenting
test: test.cpp AliLWUtils.o storeInHist.o rootDict.cxx libROOTlibs.so
	$(RUNL) -o test test.cpp AliLWUtils.o storeInHist.o -lROOTlibs $(ROOT) -O3
	clean1

bias: bias.cpp AliLWUtils.o storeInHist.o rootDict.cxx libROOTlibs.so
	$(RUNL) -o bias bias.cpp AliLWUtils.o storeInHist.o -lROOTlibs $(ROOT) -O3
	clean1
	

biasGeneral: biasGeneral.cpp AliLWUtils.o storeInHist.o rootDict.cxx libROOTlibs.so
	$(RUNL) -o biasGeneral biasGeneral.cpp AliLWUtils.o storeInHist.o -lROOTlibs $(ROOT) -O3
	clean1


bias2: bias2.cpp AliLWUtils.o storeInHist.o rootDict.cxx libROOTlibs.so
	$(RUNL) -o bias2 bias2.cpp AliLWUtils.o storeInHist.o -lROOTlibs $(ROOT) -O3
	clean1

bias3: bias3.cpp AliLWUtils.o storeInHist.o rootDict.cxx libROOTlibs.so
	$(RUNL) -o bias3 bias3.cpp AliLWUtils.o storeInHist.o -lROOTlibs $(ROOT) -O3
	clean1

biasRead: biasRead.cpp AliLWUtils.o storeInHist.o rootDict.cxx libROOTlibs.so
	$(RUNL) -o biasRead biasRead.cpp AliLWUtils.o storeInHist.o -lROOTlibs $(ROOT) -O3
	clean1





.PHONY: all clean1 clean2 clean