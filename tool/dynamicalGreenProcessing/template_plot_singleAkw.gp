#!/usr/bin/gnuplot 

w = 8.5
h = 12
offset1 = 0.0
kRange = 32
w_min = -8.0
w_max =  8.0

Nw_data = 1500
w_min_data = -13.0
w_max_data =  13.0
w_range_data = w_max_data - w_min_data

top = 0.95
left = 0.15
right = 0.85
bottom = 0.10

set terminal pdf enhanced color size w cm,h cm
set output 'akw_single.pdf'
#set terminal epslatex size w cm,h cm color
#set output 'plot.tex'

set pm3d map interpolate 0,0;
set macros
#set multiplot
set xrange [-0.5:kRange+0.5]
set yrange [w_min:w_max]
set cbrange [0.00:0.6]

set palette defined (0 'white',0.016 'white',0.05 '#ffea4a',0.09 '#fdb416',0.16 '#fa3100',0.27 '#c31b00',0.6 'black')
#set colorbox vertical user origin right+0.02, top-0.4 size .04,.4

set colorbox vertical user origin right+0.02, (top+bottom)/2 size .02,.4
set ytics ('-16' offset1-16,'-12' offset1-12,'-8' offset1-8,'-4' offset1-4, '0' offset1, '4' offset1+4.0, '8' offset1+8.0, '12' offset1+12.0,'-16' offset1+16)
set tics front
set arrow from -0.5,offset1 to kRange+0.5,offset1 nohead lw 2 dt 2 lc 'black' front
#set xtics ('$0$' 0, '$\pi$' 32 )
set xtics ('0' 0, '{/Symbol p}' kRange)

set xlabel 'k' 
set ylabel '{/Symbol w}' 
set label 1 'A(k,{/Symbol w})' at screen 0.5*(left+right), screen top+0.03 front

set lmargin at screen left; set rmargin at screen right
set tmargin at screen top; set bmargin at screen bottom
plot 'output/Akw.dat' u 1:($2*w_range_data/Nw_data+w_min_data):3 matrix notitle w image 

