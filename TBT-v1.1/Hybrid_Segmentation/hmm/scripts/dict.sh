#!/bin/tcsh -f
if( $# != 1 ) then
  echo " Argument ---> phonelist "
exit (-1)
endif

set num = `cat $1 | wc -l`
set count = 1
while ( $count <= $num )
    set phone = `head -$count $1 | tail -1`
    echo $phone $phone >> dict
    @ count++
end  
