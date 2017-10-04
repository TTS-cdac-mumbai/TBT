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
    tcsh front-end-dsp/Segmentation/scripts/group_delay_segmentation.sh wav_16KHz/$wavfn.wav 6 200 100 0.5 0.2
    wait 
    tcsh front-end-dsp/Segmentation/scripts/group_delay_segmentation_flux.sh wav_16KHz/$wavfn.wav 2 100 100 0.3
    wait
    cat results/boundaryEgy | cut -d " " -f5- > boundaryEgy 
    cat results/boundaryFlux | cut -d " " -f5- > boundaryFlux 
    echo "Boundary correction"
    cat boundaryEgy
    cat boundaryFlux
    ./syllable_boundary_correction $2/$fn > $3/$fn
    wait
    echo "Boundary correction completed for" $fn
    cd results/
    mv ste.txt STE/$wavfn-ste.txt
    mv flux.txt SF/$wavfn-flux.txt
    mv boundaryEgy.lab STE/$wavfn.lab
    mv boundaryFlux.lab SF/$wavfn.lab
    mv gd_spectrumEgy.txt STE/$wavfn-gdEgy.txt
    mv magnitudeSpectrumFlux.txt SF/$wavfn-magFlux.txt
    cd ../
    mv boundaryEgy results/STE/$wavfn-boundaryEgy
    mv boundaryFlux results/SF/$wavfn-boundaryFlux
    @ count++
end
