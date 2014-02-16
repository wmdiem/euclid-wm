#! /bin/bash

#this is a test script
#if [ -e ~/.cache/euclid-menu/handlers/c.cache ] 
#	then
#		echo -e "`cat ~/.cache/euclid-menu/handlers/c.cache`\n"
#fi
while read line 
do 
	# if we get an empty line show history if it exists
	if  [[ "$(echo "$line" | tr -d ' ')" == "" ]] 
	then
		if [[ -e ~/.cache/euclid-menu/handlers/c.cache ]] 
		then 

			echo -e "`cat ~/.cache/euclid-menu/handlers/c.cache`\n\n"
		else 
			echo -e "\n\n"
		fi

	#otherwise try to calcuate if possible, as long as we aren't getting an exec
	elif [[ "$(echo $line | grep "exec")" == "" ]] 
	then
		A=`echo "$line" | tr -d ' ' | bc -l`
		if [ $A ] ;
		then  
			echo -e "$A\n\n"
			
			#update history file
			rm ~/.cache/euclid-menu/handlers/c.cache.tmp
			#the awk one-liner magically removes duplicates without sorting, by treating the current line as an index
			cat ~/.cache/euclid-menu/handlers/c.cache | awk '!a[$0]++' > ~/.cache/euclid-menu/handlers/c.cache.tmp
			echo -e "$line = $A \t($line)" > ~/.cache/euclid-menu/handlers/c.cache
			sed -n "1,99 p" ~/.cache/euclid-menu/handlers/c.cache.tmp >>~/.cache/euclid-menu/handlers/c.cache

		#	echo -e "$line = $A \t($line)" >> ~/.cache/euclid-menu/handlers/c.cache
		#	echo "\n"
	
		else 
			#check whether anything in history matches:
			#need to escape *
			grep $(echo "$line" | tr -d ' ' | sed 's/\*/\\*/g') ~/.cache/euclid-menu/handlers/c.cache
		

			echo -e "\n"
		
		fi
	fi
done

#trim history file


#[[ $(cat ~/.cache/euclid-menu/handlers/c.cache | wc -l) -gt 100 ]] && sed -i "1,100 d" ~/.cache/euclid-menu/handlers/c.cache
