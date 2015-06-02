DDBASE="amda@cdpp1.cesr.fr"
DD_PATH="/data1/DDBASE/DATA/MAVEN/ORB"
PROJECT_PATH="/home/arnaud/WORK/TOOLS/DEV/DataProcessing/SPICE/maven/nc"
BIN="/usr/local/AMDA/DDLIB/bin"
PREFIX="orb"

while [[ 1 ]]; do
	
	if [[ -e "ready" ]]; then

		for file in ${PROJECT_PATH}/*; do
	
			filename=$( basename ${file} )
			date=${filename:6:16}
			
			scp ${PROJECT_PATH}/maven_${date}.nc ${DDBASE}:${DD_PATH}/${PREFIX}_${date}.nc
		done

		ssh ${DDBASE} "cd ${DD_PATH}; TimesUpdate -u ${PREFIX}_times.nc ${PREFIX}_[0-9]*.nc; php ${BIN}/UpdateInfo.php ${PREFIX}"; 

		rm ready
		
		exit 0;

	else
		sleep 2;
	fi
	
done