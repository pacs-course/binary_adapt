#!/bin/sh
clear ;make;
cd ./example/example$1;
gdb ./bin/example_Debug
cd ../..
