#!/bin/bash 

pids=""

rm -rfd processedData

#Starts the reading in of each file
for entry in $1/*.root; do
    echo "Starting process for $entry"
    ./readData $entry&
    pids="$pids $!"
done

#Makes sure all reading in has been performed
for id in $pids; do
    wait $id
done

echo "All files have now been processed"

#Combines the histograms into one

./combine processedData

echo "Finished"

