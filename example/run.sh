#!/bin/sh
clear
make
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../lib
./bin/example
