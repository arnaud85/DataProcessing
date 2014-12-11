#! /bin/bash

libPath="/home/budnik/AMDANEW/DDLIB/lib"
prefix="mag"
mission="Cassini"
instrument="MAG"
res="1"


path="/data/DDBASE/DATA/CASSINI/MAG/1SEC/KRTP"
php setInfoFiles.php $path $libPath $prefix $mission $instrument $res

path="/data/DDBASE/DATA/CASSINI/MAG/1SEC/KSM"
php setInfoFiles.php $path $libPath $prefix $mission $instrument $res

path="/data/DDBASE/DATA/CASSINI/MAG/1SEC/KSO"
php setInfoFiles.php $path $libPath $prefix $mission $instrument $res

path="/data/DDBASE/DATA/CASSINI/MAG/1SEC/RTN"
php setInfoFiles.php $path $libPath $prefix $mission $instrument $res
