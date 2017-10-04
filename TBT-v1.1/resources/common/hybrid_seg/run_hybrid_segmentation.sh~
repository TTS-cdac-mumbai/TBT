#! bin/bash

###############################################
#
#Designed and Developed by Atish and Rachana
#Script for hybrid segmentation to generate 
#aligned phone level and syllable level lab files. 
#
###############################################
LNG=$1

rm -rf wav_16KHz
rm -rf Phonelist_Description
rm -rf hmm/prompt-lab

cp -r ../resources/languages/$LNG/hybrid_seg/Phonelist_Description Phonelist_Description/
mv ../temp/hybrid_seg/wav_16KHz wav_16KHz/
cp -r ../temp/hybrid_seg/prompt-lab hmm/
cp -r ../temp/hybrid_seg/syldict hmm/

cp -r ../resources/common/hybrid_seg/scripts/run_1.sh ../resources/common/hybrid_seg/scripts/run_2.sh ../resources/common/hybrid_seg/scripts/run_3.sh ../resources/common/hybrid_seg/scripts/run_4.sh ./

sh run_1.sh
sh run_2.sh
sh run_3.sh
sh run_4.sh
