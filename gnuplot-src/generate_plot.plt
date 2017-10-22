set macros
load 'dark2.pal'
# set title "pathological_oski"
#set term svg font "Computer Modern, 15pt"
#set term epslatex color lw 3 newstyle
set term epslatex size 16cm,4cm color lw 2 newstyle
#widthvar=system("echo $widthvar")
#heightvar=system("echo $heightvar")
#set term epslatex size widthvar,heightvar color lw 3 newstyle
# set size square
set output 'figures/scanserialvar.tex'
set datafile separator " "
#set grid
# set xrange[40.0:100.0]
# set yrange[2.1:]
set xtics rotate by 45 offset -0.8,-0.75
#set lmargin at screen 0.2;
#set rmargin at screen 0.95;
#set bmargin at screen 0.15;
#set tmargin at screen 0.95;
# set key left top width -5
# set ylabel offset 2
set key samplen 2.5  width -12 spacing .75 # font ",10"
set key right top
# set size .75,1
# set key at 1,0
# set key at graph 1, 0.9
# set format y "%.2e"
#set style line 2 lt 3
set multiplot layout 1,4 spacing 0.08,0.08 columnsfirst # title "Error vs Norm. Time to Estimate"
set lmargin 2
set rmargin 2
set bmargin 1

#### ERROR BARS
unset xlabel
set title "ct20stif"
#ct20stif
# unset title
set ylabel "Mean Maximum \\ Relative Error" offset 2
#set xlabel "Time to estimate (normalized against ref)" 
set xrange[0:300]
set yrange[0:0.5]

plot "../data/lanka12/roi/error_asx_ct20stif" using 1:2 title '\small PHIL' with lines linestyle 2 lw 2, "../data/lanka12/roi/error_asx_ct20stif" using 1:2:3 notitle with errorbars linestyle 2 lw 1, "../data/lanka12/roi/error_oski_ct20stif" using 1:2 title '\small OSKI' with lines linestyle 3 dt '..' lw 2, "../data/lanka12/roi/error_oski_ct20stif" using 1:2:3 notitle with errorbars linestyle 3 lw 1,


#gupta1
unset key
unset ylabel
set title "gupta1"
set xrange[0:300]
set yrange[0:0.5]

plot "../data/lanka12/roi/error_asx_gupta1_conv" using 1:2 with lines linestyle 2 lw 2, "../data/lanka12/roi/error_asx_gupta1_conv" using 1:2:3 notitle with errorbars linestyle 2 lw 1, "../data/lanka12/roi/error_oski_gupta1_conv" using 1:2 with lines linestyle 3 dt '..' lw 2, "../data/lanka12/roi/error_oski_gupta1_conv" using 1:2:3 notitle with errorbars linestyle 3 lw 1,

#pathological_oski
set title "pathological_oski"
set xrange[0:1000]
set yrange[0:3.7]

plot "../data/lanka12/roi/error_asx_pathological_oski" using 1:2 title '\small ASX' with lines linestyle 2 lw 2, "../data/lanka12/roi/error_asx_pathological_oski" using 1:2:3 notitle with errorbars linestyle 2 lw 1, "../data/lanka12/roi/error_oski_pathological_oski" using 1:2 with lines linestyle 3 dt '..' lw 2, "../data/lanka12/roi/error_oski_pathological_oski" using 1:2:3 notitle with errorbars linestyle 3 lw 1,

#pathological_asx
# xlabel at the bottom
set title "pathological_asx"
set xlabel "Time to Estimate (in SpMV time)" offset screen -0.37,0
set xrange[0:3000]
set yrange[0:0.18]

plot "../data/lanka12/roi/error_asx_pathological_asx" using 1:2 title '\small ASX' with lines linestyle 2 lw 2, "../data/lanka12/roi/error_asx_pathological_asx" using 1:2:3 notitle with errorbars linestyle 2 lw 1, "../data/lanka12/roi/error_oski_pathological_asx" using 1:2 title '\small OSKI' with lines linestyle 3 dt '..' lw 2, "../data/lanka12/roi/error_oski_pathological_asx" using 1:2:3 notitle with errorbars linestyle 3 lw 1,
