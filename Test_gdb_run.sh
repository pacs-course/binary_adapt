#!/bin/sh
clear ;make tests
pushd ./test
gdb ./bin/test_Debug
popd

