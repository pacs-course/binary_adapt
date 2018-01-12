#!/bin/bash
clear ;make example$1
pushd ./example/example$1
valgrind --leak-check=full ./bin/example_Debug $2
#valgrind --tool=callgrind -v --trace-children=yes ./bin/example_Debug $2
#mv callgrind* ../..;
popd
#kcachegrind

