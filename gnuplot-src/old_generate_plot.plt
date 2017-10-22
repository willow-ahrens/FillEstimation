set macros
load 'dark2.pal'
# set title "pathological_oski"
#set term svg font "Computer Modern, 15pt"
#set term epslatex color lw 3 newstyle
set term epslatex size 15cm,22cm color lw 2 newstyle
#widthvar=system("echo $widthvar")
#heightvar=system("echo $heightvar")
#set term epslatex size widthvar,heightvar color lw 3 newstyle
# set size square
set output 'figures/scanserialvar.tex'
set datafile separator " "
#set grid
# set xrange[40.0:100.0]
# set yrange[2.1:]
#set xtics 0.1
#set lmargin at screen 0.2;
#set rmargin at screen 0.95;
#set bmargin at screen 0.15;
#set tmargin at screen 0.95;
# set key left top width -5
set ylabel offset 2
set key samplen 2.5  width -12 spacing .75 # font ",10"
set key right top
# set size .75,1
# set key at 1,0
# set key at graph 1, 0.9
# set format y "%.2e"
#set style line 2 lt 3
set multiplot layout 5,2 columnsfirst
# set title ""

#### ERROR BARS
unset xlabel
set title "Error vs Time to Estimate"
set ylabel "3dtube\n\nMean Error"
set xrange[0:50]
set yrange[0:0.35]

plot "../plot-inputs/error_asx_3dtube_conv" using 1:2 title '\small ASX' with lines linestyle 2 lw 2, "../plot-inputs/error_asx_3dtube_conv" using 1:2:3 notitle with errorbars linestyle 2 lw 1, "../plot-inputs/error_oski_3dtube_conv" using 1:2 title '\small OSKI' with lines linestyle 3 lw 2, "../plot-inputs/error_oski_3dtube_conv" using 1:2:3 notitle with errorbars linestyle 3 lw 1,

unset key

#ct20stif
unset title
set ylabel "ct20stif\n\nMean Error"
#set xlabel "Time to estimate (normalized against ref)" 
set xrange[0:60]
set yrange[0:0.5]

plot "../plot-inputs/error_asx_ct20stif" using 1:2 title '\small ASX' with lines linestyle 2 lw 2, "../plot-inputs/error_asx_ct20stif" using 1:2:3 notitle with errorbars linestyle 2 lw 1, "../plot-inputs/error_oski_ct20stif" using 1:2 title '\small OSKI' with lines linestyle 3 lw 2, "../plot-inputs/error_oski_ct20stif" using 1:2:3 notitle with errorbars linestyle 3 lw 1,

#gupta1
set ylabel "gupta1\n\nMean Error"
# set xlabel "Time to estimate (normalized against ref)" 
set xrange[0:60]
set yrange[0:0.5]

plot "../plot-inputs/error_asx_gupta1_conv" using 1:2 title '\small ASX' with lines linestyle 2 lw 2, "../plot-inputs/error_asx_gupta1_conv" using 1:2:3 notitle with errorbars linestyle 2 lw 1, "../plot-inputs/error_oski_gupta1_conv" using 1:2 title '\small OSKI' with lines linestyle 3 lw 2, "../plot-inputs/error_oski_gupta1_conv" using 1:2:3 notitle with errorbars linestyle 3 lw 1,

#pathological_oski
set ylabel "pathological_oski\n\nMean Error"
set xrange[0:2000]
set yrange[0:3.7]

plot "../plot-inputs/error_asx_pathological_oski" using 1:2 title '\small ASX' with lines linestyle 2 lw 2, "../plot-inputs/error_asx_pathological_oski" using 1:2:3 notitle with errorbars linestyle 2 lw 1, "../plot-inputs/error_oski_pathological_oski" using 1:2 title '\small OSKI' with lines linestyle 3 lw 2, "../plot-inputs/error_oski_pathological_oski" using 1:2:3 notitle with errorbars linestyle 3 lw 1,

#pathological_asx
set ylabel "pathological_asx\n\nMean Error"
# xlabel at the bottom
set xlabel "Time to Estimate (in SpMV time)" 
set xrange[0:2000]
set yrange[0:0.21]

plot "../plot-inputs/error_asx_pathological_asx" using 1:2 title '\small ASX' with lines linestyle 2 lw 2, "../plot-inputs/error_asx_pathological_asx" using 1:2:3 notitle with errorbars linestyle 2 lw 1, "../plot-inputs/error_oski_pathological_asx" using 1:2 title '\small OSKI' with lines linestyle 3 lw 2, "../plot-inputs/error_oski_pathological_asx" using 1:2:3 notitle with errorbars linestyle 3 lw 1,

##### SpMV
unset yrange
unset xlabel

# 3d tube 
# set ytics 0.00003
#set yrange[0.00530:0.00548]
set xtics rotate by 45 offset -0.8,-0.8
set xrange[0:50]
set title "SpMV time vs Time to Estimate"
set ylabel "Normalized SpMV time"
plot "../plot-inputs/asx_3dtube_conv" using 1:2 title '\small ASX' with linespoints linestyle 2 lw 3 pi -4 ,\
     "../plot-inputs/oski_3dtube_conv" using 1:2 title '\small OSKI' with lines linestyle 3 lw 3,

# set size 1,1
unset title
set xtics rotate by 45 offset -0.8,-0.8
set xrange[.3:15]
set ylabel "Normalized SpMV time"

# set title "ct20stif"
plot "../plot-inputs/asx_ct20stif" using 1:2 title '\small ASX' with linespoints linestyle 2 lw 3 pi -4 ,\
     "../plot-inputs/oski_ct20stif" using 1:2 title '\small OSKI' with lines linestyle 3 lw 3,


set xtics rotate by 45 offset -0.8,-0.8
set xrange[0:20]
set ylabel "Normalized SpMV time"
plot "../plot-inputs/asx_gupta1_conv" using 1:2 title '\small ASX' with linespoints linestyle 2 lw 3 pi -4 ,\
     "../plot-inputs/oski_gupta1_conv" using 1:2 title '\small OSKI' with lines linestyle 3 lw 3,

set ylabel "Normalized SpMV time"
set xrange[0:1500]
set xtics rotate by 45 offset -0.8,-0.8
plot "../plot-inputs/asx_pathological_oski" using 1:2 title '\small ASX' with linespoints linestyle 2 lw 3 pi -4 ,\
     "../plot-inputs/oski_pathological_oski" using 1:2 title '\small OSKI' with lines linestyle 3 lw 3,

set ylabel "Normalized SpMV time"
set xlabel "Time to Estimate (in SpMV time)" 
set xrange[0:2000]
set yrange[0.98:1]
plot "../plot-inputs/asx_pathological_asx" using 1:2 title '\small ASX' with linespoints linestyle 2 lw 3 pi -4 ,\
     "../plot-inputs/oski_pathological_asx" using 1:2 title '\small OSKI' with lines linestyle 3 lw 3,

