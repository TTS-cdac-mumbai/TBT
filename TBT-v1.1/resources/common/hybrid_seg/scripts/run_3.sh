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

echo "Pass III completed------->>>"
