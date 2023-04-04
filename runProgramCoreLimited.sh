#!/bin/bash 

pids="" #Process id:s so that we can wait for all to finish
maxcores=2
corecounter=0

#Makes sure there is a new empty folder
rm -rfd processedData
mkdir processedData


#Starts the reading in of each file
for entry in $1*.root; do
    echo "Starting process for $entry"
    ./readData $entry&
    processId="$!"
    pids="$pids $processId"
    corecounter=$(($corecounter+1))

    #Makes sure we are not using more than $maxcores cores.
    if [[ $corecounter -ge $maxcores ]]; then
        for id in $pids; do
            wait $id
        done
        pids=""
        corecounter=0
    fi
done

echo "All files have now been processed"