#! /bin/sh
# the next line restarts using wish \
exec wish8.6 "$0" "$@"

package require snack

snack::sound s

exec rm -rf f0_files
exec mkdir f0_files

set f [open list.txt]
set list [read $f]
close $f

foreach file $list {
 s read $file

 set fd [open [file rootname $file].f0 w]
 puts $fd [join [s pitch -method esps] \n]
 close $fd
 exec mv [file rootname $file].f0 f0_files/

}

exit
