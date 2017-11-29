#!/bin/sh
clear ;make examples;
cd ./example/example$1;
valgrind --leak-check=full ./bin/example_Debug
#valgrind --tool=callgrind -v --trace-children=yes ./bin/example_Debug;
#mv callgrind* ../..;
cd ../..
#kcachegrind

