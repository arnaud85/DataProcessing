set terminal png 
set output "maven_orb_arnaud_manu_diff.png"

set origin 0, 0
set size 1.0, 1.0
set title "MAVEN RELATIVE DIFF : Z"
set xlabel "Time"
set xtics ("00:00" 1417392000, "24:00" 1417478280)
# set ylabel "X, Y, Z MSO"
set ylabel "Z MSO relative diff"
set yrange [-0.00003:0.00003]
set zeroaxis 
# plot "maven_both.diff" using 1:3 lt 1 smooth csplines notitle, "maven_both.diff" using 1:4 lt 2 smooth csplines notitle, "maven_both.diff" using 1:5 lt 13 smooth csplines notitle
# plot "maven_both.diff" using 1:2 lt 1 smooth csplines notitle
# plot "maven_both.diff" using 1:3 lt 1 smooth csplines notitle
# plot "maven_both.diff" using 1:4 lt 1 smooth csplines notitle
plot "maven_both.diff" using 1:5 lt 1 smooth csplines notitle
set key off