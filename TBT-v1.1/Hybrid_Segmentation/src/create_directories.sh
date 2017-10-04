#!/bin/tcsh
# ----------------------------------------------------------------- #
# 							            #
#                   Author - Aswin Shanmugam S		            #
#        DON Lab, Indian Institute of Technology - Madras           #
# 							            #
# ----------------------------------------------------------------- #

mkdir -p results
mkdir -p results/SF results/STE
mkdir -p hmm_syllable_er_syl_lab_with_begin_end_phone hmm_syllable_lab_with_begin_end_phone intermediate_output_lab output_lab_hmm_festival_format output_lab_phone output_lab_phone_htk_format output_lab_syllable output_lab_syllable_with_begin_end_phone
cd hmm/
mkdir -p wav_8KHz models_hmm models_hybrid hybrid_phone_lab output_lab_with_SIL output_lab_with_SIL_festival_format output_lab_temp output_lab output_lab_festival_format output_lab_hmm output_lab_hybrid output_lab_splice output_lab_splice_festival_format syllab syllab_2 syllab_with_sil syllab_with_sil_2 fal_hybrid_sentence fal_hybrid_sentence_festival_format keylab wav_8KHz splice_wav hmm_GMV
mkdir models_hmm/reestimatedhmms models_hybrid/reestimatedhmms
tcsh scripts/create_HERest_directories.sh 0 8
cd ../ 
