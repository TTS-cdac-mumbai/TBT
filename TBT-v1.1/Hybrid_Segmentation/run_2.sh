# !bin/bash

cd hmm/
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

echo "Pass II completed------->>>"
