#!/bin/bash

#Clean
clear
clear

#Compile
gcc maven_ephemeris.c -o maven_ephemeris -I/home/arnaud/WORK/TOOLS/LIB/cspice/include -I/home/arnaud/WORK/TOOLS/INCLUDE /home/arnaud/WORK/TOOLS/LIB/cspice/lib/cspice.a -L/home/arnaud/WORK/TOOLS/LIB /home/arnaud/WORK/TOOLS/LIB/libnetcdf.so.7 -lDD -lm

# Download last version of MAVEN ephemride kernel
# wget http://naif.jpl.nasa.gov/pub/naif/MAVEN/kernels/spk/maven_orb_rec.bsp -P kernels/Maven
# mv kernels/Maven/maven_orb_rec.bsp.1 kernels/Maven/maven_orb_rec.bsp

#Execute 
# ./maven_ephemeris "MAVEN" "MARS" "maven" "2014 SEP 23 13:03:46 UTC" "2015 MAR 03 16:08:21 UTC"	2015
./maven_ephemeris "MAVEN" "MARS" "maven" "2014 SEP 23 13:03:46 UTC" "2015 JAN 01 00:01:00 UTC"

#Move to nc directory
mv *.txt output/plot/
mv *.nc  output/nc/