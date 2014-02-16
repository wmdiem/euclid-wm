if [ $1 ]  

then 

echo -n $PATH | xargs -d : -I {} find {} -maxdepth 1 -executable -printf '%P\n' | sort -u | grep -i $1
 
else

echo -n $PATH | xargs -d : -I {} find {} -maxdepth 1 -executable -printf '%P\n' | sort -u 

fi
