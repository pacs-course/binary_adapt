#!/bin/sh
clear ;make tests;
cd ./test;
valgrind  --leak-check=full ./bin/test_Debug
#valgrind --tool=callgrind -v --trace-children=yes ./bin/test_Debug;
#mv callgrind* ../;
cd ..;
#kcachegrind
