#!/bin/bash

# Check args
if [[ $# -ne 1 ]]; then
	
	echo "[USAGE] sh $0 YEAR"

	exit 0
fi


#Compile
# gcc maven_ephemeris.c -o maven_ephemeris -I/home/arnaud/WORK/TOOLS/LIB/cspice/include -I/home/arnaud/WORK/TOOLS/INCLUDE /home/arnaud/WORK/TOOLS/LIB/cspice/lib/cspice.a -L/home/arnaud/WORK/TOOLS/LIB /home/arnaud/WORK/TOOLS/LIB/libnetcdf.so.7 -lDD -lm

# Download last version of MAVEN ephemride kernel
# wget http://naif.jpl.nasa.gov/pub/naif/MAVEN/kernels/spk/maven_orb_rec.bsp -P kernels/Maven
# mv kernels/Maven/maven_orb_rec.bsp.1 kernels/Maven/maven_orb_rec.bsp

#Execute 
./maven_ephemeris "MAVEN" "MARS" "maven" "$1 JAN 01 00:00:00 UTC"

#Move to nc directory
mv *.txt output/plot/
mv maven_[0-9]*.nc  output/nc/maven_$1.nc
echo "[INFO] maven_$1.nc has been created"