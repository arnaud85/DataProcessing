#!/bin/bash

# Compile
# gcc maven_ephemeris.c -o maven_ephemeris -I/home/arnaud/WORK/TOOLS/LIB/cspice/include -I/home/arnaud/WORK/TOOLS/INCLUDE /home/arnaud/WORK/TOOLS/LIB/cspice/lib/cspice.a -L/home/arnaud/WORK/TOOLS/LIB /home/arnaud/WORK/TOOLS/LIB/libnetcdf.so.7 -lDD -lm

# Download last version of MAVEN ephemride kernel
# wget http://naif.jpl.nasa.gov/pub/naif/MAVEN/kernels/spk/maven_orb_rec.bsp -P kernels/Maven
# mv kernels/Maven/maven_orb_rec.bsp.1 kernels/Maven/maven_orb_rec.bsp

# Compute MAVEN orbit 
# ./maven_ephemeris "maven" "mars" "2014 DEC 01 00:00:00.000 UTC" "2014 DEC 02 00:00:00.000 UTC"
./maven_ephemeris "maven" "mars"

# Move to nc directory
mv *.txt output/plot/
mv maven_[0-9]*.nc  output/nc
echo "[INFO] Maven orbit files have been created"

# Cut nc files : one file by year
cd output/nc

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