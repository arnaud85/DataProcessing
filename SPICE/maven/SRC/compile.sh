#!/bin/bash

# Configure
sh ~/WORK/TOOLS/SCRIPTS/SHELL/my_env.sh

# Compiling
SRC="maven_ephemeris.c"
EXE="maven_ephemeris"
CSPICE_INCLUDE="/home/arnaud/WORK/TOOLS/LIB/cspice/include"
CSPICE_LIB="/home/arnaud/WORK/TOOLS/LIB/cspice/lib/cspice.a"
NC_LIB="/home/arnaud/WORK/TOOLS/LIB/libnetcdf.so.7"
LDFLAG="-I${INCLUDE} ${CSPICE_LIB} -L${LIB} ${NC_LIB} -lDD -lm"
gcc ${SRC} -o ${EXE} -I${CSPICE_INCLUDE} ${LDFLAG}