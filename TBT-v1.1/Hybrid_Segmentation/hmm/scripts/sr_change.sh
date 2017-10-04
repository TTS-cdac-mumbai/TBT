#!/bin/tcsh -f
if( $# != 1 ) then
  echo " Argument ---> wav_list "
exit (-1)
endif

set num = `cat $1 | wc -l`
set count = 1
while ( $count <= $num )
    set wav = `head -$count $1 | tail -1`
    ch_wave -f 16000 -F 8000 -o wav_8KHz/$wav ../wav_16KHz/$wav
    @ count++
end  
