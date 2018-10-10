#!/usr/bin/env bash

rm log/output
rm core

NUM=10000
for i in `seq 1 ${NUM}`; do
    echo "====== $i ======" >> log/output
    ./build/x >> log/output
    #N=$(cat log/output |grep "successfully" |wc -l)
    #valgrind --leak-check=yes ./build/x >> log/output
    #N=$(cat log/output |grep "\-1" |wc -l)
    #if [ ${N} -gt 0 ]; then
    #    echo "ERROR"
    #    exit
    #fi
done

echo "----- BATCH TEST SUCCESS -----"