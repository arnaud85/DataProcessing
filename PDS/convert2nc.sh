#! /bin/bash
cd $2

for file in *.LBL
do
	# echo ${file}
	convert2nc $2/$1.xml ${file}
done