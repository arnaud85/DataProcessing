set terminal png 
set output "maven_orb.png"

set multiplot

set origin 0, 0
set size 1.0, 0.5
set title "MAVEN X, Y, Z MSO"
set xlabel "Time"
set xtics ("00:00" 1423699267.19, "06:00" 1423720867.19, "12:00" 1423742467.19, "18:00" 1423764067.19, "24:00" 1423785667.19)
set ylabel "X_mso, Y_mso, Z_mso"
set yrange [-3:3]
set zeroaxis 
plot "maven_orb.txt" using 1:2 lt 1 smooth csplines notitle, "maven_orb.txt" using 1:3 lt 2 smooth csplines notitle, "maven_orb.txt" using 1:4 lt 3 smooth csplines notitle
set key off

set origin 0, 0.5
set size 1.0, 0.5
set title "MAVEN distance to Mars"
set xlabel "Time"
set xtics ("00:00" 1423699267.19, "06:00" 1423720867.19, "12:00" 1423742467.19, "18:00" 1423764067.19, "24:00" 1423785667.19)
set ylabel "Distance (R_mars)"
set yrange [1:3]
set zeroaxis 
set key on inside right top
plot "maven_orb.txt" using 1:5 lt 0 smooth csplines notitle
set key off

unset multiplot