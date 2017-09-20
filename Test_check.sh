#!/bin/sh
cd ./test;
if [ -f "test.log" ]; then
	echo Removing previous "test.log"
	rm test.log;
fi
./bin/test;
cd ..;
