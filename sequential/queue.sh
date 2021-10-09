#!/bin/sh
sbatch --partition=Centaurus --time=02:00:00 --nodes=1 --ntasks-per-node=16 bench_sequential.sh
