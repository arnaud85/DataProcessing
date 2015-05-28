while read line; do
	
	new_date=`date -u -d "${line:0:19}" +%s`
	other="${line:26}"

	echo "$new_date $other" >> orb.txt

done < maven_orbit_manu.txt