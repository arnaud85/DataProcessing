while read line; do
	
	new_date="${line:0:8}00"
	other="${line:14}"

	echo "$new_date $other" >> new.txt

done < maven_orbit_arnaud.txt