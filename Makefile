#C++
CXX = g++
CXXVER =  #Handled by ROOT-flags below
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


#Commands
all: combine.cpp readData.cpp AliLWUtils.o storeInHist.o rootDict.cxx libROOTlibs.so
	make readData
	make combine
	make clean1


combine: combine.cpp AliLWUtils.o storeInHist.o rootDict.cxx libROOTlibs.so
	$(RUNL) -o combine combine.cpp AliLWUtils.o storeInHist.o -lROOTlibs $(ROOT) -O3
	make clean1



readData: readData.cpp AliLWUtils.o storeInHist.o rootDict.cxx libROOTlibs.so
	$(RUNL) -o readData readData.cpp AliLWUtils.o storeInHist.o -lROOTlibs $(ROOT) -O3
	make clean1





AliLWUtils.o: include/AliLWUtils.h src/AliLWUtils.cxx
	$(RUN) -c include/AliLWUtils.h src/AliLWUtils.cxx  $(ROOT)


storeInHist.o: include/storeInHist.h src/storeInHist.cxx
	$(RUN) -c include/storeInHist.h src/storeInHist.cxx $(ROOT)


#I have no idea what -p does, but just included it as the ROOT documentation said to do so.
#From what I can see the flag is not listed on rootcling:s man-page.
rootDict.cxx: $(HEADERS) include/LinkDef.h
	rootcling -f $@ -c -p $^



# $@ refers to the name of the Make-command and $^ refers to the listed dependencies of said Make-command

#-fPIC seems to do something about "position indepent code". I don't know why it is included,
#but the compiler gives me an error message and tells me to include it when the command is ran without it.

#It would be nice to place this inside ./libs/ , but the way the dictionary is created with paths
#that does not work.
libROOTlibs.so: rootDict.cxx $(SOURCES)
	$(CXX) -shared -o$@ -I $(ROOTSYS)/include $^ $(ROOT) -fPIC 



#Cleans away garbage created during compilation 
clean1: 
	rm *.o
	rm include/*.gch
	rm rootDict.cxx
	

#Cleans away the files needed to run the program
clean2:
	rm *.pcm
	rm *.so
	rm main

#Cleans away everything
clean: 
	rm *.o
	rm *.so
	rm include/*.gch
	rm rootDict.cxx
	rm *.pcm
	rm main


.PHONY: all clean1 clean2 clean
