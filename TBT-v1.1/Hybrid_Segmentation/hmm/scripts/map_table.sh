#!/bin/tcsh -f
if( $# != 1 ) then
  echo " Argument ---> wavlist "
exit (-1)
endif

set num = `cat $1 | wc -l`
set count = 1
while ( $count <= $num )
    set wav = `head -$count $1 | tail -1`
    set wwav = `echo $wav  | cut -d "." -f1`
    echo $wwav.wav $wwav.mfc >> map_table
    @ count++
end  
