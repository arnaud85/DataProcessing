#! /bin/bash

libPath="/home/budnik/AMDANEW/DDLIB/lib"
prefix="mag"
mission="Cassini"
instrument="MAG"
res="60"


path="/data/DDBASE/DATA/CASSINI/MAG/1MIN/KRTP"
php setInfoFiles.php $path $libPath $prefix $mission $instrument $res

path="/data/DDBASE/DATA/CASSINI/MAG/1MIN/KSM"
php setInfoFiles.php $path $libPath $prefix $mission $instrument $res

path="/data/DDBASE/DATA/CASSINI/MAG/1MIN/KSO"
php setInfoFiles.php $path $libPath $prefix $mission $instrument $res

path="/data/DDBASE/DATA/CASSINI/MAG/1MIN/RTN"
php setInfoFiles.php $path $libPath $prefix $mission $instrument $res
