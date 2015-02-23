#!/bin/bash

export DDBASE=/data1/DDBASE/DATA
export DDLIB=/usr/local/AMDA/DDLIB/lib64


##################################################### COMAG_4002 SYNC DATA ########################################
for dir in KRTP KSM KSO RTN; do
		
	ssh amda@cdpp1.cesr.fr "sh /home/mbouchemit/depotDECODER/env.sh;
	cd $DDBASE/CASSINI/MAG/1MIN/$dir/; 
	cp $DDBASE/LOCK .;
	rsync -arv amda@manunja.cesr.fr:/data/DDBASE/DATA/CASSINI/MAG/1MIN/$dir/ .; 
	$DDBIN/TimesUpdate -u mag_times.nc mag_[0-9]*.nc;
	php $DDBIN/UpdateInfo.php mag; 
	php $DECODER/TOOLS/DataBaseLog/updateDataBaseLog.php /data/DDBASE/DATA/CASSINI/MAG/1MIN/$dir;
	rm LOCK"
done
################################################ END OF SYNC DATA ########################################
