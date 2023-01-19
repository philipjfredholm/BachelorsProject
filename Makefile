CXX = g++
CXXVER =  #-std=c++17 #This is overriden by the ROOT-variable anyways.
#ROOT = `root-config --cflags --libs --glibs --ldflags --auxlibs` #If ROOT things are not found, run the command in the backticks in the terminal and copy-paste it here.
ROOT = -pthread -std=c++14 -m64 -I/home/fredholm/root/root/include -L/home/fredholm/root/root/lib -lCore -lImt -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lROOTVecOps -lTree -lTreePlayer -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lMultiProc -lROOTDataFrame -Wl,-rpath,/home/fredholm/root/root/lib -pthread -lm -ldl -rdynamic -L/home/fredholm/root/root/lib -lGui -m64 -Wl,-rpath,/home/fredholm/root/root/lib -pthread -lm -ldl -rdynamic
ERRORS = -Wall -Wextra -Wall
INCLUDES = $(ERRORS) -I include $(ROOT) 

#Make sure to double check the spelling


main: main.cpp
	$(CXX) $(CXXVER) $(INCLUDES) main.cpp -o main

main2: main.cpp
	g++ -Wall -Wextra -Wall main.cpp -o main `root-config --cflags --libs --glibs --ldflags --auxlibs` -I include

all: main.cpp AliLWUtils.o
	$(CXX) $(CXXVER) $(INCLUDES) main.cpp AliLWUtils.o -o main


clean: 
	rm *.o

AliLWUtils.o: include/AliLWUtils.h src/AliLWUtils.cxx
	$(CXX) $(CXXVER) $(INCLUDES) -c include/AliLWUtils.h src/AliLWUtils.cxx 




.PHONY: all clean