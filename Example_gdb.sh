#!/bin/sh
clear ;make examples;
cd ./example/example$1;
gdb ./bin/example_Debug
cd ../..
