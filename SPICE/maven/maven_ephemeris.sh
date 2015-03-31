#!/bin/bash

# Check args
# if [[ $# -ne 1 ]]; then
	
# 	echo "[USAGE] sh $0 YEAR"

# 	exit 0
# fi


# Compile
gcc maven_ephemeris.c -o maven_ephemeris -I/home/arnaud/WORK/TOOLS/LIB/cspice/include -I/home/arnaud/WORK/TOOLS/INCLUDE /home/arnaud/WORK/TOOLS/LIB/cspice/lib/cspice.a -L/home/arnaud/WORK/TOOLS/LIB /home/arnaud/WORK/TOOLS/LIB/libnetcdf.so.7 -lDD -lm

# Download last version of MAVEN ephemride kernel
# wget http://naif.jpl.nasa.gov/pub/naif/MAVEN/kernels/spk/maven_orb_rec.bsp -P kernels/Maven
# mv kernels/Maven/maven_orb_rec.bsp.1 kernels/Maven/maven_orb_rec.bsp

# Compute MAVEN orbit 
# ./maven_ephemeris "MAVEN" "MARS" "maven" "$1 JAN 01 00:00:00 UTC"
# ./maven_ephemeris "MAVEN" "MARS" "maven" "2014 SEP 22 02:43:52 UTC" "2015 MAR 19 16:59:59.182"
./maven_ephemeris "maven" "mars"

# Move to nc directory
mv *.txt output/plot/
mv maven_[0-9]*.nc  output/nc
echo "[INFO] Maven orbit files have been created"