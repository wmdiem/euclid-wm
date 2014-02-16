#!/bin/bash


while read line

do
if [ "$line" = "" ];
 
then 
	echo -n $PATH | xargs -d : -I {} find {} -maxdepth 1 -executable -printf '%P\n' | sort -u 
	#ls /usr/bin | sort
	echo -e "\n"

elif [[ $line == exe* ]];

then 
	exec ${line:5:${#line}-5};


else

	#ls /usr/bin | grep "$line"
	echo -n $PATH | xargs -d : -I {} find {} -maxdepth 1 -executable -printf '%P\n' |  sort -u | grep -i $line
	echo -e "\n"

fi
done 
