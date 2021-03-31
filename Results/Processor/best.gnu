set obj 18 rect from 420,350 to 800,670 fs solid fc rgb "#1718"
set obj 17 rect from 660,1020 to 980,1400 fs solid fc rgb "#4228"
set obj 16 rect from 800,330 to 1000,560 fs solid fc rgb "#307e"
set obj 15 rect from 860,0 to 970,330 fs solid fc rgb "#3a16"
set obj 14 rect from 330,680 to 710,1010 fs solid fc rgb "#7174"
set obj 13 rect from 0,350 to 420,680 fs solid fc rgb "#191e"
set obj 12 rect from 330,1010 to 660,1430 fs solid fc rgb "#d41"
set obj 11 rect from 0,680 to 330,1100 fs solid fc rgb "#361f"
set obj 4 rect from 980,1020 to 1310,1350 fs solid fc rgb "#2123"
set obj 3 rect from 540,0 to 750,340 fs solid fc rgb "#136e"
set obj 2 rect from 750,0 to 860,210 fs solid fc rgb "#3579"
set obj 1 rect from 800,560 to 920,670 fs solid fc rgb "#7545"
set obj 5 rect from 0,1100 to 330,1430 fs solid fc rgb "#4906"
set obj 6 rect from 1000,690 to 1330,1020 fs solid fc rgb "#4508"
set obj 7 rect from 1000,540 to 1230,690 fs solid fc rgb "#2222"
set obj 8 rect from 0,0 to 540,350 fs solid fc rgb "#2528"
set obj 9 rect from 1000,0 to 1350,540 fs solid fc rgb "#570b"
set obj 10 rect from 710,680 to 950,1010 fs solid fc rgb "#3731"
set ytics 138
set grid ytics lc rgb "#bbbbbb" lw 1 lt 0
set xtics 138 rotate
set grid xtics lc rgb "#bbbbbb" lw 1 lt 0
set terminal png size 400,400
set output '../outputs/best.gnu.png'
plot [0:2070][0:2070] 0 notitle
show grid
