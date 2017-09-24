#!/bin/bash

args=$*
pidprog=$$

killprocess(){
	for j in $pids;do
		if [ "$j" != "$pidprog" ];
		then
			kill $j
		fi
	done
}

if [ $# == 0 ]; 
then
	echo "Usage: ./killprocs.sh [-n] <pid> <nameprocess>..."

elif [ "$1" == "-n" ];
then
	if [ "$2" == "" ];
	then
		echo "Usage: ./killprocs.sh [-n] <pid> <nameprocess>..."
	fi

	for i in $args; do
		if [ "$i" != "-n" ];
		then
			pids=$(ps ax | grep $i | awk '{print $1}')
			killprocess $pids
		fi
	done
	
else
	pids=$args
	killprocess $pids
fi
