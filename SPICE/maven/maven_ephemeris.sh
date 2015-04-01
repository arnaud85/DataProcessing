#!/bin/bash

# Compile
gcc maven_ephemeris.c -o maven_ephemeris -I/home/arnaud/WORK/TOOLS/LIB/cspice/include -I/home/arnaud/WORK/TOOLS/INCLUDE /home/arnaud/WORK/TOOLS/LIB/cspice/lib/cspice.a -L/home/arnaud/WORK/TOOLS/LIB /home/arnaud/WORK/TOOLS/LIB/libnetcdf.so.7 -lDD -lm

# Download last version of MAVEN ephemride kernel
# wget http://naif.jpl.nasa.gov/pub/naif/MAVEN/kernels/spk/maven_orb_rec.bsp -P kernels/Maven
# mv kernels/Maven/maven_orb_rec.bsp.1 kernels/Maven/maven_orb_rec.bsp

# Compute MAVEN orbit 
./maven_ephemeris "maven" "mars" "2014 DEC 01 00:00:00.000 UTC" "2014 DEC 02 00:00:00.000 UTC"
# ./maven_ephemeris "maven" "mars"

# Move to nc directory
mv *.txt output/plot/
mv maven_[0-9]*.nc  output/nc
echo "[INFO] Maven orbit files have been created"