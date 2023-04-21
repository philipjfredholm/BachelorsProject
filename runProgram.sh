#!/bin/bash

pids="" #Stores active process id:s so that we can wait for all to finish
maxcores=$2
fileextension=".root"

#Makes sure there is a new empty folder
rm -rfd processedData
rm -rfd "temp"
mkdir processedData
mkdir temp
mkdir -p  processedData/disk/DataSets_PbPb/TPCFMDTrees/LHC15o/WithFMD/ #For backwards compatiblity

#Stores a list of all files the directory to be read in ascending file size order
#so that some files may be used while waiting for program to finish
ls -S ${1}*.root > fileListDescending.txt
tac fileListDescending.txt > fileListAscending.txt
rm fileListDescending.txt


#Starts the reading in of each files 
for entry in $(cat fileListAscending.txt); do
    echo "Starting process for $entry"
    echo $(date) #Prints start time
    numberOfEvents=$(./entries $entry)
    eventsPerCore=$((numberOfEvents / maxcores))

    for task in $(seq 1 $maxcores); do
        newfolder="temp"
        filename="${entry#"$1"}"
        filename="${filename%".root"}"
        newname="${newfolder}/${filename}part${task}.root"
        cp $entry $newname

        start=$(( eventsPerCore * (task-1) ))
        stop=$(( eventsPerCore * task ))
        if [[ $task = $maxcores ]]; then
            stop=$numberOfEvents
        fi

        ./readData $newname $start $stop &
        processId="$!"
        pids="$pids $processId"


    done

    for id in $pids; do
        wait $id
    done

    for task in $(seq 1 $maxcores); do
        newfolder="temp"
        filename="${entry#"$1"}"
        filename="${filename%".root"}"
        newname="${newfolder}/${filename}part${task}.root"
        rm $newname

    done

    echo $(date) #Prints finish time


done


rm fileListAscending.txt
echo "All files have now been processed"


