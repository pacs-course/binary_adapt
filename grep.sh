#!/bin/sh
grep -r --exclude-dir=doc --exclude-dir=bin --exclude-dir=lib --exclude-dir=obj --color=always "$1";
