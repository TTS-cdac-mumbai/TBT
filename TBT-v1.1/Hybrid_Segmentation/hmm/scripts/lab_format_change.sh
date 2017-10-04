#!/bin/tcsh -f
rm -f output_lab_phone_htk_format/*

set num = `cat $1 | wc -l`
set count = 1
set scalefactor = 10000000
while ( $count <= $num )
    set fn = `head -$count $1 | tail -1`
    set n1 = 0
    set num2 = `cat output_lab_phone_flux/$fn | wc -l`
    set count2 = 2
    while ( $count2 <= $num2 )
        set ln1 = `head -$count2 output_lab_phone_flux/$fn | tail -1`
        set var1 = `echo $ln1 | cut -d " " -f1`
        set n2 = `echo "" | awk -v a="$var1" -v b="$scalefactor" '{ print (a * b); }'`
        set phone = `echo $ln1 | cut -d " " -f3`
        echo $n1 $n2 $phone >> output_lab_phone_htk_format/$fn
        set n1 = $n2
    @ count2++
    end
@ count++
end
