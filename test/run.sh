#!/bin/sh
clear ;make
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../lib; \
#echo $LD_LIBRARY_PATH;\
./bin/test
