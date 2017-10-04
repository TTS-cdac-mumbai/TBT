# ----------------------------------------------------------------- #
# 							            #
#                   Author - Aswin Shanmugam S		            #
#        DON Lab, Indian Institute of Technology - Madras           #
# 							            #
# ----------------------------------------------------------------- #

sh src/clean.sh
make -B
sh src/create_directories.sh

cd hmm/
cat syldict | cut -d " " -f2- > temp
cp syldict syldict_without_context
sed -e 's/beg\-//g' -i syldict_without_context
sed -e 's/\_end//g' -i syldict_without_context
sed -e 's/\s\+/\n/g' -i temp
sort -u temp > phonelist
cat syldict_without_context | cut -d " " -f2- > temp
sed -e 's/\s\+/\n/g' -i temp
sort -u temp > phonelist_without_context
cd ../
cp hmm/syldict_without_context dict_with_end
sed -e 's/$/ end/' -i dict_with_end
cp Phonelist_Description/Vowels hmm/vowels

cd hmm/
echo "Downsampling Wavefiles to 8KHz for Extracting MFCC's"
ls ../wav_16KHz > wav_list
tcsh scripts/sr_change.sh wav_list
ls wav_8KHz/*.wav > wav_list
ls prompt-lab > list
cp list ../

tcsh scripts/dict.sh phonelist_without_context
tcsh scripts/dict_with_context.sh phonelist
tcsh scripts/map_table.sh wav_list
echo "Extracting MFCC's from 8KHz wavefiles"
sh scripts/extract_feature.sh

sort phonelist_without_context > phonelist_sorted
sort vowels > vowels_sorted
comm -23 phonelist_sorted vowels_sorted > consonants

cd prompt-lab/
sed -i 's/_v//' *
sed -i 's/_iv//' *
sed -i 's/_uv//' *
sed -i 's/_GEM//' *
sed -i 's/_BEG//' *
sed -i 's/_MID//' *
sed -i 's/_END//' *
sed -i 's/_beg//' *
sed -i 's/_mid//' *
sed -i 's/_end//' *
sed -i 's/_bg//' *
sed -i 's/_eg//' *
sed -i 's/-x//' *
cd ../

perl scripts/create_transcription.pl
./createMLF

HLEd -l '*' -d syldict_without_context -i phones.mlf mkphones0.led words.mlf
HLEd -l '*' -d syldict -i phones_with_context.mlf mkphones0.led words.mlf
cat map_table | cut -d " " -f2 > flist
HCompV -C config_files/config_feature -f 0.01 -m -S flist -M hmm_GMV protos/proto_3s_2m
HCompV -C config_files/config_feature -f 0.01 -m -S flist -M hmm_GMV protos/proto_5s_2m
cat HMM_macro_header hmm_GMV/vFloors > hmm_GMV/macros
tcsh scripts/hdef.sh vowels consonants 
tcsh scripts/FS_HERest.sh 0 1 14

echo "Sentence Lvl Alignment with SIL"
HVite -l output_lab_with_SIL/ -C config_files/config -a -H hmm_GMV/hmm14/macros -H hmm_GMV/hmm14/hmmdefs -y lab -o SM -I words.mlf -S flist syldict_without_context phonelist_without_context
perl scripts/normallab_with_SIL.pl
tcsh scripts/remsil_initial.sh list
rm Transcription.txt
perl scripts/create_transcription_SIL_removed.pl
mv words.mlf words_with_SIL.mlf
./createMLF

echo "Sentence Lvl Alignment"
HVite -l output_lab/ -C config_files/config -a -H hmm_GMV/hmm14/macros -H hmm_GMV/hmm14/hmmdefs -y lab -o SM -I words.mlf -S flist syldict_without_context phonelist_without_context
perl scripts/normallab.pl

cd ../
echo "Adding Begin & End Phone to Syllable Lab Files.... I"
tcsh src/add_phone2syl.sh list
rm -f intermediate_output_lab/*
echo "Syllable Boundary Correction.... I"
tcsh src/gd_correction.sh list hmm_syllable_lab_with_begin_end_phone intermediate_output_lab
cp intermediate_output_lab/* hmm/syllab_with_sil/
cd hmm/
echo "Remove very small silences.... I"
tcsh scripts/remsil.sh list

echo "Splicing Waveforms & Extracting Features.... I"
perl scripts/splice.pl
./createMLF_splice
ls splice_wav/ > wav_list_splice
sed -i 's/^/splice_wav\//' wav_list_splice 
rm map_table
tcsh scripts/map_table.sh wav_list_splice
cat map_table | cut -d " " -f2 > flist_splice
sh scripts/extract_feature_splice.sh

HLEd -l '*' -d syldict -i phones_splice.mlf mkphones0.led words_splice.mlf
cp hmm_GMV/hmmdefs hmm0/hmmdefs
cp hmm_GMV/macros hmm0/macros

echo "Embedded Reestimation.... I"
HERest -C config_files/config_feature_2 -I phones_splice.mlf -t 250.0 150.0 6000.0 -S flist_splice -H hmm0/macros -H hmm0/hmmdefs -M hmm1 phonelist
HERest -C config_files/config_feature_2 -I phones_splice.mlf -t 250.0 150.0 6000.0 -S flist_splice -H hmm1/macros -H hmm1/hmmdefs -M hmm2 phonelist
HERest -C config_files/config_feature_2 -I phones_splice.mlf -t 250.0 150.0 6000.0 -S flist_splice -H hmm2/macros -H hmm2/hmmdefs -M hmm3 phonelist
HERest -C config_files/config_feature_2 -I phones_splice.mlf -t 250.0 150.0 6000.0 -S flist_splice -H hmm3/macros -H hmm3/hmmdefs -M hmm4 phonelist
HERest -C config_files/config_feature_2 -I phones_splice.mlf -t 250.0 150.0 6000.0 -S flist_splice -H hmm4/macros -H hmm4/hmmdefs -M hmm5 phonelist
HERest -C config_files/config_feature_2 -I phones_splice.mlf -t 250.0 150.0 6000.0 -S flist_splice -H hmm5/macros -H hmm5/hmmdefs -M hmm6 phonelist
HERest -C config_files/config_feature_2 -I phones_splice.mlf -t 250.0 150.0 6000.0 -S flist_splice -H hmm6/macros -H hmm6/hmmdefs -M hmm7 phonelist
HVite -l output_lab_splice/ -C config_files/config_splice -a -H hmm7/macros -H hmm7/hmmdefs -y lab -o SM -I words.mlf -S flist syldict phonelist 
perl scripts/normallab_splice.pl

cd ../
echo "Adding Begin & End Phone to Syllable Lab Files.... II"
tcsh src/add_phone2syl_2.sh list
rm -f output_lab_syllable/*
echo "Syllable Boundary Correction.... II"
tcsh src/gd_correction_2.sh list hmm_syllable_er_syl_lab_with_begin_end_phone output_lab_syllable
cp output_lab_syllable/* hmm/syllab_with_sil_2/
cd hmm/
echo "Remove very small silences.... II"
tcsh scripts/remsil_2.sh list

echo "Splicing Waveforms & Extracting Features.... II"
rm -rf splice_wav/
mkdir splice_wav
perl scripts/splice_2.pl
./createMLF_splice_2
ls splice_wav/ > wav_list_splice
sed -i 's/^/splice_wav\//' wav_list_splice 
rm map_table
tcsh scripts/map_table.sh wav_list_splice
cat map_table | cut -d " " -f2 > flist_splice
sh scripts/extract_feature_splice.sh

HLEd -l '*' -d syldict -i phones_splice.mlf mkphones0.led words_splice.mlf
cp hmm7/hmmdefs hmm0/hmmdefs
cp hmm7/macros hmm0/macros

echo "Embedded Reestimation.... II"
HERest -C config_files/config_feature_2 -I phones_splice.mlf -t 250.0 150.0 6000.0 -S flist_splice -H hmm0/macros -H hmm0/hmmdefs -M hmm1 phonelist
HERest -C config_files/config_feature_2 -I phones_splice.mlf -t 250.0 150.0 6000.0 -S flist_splice -H hmm1/macros -H hmm1/hmmdefs -M hmm2 phonelist
HERest -C config_files/config_feature_2 -I phones_splice.mlf -t 250.0 150.0 6000.0 -S flist_splice -H hmm2/macros -H hmm2/hmmdefs -M hmm3 phonelist
HERest -C config_files/config_feature_2 -I phones_splice.mlf -t 250.0 150.0 6000.0 -S flist_splice -H hmm3/macros -H hmm3/hmmdefs -M hmm4 phonelist
HERest -C config_files/config_feature_2 -I phones_splice.mlf -t 250.0 150.0 6000.0 -S flist_splice -H hmm4/macros -H hmm4/hmmdefs -M hmm5 phonelist
HERest -C config_files/config_feature_2 -I phones_splice.mlf -t 250.0 150.0 6000.0 -S flist_splice -H hmm5/macros -H hmm5/hmmdefs -M hmm6 phonelist
HERest -C config_files/config_feature_2 -I phones_splice.mlf -t 250.0 150.0 6000.0 -S flist_splice -H hmm6/macros -H hmm6/hmmdefs -M hmm7 phonelist > log_remove
cat log_remove | cut -d " " -f7 | cut -d "/" -f2 | cut -d "-" -f1 | sort | uniq > list_remove
HVite -l '*' -C config_files/config_splice -H hmm7/macros -H hmm7/hmmdefs -i align.mlf -m -t 250.0 150.0 6000.0 -y lab -I words_splice.mlf -S flist_splice syldict phonelist
perl scripts/combine_syllable_level_alignment.pl
cat list_remove_duplicates | uniq > list_remove
tcsh scripts/lab_remove.sh list_remove

HVite -l fal_hybrid_sentence/ -C config_files/config -a -H hmm7/macros -H hmm7/hmmdefs -y lab -I phones_with_context.mlf -S flist dict_with_context phonelist
perl scripts/normallab_2.pl
tcsh scripts/lab_copy.sh list_remove
cp hybrid_phone_lab/* ../output_lab_phone/
sed -e 's/beg\-//g' -i ../output_lab_phone/*
sed -e 's/\_end//g' -i ../output_lab_phone/*

HVite -l output_lab_hybrid/ -C config_files/config -a -H hmm7/macros -H hmm7/hmmdefs -y lab -o MN -I phones_with_context.mlf -S flist dict_with_context phonelist
HVite -l output_lab_hmm/ -C config_files/config -a -H hmm_GMV/hmm14/macros -H hmm_GMV/hmm14/hmmdefs -y lab -o MN -I phones.mlf -S flist dict phonelist_without_context
sed -e 's/beg\-//g' -i output_lab_hybrid/*
sed -e 's/\_end//g' -i output_lab_hybrid/*

perl scripts/cal_likelihood_category.pl
perl scripts/cal_likelihood_category_hybrid.pl
cd ../
perl hmm/scripts/normallab_hmm.pl

echo "Completed"
