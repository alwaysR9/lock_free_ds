#!/usr/bin/env bash

rm log/output
rm core

NUM=1000
for i in `seq 1 ${NUM}`; do
    echo "====== $i ======" > log/output
    ./build/x >> log/output
    N=$(cat log/output |grep "successfully" |wc -l)
    if [ ${N} -ne 5 ]; then
        echo "ERROR"
        exit
    fi
done

echo "----- BATCH TEST SUCCESS -----"