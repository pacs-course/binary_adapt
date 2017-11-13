#!/bin/sh
clear ;make examples;
cd ./example/example$1;
#valgrind --leak-check=full ./bin/example_Debug
valgrind ./bin/example_Debug
cd ../..
