#!/bin/bash

#Clean
rm ephemeris 
clear
clear

#Compile
#gcc maven_ephemeris.c -o maven_ephemeris -I/home/arnaud/WORK/TOOLS/LIB/cspice/include -I/home/arnaud/WORK/TOOLS/INCLUDE /home/arnaud/WORK/TOOLS/LIB/cspice/lib/cspice.a -L/home/arnaud/WORK/TOOLS/LIB /home/arnaud/WORK/TOOLS/LIB/libnetcdf.so.7 -lDD -lm

# Download last version of MAVEN ephemride kernel
# wget http://naif.jpl.nasa.gov/pub/naif/MAVEN/kernels/spk/maven_orb.bsp -P kernels/Maven/
# wget http://naif.jpl.nasa.gov/pub/naif/MAVEN/kernels/spk/trj_orb_00777-00779_00939_v1.bsp -P kernels/Maven/
#wget http://naif.jpl.nasa.gov/pub/naif/MAVEN/kernels/spk/maven_orb_rec.bsp -P kernels/Maven
# wget http://naif.jpl.nasa.gov/pub/naif/MAVEN/kernels/spk/mar097s.bsp -P kernels/general


#Execute 
./maven_ephemeris "MAVEN" "MARS" "maven" "2014 DEC 31 13:03:46" "2015 JAN 24 17:21:49" 		


#Move to nc directory
mv *.txt output/
mv *.nc nc/