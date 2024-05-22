#!/bin/bash

tcecc -O3 -o program.tpef fir.c -a task_2.adf --unroll-threshold=10000 --inline-threshold=10000
ttasim -a task_2.adf -p program.tpef -e"run; puts [info proc cycles];quit;"
