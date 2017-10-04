#!/bin/tcsh -f
# ----------------------------------------------------------------- #
# 							            #
#                   Author - Aswin Shanmugam S		            #
#        DON Lab, Indian Institute of Technology - Madras           #
# 							            #
# ----------------------------------------------------------------- #

if( $# != 3 ) then
  echo " Argument ---> list input_folder output_folder"
endif

set num = `cat $1 | wc -l`
set count = 1
while ( $count <= $num )
    set fn = `head -$count $1 | tail -1`
    set wavfn = `echo $fn | cut -d "." -f1`
    echo "Syllable boundary correction for" $fn
    cp results/STE/$wavfn-boundaryEgy boundaryEgy
    cp results/SF/$wavfn-boundaryFlux boundaryFlux
    ./syllable_boundary_correction $2/$fn > $3/$fn
    wait
    echo "Boundary correction completed for" $fn
    @ count++
end
