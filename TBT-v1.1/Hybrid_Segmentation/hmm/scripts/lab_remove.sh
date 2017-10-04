#!/bin/tcsh

# input --> list_remove

set count = 1
set len = `cat $1|wc -l`
while ($count <= $len)
   set filename = `cat $1|head -$count|tail -1`
   echo "Alignment within Syllables failed for : "$filename
   rm -f hybrid_phone_lab/$filename
@ count++
end
