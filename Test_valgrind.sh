#!/bin/sh
clear ;make;
cd ./test;
valgrind --leak-check=full ./bin/test_Debug
#valgrind ./bin/test_Debug
cd ..;
