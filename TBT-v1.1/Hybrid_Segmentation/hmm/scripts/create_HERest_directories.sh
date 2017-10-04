#!/bin/tcsh
set count = $1
set NoI = $2
while ($count < $NoI)
mkdir -p hmm$count
@ count++
end
