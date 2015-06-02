#!/bin/bash

# Configuration
LIB_PATH="/home/arnaud/WORK/TOOLS/LIB/"
# MAVEN_ARCHIVE_2014="kernels/Maven/maven_orb_rec_140922_150101_v1.bsp" 
# MAVEN_ARCHIVE_2015="kernels/Maven/maven_orb_rec_150101_150401_v1.bsp"
MAVEN_UPDATE="kernels/Maven/maven_orb_rec.bsp"
export LD_LIBRARY_PATH=${LIB_PATH}

# Get last Spice's kernels
sh getSpiceKernels.sh

# Compute MAVEN orbit
# echo "[INFO] KERNEL : ${MAVEN_ARCHIVE_2014}"
# ./maven_ephemeris ${MAVEN_ARCHIVE_2014} "maven" "mars"
# echo "[INFO] KERNEL : ${MAVEN_ARCHIVE_2015}"
# ./maven_ephemeris ${MAVEN_ARCHIVE_2015} "maven" "mars"
echo "[INFO] KERNEL : ${MAVEN_UPDATE}"
./maven_ephemeris ${MAVEN_UPDATE} "maven" "mars"

# Test if the maven orbit data file has been correctly created
ls *.nc >> /dev/null 2>&1

if [[ $? != 0 ]]; then
	
	echo "$(date) [ERROR] : Error during creation of maven orbit data file" >> LOG/log.txt
	exit 1;
fi
	
# Move to outut directory
mv *.txt plot/
mv maven_[0-9]*.nc nc/

# End of process
touch ready