#!/bin/sh
clear ;make tests
pushd ./test
valgrind  --leak-check=full ./bin/test_Debug
#valgrind --tool=callgrind -v --trace-children=yes ./bin/test_Debug;
#mv callgrind* ../;
popd
#kcachegrind
