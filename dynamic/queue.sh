#!/bin/sh


RESULTDIR=result/
h=`hostname`



if [ ! -e .passed_dynamic ];
then
    echo "You must pass all test before submitting"
    exit 1
fi

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi
    

# $THREADS
source ../params.sh

for thread in ${THREADS};
do
    sbatch --partition=Centaurus --time=03:00:00 --nodes=1 --ntasks-per-node=16 --export=ALL,THREAD=${thread} run_dynamic.sh
done 
