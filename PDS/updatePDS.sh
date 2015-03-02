#!/bin/bash
# Version: 2015-02-25 Arnaud BIEGUN $

if [[ $# -ne 1 ]]; then
	
	echo "[USAGE] sh $0 PDS_dataset_path"

	exit 0
fi

# Configures paths
PDS_dataset_path=$1
JAR_path=/home/arnaud/WORK/TOOLS/LIB/mimic
JAVA_path=/usr/lib/jvm/jre/lib/ext/

# Go to dataset directory
cd $1

# Updates local copy
java -Djava.ext.dirs="${JAR_path}:${JAVA_path}" igpp.mimic.Refresh -v -r

# Download if needed
java -Djava.ext.dirs="${JAR_path}:${JAVA_path}" igpp.mimic.Pull -v