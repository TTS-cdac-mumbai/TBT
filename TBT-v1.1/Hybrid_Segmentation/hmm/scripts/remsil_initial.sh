#!/bin/tcsh -f
if( $# != 1 ) then
  echo " Argument ---> list "
endif

rm -f output_lab_temp/*

set num = `cat $1 | wc -l`
set count = 1
while ( $count <= $num )
    set fn = `head -$count $1 | tail -1`
    echo $fn
    set num2 = `cat output_lab_with_SIL_festival_format/$fn | wc -l`
    set num3 = $num2
    @ num3--
    set ln1 = `head -1 output_lab_with_SIL_festival_format/$fn`
    echo $ln1 >> output_lab_temp/$fn 
    set count2 = 1
    while ( $count2 < $num2 )
        set cnt3 = $count2
        @ cnt3++ 
	set cnt4 = $cnt3
	@ cnt4++
        set ln1 = `head -$count2 output_lab_with_SIL_festival_format/$fn | tail -1`
        set ln2 = `head -$cnt3 output_lab_with_SIL_festival_format/$fn | tail -1`
	if ( $count2 == $num3 ) then
		set var8 = 1
	else 
		set ln3 = `head -$cnt4 output_lab_with_SIL_festival_format/$fn | tail -1`
		set var2 = `echo $ln2 | cut -d " " -f1`
		set var6 = `echo $ln3 | cut -d " " -f1`
	        set var7 = `echo "" | awk -v a="$var6" -v b="$var2" '{ print (a - b); }'`
	        set var8 = `echo "" | awk -v a="$var7" '{if (a >= 0.02) print 1; else print 0}'`
	endif
     	set var1 = `echo $ln1 | cut -d " " -f1`
        set var2 = `echo $ln2 | cut -d " " -f1`
        set var3 = `echo $ln2 | cut -d " " -f3`
        set var4 = `echo "" | awk -v a="$var2" -v b="$var1" '{ print (a - b); }'`
        set var5 = `echo "" | awk -v a="$var4" '{if (a >= 0.1) print 1; else print 0}'`
        if ( $var3 != "SIL" || ( $var5 == "1" && $var8 == "1" ) ) then
                echo $ln2 >> output_lab_temp/$fn
        endif
        @ count2++
        end
    @ count++
end
