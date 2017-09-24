#!/bin/bash
args=$*

fich=$(find $args -type f -name '*.c' -o -name '*.h' -o -name '*.cpp') 
egrep -r '#include' $fich |awk '$0=$2' FS='<' RS='>'| sort -u