#!/bin/sh

#echo ============================= $1 =============================
for f in `cat package.desc`
do
#    echo gestisco $f
    #for ff in `ls $1/$f`
    for ff in $1/$f
    do
	    #echo dovrei gestire $ff
	    #echo dovrei copiare $ff in $2
	    if [ -e $ff ]
	    then
		#echo mi smazzo $ff
		mkdir -p $2/`dirname $ff`
		cp -r $ff $2/`dirname $ff`
	    fi
    done
done
