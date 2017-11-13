#!/bin/sh

for f in `cat package.desc`
do
	for ff in $1/$f
	do
		if [ -e $ff ]
		then
			mkdir -p $2/`dirname $ff`
			cp -r $ff $2/`dirname $ff`
		fi
	done
done
