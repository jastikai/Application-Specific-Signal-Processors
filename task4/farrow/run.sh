#!/bin/bash

tcecc -O3 -o program.tpef farrow.c -a task_4.adf --unroll-threshold=10000 --inline-threshold=10000
ttasim -a task_4.adf -p program.tpef -e"run; puts [info proc cycles];quit;"
