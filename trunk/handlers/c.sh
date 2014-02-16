#! /bin/sh

#this is a test scripts
A=`echo $@ | bc -l`
if [ $A ] 
	then  
	echo $A
fi

