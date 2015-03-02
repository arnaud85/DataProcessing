#! /bin/bash


######################### USAGE ###############################
if test $# -ne 1
then
	
	echo "[ERROR] USAGE : sh update.sh PDS_VOLUME"
	exit 0

else

	PDS_data="$1"
	local_dir="/home/arnaud/WORK/DATA/$1/"	
	
	echo "[INFO] PDS_VOLUME : $PDS_data"
	echo "[INFO] LOCAL_DIR  : $local_dir"
fi

################### END OF USAGE ###############################




############ DOWNLOAD DATA ###########

php downloadPDS.php $PDS_data $local_dir

######## END OF DOWNLOAD DATA ######## 



####################### PDS VOLUME ############################
if test "$PDS_data" == "COMAG_4002"
then
	
	# Clean
	rm $local_dir/KRTP/*.TAB
	rm $local_dir/KRTP/*.LBL
	rm $local_dir/KSM/*.TAB
	rm $local_dir/KSM/*.LBL
	rm $local_dir/KSO/*.TAB
	rm $local_dir/KSO/*.LBL
	rm $local_dir/RTN/*.TAB
	rm $local_dir/RTN/*.LBL

	# Push files into right directory
	mv $local_dir/*_KRTP* $local_dir/KRTP/
	mv $local_dir/*_KSM* $local_dir/KSM/
	mv $local_dir/*_KSO* $local_dir/KSO/
	mv $local_dir/*_RTN* $local_dir/RTN/

	# Convert data into netCDF format
	for dir in KRTP KSM KSO RTN
	do
		rm $local_dir/$dir/nc/*
		sh convert2nc.sh $PDS_data $local_dir/$dir
	done
	
	#Move nc file to AMDA database
	for dir in KRTP KSM KSO RTN; do
		
		rsync -arv $local_dir/$dir/nc/ amda@manunja.cesr.fr:/data/DDBASE/DATA/CASSINI/MAG/1MIN/$dir
	done
	
	
	#Set time update
	for dir in KRTP KSM KSO RTN; do
		
		ssh amda@manunja.cesr.fr "cd /data/DDBASE/DATA/CASSINI/MAG/1MIN/$dir; 
		cp /data/DDBASE/DATA/LOCK .; 
		/home/budnik/AMDANEW/DDLIB/bin/TimesUpdate -u mag_times.nc mag_[0-9]*.nc;
		php /home/budnik/AMDANEW/DDLIB/bin/UpdateInfo.php mag; 
		php /home/budnik/depotDECODER/TOOLS/DataBaseLog/updateDataBaseLog.php /data/DDBASE/DATA/CASSINI/MAG/1MIN/$dir;
		rm LOCK"
	done

	

elif test "$PDS_data" == "COMAG_4001"
then

	rm $2/KRTP/*.TAB
	rm $2/KRTP/*.LBL
	rm $2/KSM/*.TAB
	rm $2/KSM/*.LBL
	rm $2/KSO/*.TAB
	rm $2/KSO/*.LBL
	rm $2/RTN/*.TAB
	rm $2/RTN/*.LBL

	mv $local_dir/*_KRTP* $local_dir/KRTP/
	mv $local_dir/*_KSM* $local_dir/KSM/
	mv $local_dir/*_KSO* $local_dir/KSO/
	mv $local_dir/*_RTN* $local_dir/RTN/

	# Convert data into netCDF format
	for dir in KRTP KSM KSO RTN
	do
		rm $local_dir/$dir/nc/*
		sh convert2nc.sh $PDS_data $local_dir/$dir
	done
	
	#Move nc file to AMDA database
	for dir in KRTP KSM KSO RTN; do
		
		rsync -arv $local_dir/$dir/nc/ amda@manunja.cesr.fr:/data/DDBASE/DATA/CASSINI/MAG/1SEC/$dir
	done

	#Set time update
	for dir in KRTP KSM KSO RTN; do
		
		ssh amda@manunja.cesr.fr "cd /data/DDBASE/DATA/CASSINI/MAG/1SEC/$dir; 
		cp /data/DDBASE/DATA/LOCK .; 
		/home/budnik/AMDANEW/DDLIB/bin/TimesUpdate -u mag_times.nc mag_[0-9]*.nc;
		php /home/budnik/AMDANEW/DDLIB/bin/UpdateInfo.php mag; 
		php /home/budnik/depotDECODER/TOOLS/DataBaseLog/updateDataBaseLog.php /data/DDBASE/DATA/CASSINI/MAG/1SEC/$dir;
		rm LOCK"
	done



elif test "$PDS_data" == "MESSMAGDATA_3001"
then

	cd $local_dir

	cp *_60_* 1M/
	cp *_01_* 1S/

	rm *_60_*
	rm *_01_*

	# Convert files to netCDF format
	i=0;
	for file in 1M/MAGMSO*.LBL
	do
		days_60[$i]=${file:15:5}
		let i++
	done

	i=0;
	for file in 1S/MAGMSO*.LBL
	do
		days_01[$i]=${file:15:5}
		let i++
	done

	cd 1M/

	i=0
	for i in "${!days_60[@]}"
	do 
		# echo "Key : $i; Value : ${days_60[$i]}"
		convert2nc MESSENGER_MAG.xml MAGMSOSCIAVG${days_60[$i]}_60_V05.LBL MAGMBFSCIAVG${days_60[$i]}_60_V05.LBL MAGRTNSCIAVG${days_60[$i]}_60_V05.LBL
	done

	cd ../1S/

	i=0
	for i in "${!days_01[@]}"
	do 
		# echo "Key : $i; Value : ${days_01[$i]}"
		convert2nc MESSENGER_MAG.xml MAGMSOSCIAVG${days_01[$i]}_01_V05.LBL MAGMBFSCIAVG${days_01[$i]}_01_V05.LBL MAGRTNSCIAVG${days_01[$i]}_01_V05.LBL
	done

	cd ..
	
	#Move nc file to AMDA database
	rsync -arv $local_dir/1M/nc/ amda@manunja.cesr.fr:/data/DDBASE/DATA/MES/MAG/ORBIT/1M
	rsync -arv $local_dir/1S/nc/ amda@manunja.cesr.fr:/data/DDBASE/DATA/MES/MAG/ORBIT/1S

	#Configure info files : prefix_times, prefix_info, prefix_cache
	ssh amda@manunja.cesr.fr bash < remote_MESSMAGDATA_3001.sh

else

	echo "[ERROR] PDS volume $1 is unknown"
	exit 0
fi

####################### END OF PDS VOLUME ############################

