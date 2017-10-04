#!/bin/tcsh

# input --> list_remove

set count = 1
set len = `cat $1|wc -l`
while ($count <= $len)
set word = `cat $1|head -$count|tail -1`
   echo "Sentence Level alignment for: "$word
   cp fal_hybrid_sentence_festival_format/$word hybrid_phone_lab/$word
@ count++
end
