#!/bin/bash

# Configuration
NAIF_KERNELS_PATH="http://naif.jpl.nasa.gov/pub/naif/MAVEN/kernels"
LOCAL_KERNELS_PATH="kernels/Maven"
SPK=${NAIF_KERNELS_PATH}/spk
FK=${NAIF_KERNELS_PATH}/fk
MAVEN_UPDATE="maven_orb_rec.bsp"
# MAVEN_ARCHIVE_2015="maven_orb_rec_150101_150401_v1.bsp"
# MAVEN_ARCHIVE_2014="maven_orb_rec_140922_150101_v1.bsp"
MAVEN_TF_UPDATE="maven_v06.tf"

# Download last version of MAVEN ephemride kernel
rm ${LOCAL_KERNELS_PATH}/*
wget ${SPK}/${MAVEN_UPDATE} -P ${LOCAL_KERNELS_PATH}
# wget ${SPK}/${MAVEN_ARCHIVE_2015} -P ${LOCAL_KERNELS_PATH}
# wget ${SPK}/${MAVEN_ARCHIVE_2014} -P ${LOCAL_KERNELS_PATH}
wget ${FK}/${MAVEN_TF_UPDATE} -P ${LOCAL_KERNELS_PATH}

# Write maven_ephemeris.tm file
echo "\begindata

	KERNELS_TO_LOAD = (

		'kernels/general/naif0011.tls'
		'kernels/general/de430.bsp'
		'kernels/general/pck00010.tpc'
		'kernels/general/mar097s.bsp'
		'kernels/Maven/${MAVEN_UPDATE}'
		'kernels/Maven/${MAVEN_TF_UPDATE}'
	)

\begintext" > maven_ephemeris.tm