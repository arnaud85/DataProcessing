#! /bin/bash


######################### USAGE ###############################
if test $# -ne 2
then
	
	echo "[ERROR] USAGE : sh update.sh (PDS_VOLUME) (LOCAL_DIR_PATH)"
	exit 0

else

	echo "[INFO] PDS_VOLUME : $1"
	echo "[INFO] LOCAL_DIR_PATH : $2"
fi

################### END OF USAGE ###############################




############ DOWNLOAD DATA ###########

# php downloadPDS.php $1 $2

######## END OF DOWNLOAD DATA ######## 



####################### PDS VOLUME ############################
if test "$1" == "COMAG_4002"
then
	
	rm $2/KRTP/*.TAB
	rm $2/KRTP/*.LBL
	rm $2/KSM/*.TAB
	rm $2/KSM/*.LBL
	rm $2/KSO/*.TAB
	rm $2/KSO/*.LBL
	rm $2/RTN/*.TAB
	rm $2/RTN/*.LBL

	mv $2/*_KRTP* $2/KRTP/
	mv $2/*_KSM* $2/KSM/
	mv $2/*_KSO* $2/KSO/
	mv $2/*_RTN* $2/RTN/

	# Convert data into netCDF format
	for dir in KRTP KSM KSO RTN
	do
		rm $2/$dir/nc/*
		sh convert2nc.sh $1 $2/$dir
	done
	
	#Move nc file to AMDA database
	rsync -arv $2/KRTP/nc/ amda@manunja.cesr.fr:/data/DDBASE/DATA/CASSINI/MAG/1MIN/KRTP
	rsync -arv $2/KSM/nc/ amda@manunja.cesr.fr:/data/DDBASE/DATA/CASSINI/MAG/1MIN/KSM
	rsync -arv $2/KSO/nc/ amda@manunja.cesr.fr:/data/DDBASE/DATA/CASSINI/MAG/1MIN/KSO
	rsync -arv $2/RTN/nc/ amda@manunja.cesr.fr:/data/DDBASE/DATA/CASSINI/MAG/1MIN/RTN
	
	#Configure info files : prefix_times, prefix_info, prefix_cache
	ssh amda@manunja.cesr.fr bash < remote_COMAG_4002.sh

elif test "$1" == "COMAG_4001"
then

	#rm $2/KRTP/*.TAB
	#rm $2/KRTP/*.LBL
	#rm $2/KSM/*.TAB
	#rm $2/KSM/*.LBL
	#rm $2/KSO/*.TAB
	#rm $2/KSO/*.LBL
	#rm $2/RTN/*.TAB
	#rm $2/RTN/*.LBL

	mv $2/*_KRTP* $2/KRTP/
	mv $2/*_KSM* $2/KSM/
	mv $2/*_KSO* $2/KSO/
	mv $2/*_RTN* $2/RTN/

	# Convert data into netCDF format
	for dir in KRTP KSM KSO RTN
	do
		rm $2/$dir/nc/*
		sh convert2nc.sh $1 $2/$dir
	done
	
	#Move nc file to AMDA database
	rsync -arv $2/KRTP/nc/ amda@manunja.cesr.fr:/data/DDBASE/DATA/CASSINI/MAG/1SEC/KRTP
	rsync -arv $2/KSM/nc/ amda@manunja.cesr.fr:/data/DDBASE/DATA/CASSINI/MAG/1SEC/KSM
	rsync -arv $2/KSO/nc/ amda@manunja.cesr.fr:/data/DDBASE/DATA/CASSINI/MAG/1SEC/KSO
	rsync -arv $2/RTN/nc/ amda@manunja.cesr.fr:/data/DDBASE/DATA/CASSINI/MAG/1SEC/RTN

	#Configure info files : prefix_times, prefix_info, prefix_cache
	ssh amda@manunja.cesr.fr bash < remote_COMAG_4001.sh



elif test "$1" == "MESSMAGDATA_3001"
then

	cd $2

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
	rsync -arv $2/1M/nc/ amda@manunja.cesr.fr:/data/DDBASE/DATA/MES/MAG/ORBIT/1M
	rsync -arv $2/1S/nc/ amda@manunja.cesr.fr:/data/DDBASE/DATA/MES/MAG/ORBIT/1S

	#Configure info files : prefix_times, prefix_info, prefix_cache
	ssh amda@manunja.cesr.fr bash < remote_MESSMAGDATA_3001.sh

else

	echo "[ERROR] PDS volume $1 is unknown"
	exit 0
fi

####################### END OF PDS VOLUME ############################

