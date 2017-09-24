#!/bin/bash

args=$*

for i in $args;do
	if test -f $i
	then
		new=${i/.h/.c}
		touch $new
		sed 's/ /\n/' $i| sed 's/;/\n{ \n\t\n}\n/' > $new
	fi
done