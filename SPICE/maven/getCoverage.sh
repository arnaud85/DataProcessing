#!/bin/bash

export SPICE="-I$LIB/cspice/include $LIB/cspice/lib/cspice.a"

#Compile
gcc -Wall getCoverage.c -o getCoverage -I$INCLUDE -L$LIB $SPICE -lm


#Execute 
./getCoverage