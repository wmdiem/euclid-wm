#! /bin/sh

#this handler is meand to read a file containing options specified as $1 and print matches

if [ $2 ] 
then 

cat $1 | grep $2

else 

cat $1

fi
