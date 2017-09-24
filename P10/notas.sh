#!/bin/bash
args=$*
n=$#

for i in `awk '{print $1}' $args| sort | uniq`; do

	if test `finger -s $i` -n 2> /dev/null; then
		continue
	else

		name=$(finger -s $i | egrep $i | awk '{print $2, $3, $4}'|sed 's/ /-/g')

		notas=$(cat $args | sort | grep "$i" |awk '{print $2}')
		suma=$(echo $notas|sed 's/ /+/g'|bc)
		media=$(echo 'scale=2;' $suma/$n | bc)
		matri=$(echo 'scale=2;' "$media > 9.99" | bc)
		sobre=$(echo 'scale=2;' "$media >= 9.0" | bc)
		not=$(echo 'scale=2;' "$media >= 7.0" | bc)
		bien=$(echo 'scale=2;' "$media >= 6.0" | bc)
		sufi=$(echo 'scale=2;' "$media >= 5.0" | bc)
		susp=$(echo 'scale=2;' "$media < 5.0" | bc)

		if [ $matri -eq 1 ]; then
			echo $name $media  "Matrícula"
		elif [ $sobre -eq 1 ]; then
			echo $name $media  "Sobresaliente"
		elif [ $not -eq 1 ]; then
			echo $name $media  "Notable"
		elif [ $bien -eq 1 ]; then
			echo $name $media "Aprobado"
		elif [ $sufi -eq 1 ]; then
			echo $name $media "Suficiente"
		elif [ $susp -eq 1 ]; then
			echo $name $media "Suspenso"
		fi

	fi

done