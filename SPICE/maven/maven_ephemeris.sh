#!/bin/bash

# Download last kernels
# sh download.sh

# Compute MAVEN orbit 
# ./maven_ephemeris "maven" "mars" "2014 DEC 01 00:00:00.000 UTC" "2014 DEC 02 00:00:00.000 UTC"
./maven_ephemeris "maven" "mars"

# Move to nc directory
mv *.txt plot/
mv maven_[0-9]*.nc nc
echo "[INFO] Maven orbit files have been created"

# Cut nc files : one file by year
cd nc

mkdir temp/
cp maven_[0-9]*.nc temp/

mkdir new/

nc2nc maven_[0-9]*.nc -b2014000 -e2015000
mv maven_2014[0-9]*.nc maven_2014.nc
mv maven_2014.nc new/
rm *.log *.rm

cp temp/maven_[0-9]*.nc .

nc2nc maven_[0-9]*.nc -b2015000 -e2016000
mv maven_2014[0-9]*.nc maven_2015.nc
mv maven_2015.nc new/
rm *.log *.rm

cp temp/maven_[0-9]*.nc .
cp new/* .

rm -r temp/
rm -r new/