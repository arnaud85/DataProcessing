#! /bin/bash

libPath="/home/budnik/AMDANEW/DDLIB/lib"
prefix="mag"
mission="MESSENGER"
instrument="MAG"

res="60"

path="/data/DDBASE/DATA/MES/MAG/ORBIT/1M"
php setInfoFiles.php $path $libPath $prefix $mission $instrument $res

res="1"

path="/data/DDBASE/DATA/MES/MAG/ORBIT/1S"
php setInfoFiles.php $path $libPath $prefix $mission $instrument $res


