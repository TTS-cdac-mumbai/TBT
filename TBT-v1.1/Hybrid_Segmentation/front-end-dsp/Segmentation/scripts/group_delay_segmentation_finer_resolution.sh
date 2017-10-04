#!/bin/tcsh -f

setenv WSF $2

	ch_wave $1 -o results/wav -otype nist

	echo "thresSilence = $3 and thersVoiced = $4"
	echo "wavefile = $1"

front-end-dsp/Segmentation/bin/WordsWithSilenceRemoval front-end-dsp/Segmentation/src/fe-words_finer_resolution.base $1 results/sigEgy results/specEgy results/boundaryEgy $3  $4

        cat results/specEgy | awk '{print $7}' > results/gd_spectrumEgy
        cat results/boundaryEgy | awk '{print $5 * 16000}' > results/segmentsEgy
        cat results/boundaryEgy | awk '{print $6}' > results/segmentsEgy_indicator
        cat results/boundaryEgy | cut -d " " -f5- > results/boundaryEgy.lab
        sed -i 's/\ /\ 125\ /' results/boundaryEgy.lab
        sed -i '1,1s/^/#\n/' results/boundaryEgy.lab
        cp results/gd_spectrumEgy results/gd_spectrumEgy.txt
        cat results/specEgy | cut -d " " -f3 > results/ste.txt
