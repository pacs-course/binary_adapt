#!/bin/bash
clear ;make example$1
pushd ./example/example$1
gdb ./bin/example_Debug $2
popd
