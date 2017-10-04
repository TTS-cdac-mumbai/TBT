# ----------------------------------------------------------------- #
# 							            #
#                   Author - Aswin Shanmugam S		            #
#        DON Lab, Indian Institute of Technology - Madras           #
# 							            #
# ----------------------------------------------------------------- #

rm -f list boundary* dict_with_end
rm -f syllable_boundary_correction
rm -rf results
rm -rf hmm_syllable_er_syl_lab_with_begin_end_phone hmm_syllable_lab_with_begin_end_phone intermediate_output_lab output_lab*
rm -f *_list *_sorted consonants
cd hmm/
rm -f phonelist phonelist_without_context *_models scores syldict_without_context vowels createMLF*
rm -rf models_hmm models_hybrid
rm -rf hmm?
rm -rf hmm_GMV
rm -rf hybrid_phone_lab output* syllab* fal_hybrid_sentence* keylab wav_8KHz splice_wav
rm -f list flist* map_table *.mlf Transcription* allhmms *sorted wav_list* consonants dict* list_remove* temp
