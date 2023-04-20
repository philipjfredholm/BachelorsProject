#!/bin/bash 

pids="" #Process id:s so that we can wait for all to finish
maxcores=$2
fileextension=".root"

#Makes sure there is a new empty folder
rm -rfd processedData
rm -rfd "$1/temp"
mkdir processedData
mkdir "${1}/temp"
#mkdir -p  processedData/disk/DataSets_PbPb/TPCFMDTrees/LHC15o/WithFMD/


ls -S ${1}*.root > fileListDescending.txt
tac fileListDescending.txt > fileListAscending.txt
rm fileListDescending.txt



#Starts the reading in of each file
for entry in $(cat fileListAscending.txt); do
    echo "Starting process for $entry"
    echo $(date)
    numberOfEvents=$(./entries $entry)
    eventsPerCore=$((numberOfEvents / maxcores))

    for task in $(seq 1 $maxcores); do
        newfolder="${1}temp"
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
        newfolder="${1}temp"
        filename="${entry#"$1"}"
        filename="${filename%".root"}"
        newname="${newfolder}/${filename}part${task}.root"
        rm $newname

    done

    echo $(date)


done

#rm fileListAscending.txt
echo "All files have now been processed"
