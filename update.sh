#! /bin/bash

#Usage
if test $# -ne 2
then
	
	echo "[ERROR] USAGE : sh update.sh (PDS_VOLUME) (LOCAL_DIR_PATH)"
	exit 0

else

	echo "[INFO] PDS_VOLUME : $1"
	echo "[INFO] LOCAL_DIR_PATH : $2"
fi

# DL data
PDS_VOLUME=$1
LOCAL_DIR_PATH=$2

php download_parse_html.php $1 $2

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
if test "$1" == "COMAG_4002"
then
	
	rsync -arv $2/KRTP/nc/ amda@manunja.cesr.fr:/data/DDBASE/DATA/CASSINI/MAG/1MIN/KRTP
	ssh amda@manunja.cesr.fr bash < remote.sh

	rsync -arv $2/KSM/nc/ amda@manunja.cesr.fr:/data/DDBASE/DATA/CASSINI/MAG/1MIN/KSM
	ssh amda@manunja.cesr.fr bash < remote.sh

	rsync -arv $2/KSO/nc/ amda@manunja.cesr.fr:/data/DDBASE/DATA/CASSINI/MAG/1MIN/KSO
	ssh amda@manunja.cesr.fr bash < remote.sh

	rsync -arv $2/RTN/nc/ amda@manunja.cesr.fr:/data/DDBASE/DATA/CASSINI/MAG/1MIN/RTN
	ssh amda@manunja.cesr.fr bash < remote.sh

else

	echo "[ERROR] PDS volume is unknown"
	exit 0
fi

