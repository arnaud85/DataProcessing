#!/bin/bash

#rm earth_1970*.nc
years=("1970" "1971" "1972" "1973" "1974" "1975" "1976" "1977" "1978" "1979" "1980" "1981" "1982" "1983" "1984" "1985" "1986" "1987" "1988" "1989" "1990"
"1991" "1992" "1993" "1994" "1995" "1996" "1997" "1998" "1999" "2000" "2001" "2002" "2003" "2004" "2005" "2006" "2007" "2008" "2009" "2010"
"2011" "2012" "2013" "2014" "2015" "2016" "2017" "2018" "2019" "2020" "2021" "2022" "2023")

startTime=("01" "02" "03" "04" "05" "06" "07" "08" "09" "10" "11" "12")
stopTime=("02" "03" "04" "05" "06" "07" "08" "09" "10" "11" "12" "01")

#Clean
rm ephemeris 
clear
clear

#Compile
gcc ephemeris.c -o ephemeris -I/home/arnaud/WORK/TOOLS/LIB/cspice/include -I/home/arnaud/WORK/TOOLS/INCLUDE /home/arnaud/WORK/TOOLS/LIB/cspice/lib/cspice.a -L/home/arnaud/WORK/TOOLS/LIB /home/arnaud/WORK/TOOLS/LIB/libnetcdf.so.7 -lDD -lm

#Execute
for body_index in "1" "2" "3" "4" "5" "6" "7" "8" "9";
do
	for j in `seq 0 53`;
	do
		for i in "0" "1" "2" "3" "4" "5" "6" "7" "8" "9" "10";
		do
			if [[ $body_index -eq 1 ]]; then
				./ephemeris $body_index "SUN" "mercury" "${years[j]}-${startTime[i]}-01T00:00:00" "${years[j]}-${stopTime[i]}-01T00:00:00" 
			elif [[ $body_index -eq 2 ]]; then
				./ephemeris $body_index "SUN" "venus" "${years[j]}-${startTime[i]}-01T00:00:00" "${years[j]}-${stopTime[i]}-01T00:00:00"
			elif [[ $body_index -eq 3 ]]; then
				./ephemeris $body_index "SUN" "earth" "${years[j]}-${startTime[i]}-01T00:00:00" "${years[j]}-${stopTime[i]}-01T00:00:00"
			elif [[ $body_index -eq 4 ]]; then
				./ephemeris $body_index "SUN" "mars" "${years[j]}-${startTime[i]}-01T00:00:00" "${years[j]}-${stopTime[i]}-01T00:00:00"
			elif [[ $body_index -eq 5 ]]; then
				./ephemeris $body_index "SUN" "jupiter" "${years[j]}-${startTime[i]}-01T00:00:00" "${years[j]}-${stopTime[i]}-01T00:00:00"
			elif [[ $body_index -eq 6 ]]; then
				./ephemeris $body_index "SUN" "saturn" "${years[j]}-${startTime[i]}-01T00:00:00" "${years[j]}-${stopTime[i]}-01T00:00:00"
			elif [[ $body_index -eq 7 ]]; then
				./ephemeris $body_index "SUN" "uranus" "${years[j]}-${startTime[i]}-01T00:00:00" "${years[j]}-${stopTime[i]}-01T00:00:00"
			elif [[ body_index -eq 8 ]]; then
				./ephemeris $body_index "SUN" "neptune" "${years[j]}-${startTime[i]}-01T00:00:00" "${years[j]}-${stopTime[i]}-01T00:00:00"
			elif [[ $body_index -eq 9 ]]; then
				./ephemeris $body_index "SUN" "pluto" "${years[j]}-${startTime[i]}-01T00:00:00" "${years[j]}-${stopTime[i]}-01T00:00:00"
			fi
		done
		
		let "year2 = ${years[j]} + 1"
		if [[ $body_index -eq 1 ]]; then
			./ephemeris $body_index "SUN" "mercury" "${years[j]}-${startTime[11]}-01T00:00:00" "$year2-${stopTime[11]}-01T00:00:00" 
		elif [[ $body_index -eq 2 ]]; then
			./ephemeris $body_index "SUN" "venus" "${years[j]}-${startTime[11]}-01T00:00:00" "$year2-${stopTime[11]}-01T00:00:00" 
		elif [[ $body_index -eq 3 ]]; then
			./ephemeris $body_index "SUN" "earth" "${years[j]}-${startTime[11]}-01T00:00:00" "$year2-${stopTime[11]}-01T00:00:00" 
		elif [[ $body_index -eq 4 ]]; then
			./ephemeris $body_index "SUN" "mars" "${years[j]}-${startTime[11]}-01T00:00:00" "$year2-${stopTime[11]}-01T00:00:00" 
		elif [[ $body_index -eq 5 ]]; then
			./ephemeris $body_index "SUN" "jupiter" "${years[j]}-${startTime[11]}-01T00:00:00" "$year2-${stopTime[11]}-01T00:00:00" 
		elif [[ $body_index -eq 6 ]]; then
			./ephemeris $body_index "SUN" "saturn" "${years[j]}-${startTime[11]}-01T00:00:00" "$year2-${stopTime[11]}-01T00:00:00" 
		elif [[ $body_index -eq 7 ]]; then
			./ephemeris $body_index "SUN" "uranus" "${years[j]}-${startTime[11]}-01T00:00:00" "$year2-${stopTime[11]}-01T00:00:00" 
		elif [[ $body_index -eq 8 ]]; then
			./ephemeris $body_index "SUN" "neptune" "${years[j]}-${startTime[11]}-01T00:00:00" "$year2-${stopTime[11]}-01T00:00:00" 
		elif [[ $body_index -eq 9 ]]; then
			./ephemeris $body_index "SUN" "pluto" "${years[j]}-${startTime[11]}-01T00:00:00" "$year2-${stopTime[11]}-01T00:00:00" 
		fi
	done
done

#Clean
for body in mercury venus earth mars jupiter saturn uranus neptune pluto
do
	cp $body*.nc nc/$body/
	rm $body*.nc
done