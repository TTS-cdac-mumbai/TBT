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

echo "Pass IV completed------->>>"
