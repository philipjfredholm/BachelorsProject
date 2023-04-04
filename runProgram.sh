#!/bin/bash 

pids="" #Process id:s so that we can wait for all to finish

rm -rfd processedData
mkdir processedData

#Starts the reading in of each file
for entry in $1/*.root; do
    echo "Starting process for $entry"
    ./readData $entry&
    pids="$pids $!"
done

#Makes sure all reading in has been finished before starting the merging program
for id in $pids; do
    wait $id
done

echo "All files have now been processed"

#Combines the histograms into one

./combine processedData all

echo "Finished"

