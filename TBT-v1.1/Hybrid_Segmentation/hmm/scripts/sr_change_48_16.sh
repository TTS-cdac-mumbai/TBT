#!/bin/tcsh -f
if( $# != 1 ) then
  echo " Argument ---> wav_list "
exit (-1)
endif

set num = `cat $1 | wc -l`
set count = 1
while ( $count <= $num )
    set wav = `head -$count $1 | tail -1`
    ch_wave -f 48000 -F 16000 -o wav_16KHz/$wav wav_48KHz/$wav
    @ count++
end  
