#!/bin/sh
clear ;make tests
pushd ./test
./bin/test_Debug
popd
