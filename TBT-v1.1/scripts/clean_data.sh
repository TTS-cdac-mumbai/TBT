#! bin/bash

###############################################

#Designed and Developed by Atish and Rachana
#Script to clean data

###############################################

LNG=$1

rm -rf ../source_hybrid_seg/$LNG/for_f0_values/f0_files
rm -rf ../source_hybrid_seg/$LNG/for_f0_values/f0_txt_files
rm -rf ../source_hybrid_seg/$LNG/for_f0_values/global_minmax
rm -rf ../source_hybrid_seg/$LNG/for_f0_values/list.txt
rm -rf ../source_hybrid_seg/$LNG/for_f0_values/list_f0_txtfiles
rm -rf ../source_hybrid_seg/$LNG/for_f0_values/list_of_f0
rm -rf ../source_hybrid_seg/$LNG/for_f0_values/local_minmax

rm -rf ../source_hybrid_seg/$LNG/prompt-lab
rm -rf ../source_hybrid_seg/$LNG/list
rm -rf ../source_hybrid_seg/$LNG/syldict

rm -rf ../source_htsvoice/$LNG/raw_files/raw
rm -rf ../source_htsvoice/$LNG/raw_files/wav_48KHz
rm -rf ../source_htsvoice/$LNG/utts	
rm -rf ../source_htsvoice/$LNG/raw_files/wav_list
