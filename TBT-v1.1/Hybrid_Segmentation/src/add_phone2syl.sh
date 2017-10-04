#!/bin/tcsh -f
# ----------------------------------------------------------------- #
# 							            #
#                   Author - Aswin Shanmugam S		            #
#        DON Lab, Indian Institute of Technology - Madras           #
# 							            #
# ----------------------------------------------------------------- #

if( $# != 1 ) then
  echo " Argument ---> list "
endif

rm -f hmm_syllable_lab_with_begin_end_phone/*

set num = `cat $1 | wc -l`
set count = 1
while ( $count <= $num )
    set fn = `head -$count $1 | tail -1`
    set num2 = `cat hmm/output_lab_festival_format/$fn | wc -l`
    set count2 = 2
    echo "#" >> hmm_syllable_lab_with_begin_end_phone/$fn
    while ( $count2 <= $num2 )  
        set ln1 = `head -$count2 hmm/output_lab_festival_format/$fn | tail -1`
        set syl = `echo $ln1 | cut -d " " -f3`
        set transcription = `cat dict_with_end | awk "/^$syl /"`
        set phoneme1 = `echo $transcription | cut -d " " -f2`
        set phoneme2 = `echo $transcription | cut -d " " -f2` 
        set count3 = 3
        while ( $phoneme2 != "end" )
              set phoneme2 = `echo $transcription | cut -d " " -f$count3`
              @ count3++
        end
        @ count3--
        @ count3--
        set phoneme2 = `echo $transcription | cut -d " " -f$count3` 
        echo $ln1 $phoneme1 $phoneme2 >> hmm_syllable_lab_with_begin_end_phone/$fn
        @ count2++
        end
    @ count++
end
