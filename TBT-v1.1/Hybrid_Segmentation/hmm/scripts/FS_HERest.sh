#!/bin/tcsh

set count = $1
set countplusone = $2
set NoI = $3
mkdir -p hmm_GMV/hmm0
cp hmm_GMV/macros hmm_GMV/hmm0/
cp hmm_GMV/hmmdefs hmm_GMV/hmm0/
while ($count < $NoI)
mkdir -p hmm_GMV/hmm$countplusone
echo "Embedded Reestimation ---- Iteration $countplusone"
HERest -C config_files/config_feature -I phones.mlf -t 250.0 150.0 6000.0 -S flist -H hmm_GMV/hmm$count/macros -H hmm_GMV/hmm$count/hmmdefs -M hmm_GMV/hmm$countplusone phonelist_without_context
@ count++
@ countplusone++
end
