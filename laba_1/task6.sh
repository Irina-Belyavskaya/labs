#!/bin/bash
file_out=$1
catalog=$2
extension=$3
all_param=3
input_param=$#

if [ "$all_param" -eq "$input_param" ]
then 
	if [ -d "$catalog" ] 
	then 
		find $catalog -type f -name "*.$extension" | sort > $file_out.txt
		#files=$(sort $file_out.txt)
		#echo "${files}" > $file_out.txt
		#sort $file_out.txt > $file_out.txt
	else 
		echo "Directory does not exist" 1>&2
		echo "Parameter 1 - output file name" 1>&2
		echo "Parameter 2 - catalog name" 1>&2
		echo "Parameter 3 - extension of searched file" 1>&2
		
	fi

else 

	echo "Not enough parametrs" 1>&2
	echo "Parameter 1 - output file name" 1>&2
	echo "Parameter 2 - catalog name" 1>&2
	echo "Parameter 3 - extension of searched file" 1>&2

fi

exit 0
