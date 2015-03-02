#!/bin/bash

#Clean
rm ephemeris 
clear
clear

#Compile
gcc ephemeris.c -o ephemeris -I/home/arnaud/WORK/TOOLS/LIB/cspice/include -I/home/arnaud/WORK/TOOLS/INCLUDE /home/arnaud/WORK/TOOLS/LIB/cspice/lib/cspice.a -L/home/arnaud/WORK/TOOLS/LIB /home/arnaud/WORK/TOOLS/LIB/libnetcdf.so.7 -lDD -lm

#Execute
./ephemeris "1" "SUN" "mercury" "1970-01-01T00:00:00" "1970-02-01T00:00:00" 		#MERCURY
./ephemeris "2" "SUN" "venus" "1970-01-01T00:00:00" "1970-02-01T00:00:00"			#VENUS
./ephemeris "3" "SUN" "earth" "1970-01-01T00:00:00" "1970-02-01T00:00:00"			#EARTH
./ephemeris "4" "SUN" "mars" "1970-01-01T00:00:00" "1970-02-01T00:00:00" 			#MARS
./ephemeris "5" "SUN" "jupiter" "1970-01-01T00:00:00" "1970-02-01T00:00:00"			#JUPITER
./ephemeris "6" "SUN" "saturn" "1970-01-01T00:00:00" "1970-02-01T00:00:00"			#SATURN
./ephemeris "7" "SUN" "uranus" "1970-01-01T00:00:00" "1970-02-01T00:00:00"			#URANUS
./ephemeris "8" "SUN" "neptune" "1970-01-01T00:00:00" "1970-02-01T00:00:00"			#NEPTUNE
./ephemeris "9" "SUN" "pluto" "1970-01-01T00:00:00" "1970-02-01T00:00:00"			#PLUTO

#Move to test directory
mv *.nc nc/