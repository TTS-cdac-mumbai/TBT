#!/bin/perl
# ----------------------------------------------------------------- #
# 							            #
#                   Author - Aswin Shanmugam S		            #
#        DON Lab, Indian Institute of Technology - Madras           #
# 							            #
# ----------------------------------------------------------------- #

system("rm -f Transcription_splice.txt");
open(F, '<list');
while(<F>)
{
chomp;
($f) = split(" ");
print "File : $f\n";
$a = substr($f, 0, length($f) - 4);
open(FW, ">>Transcription_splice.txt");
$count = 1;
$prevTime = 0;
open(FR, "<syllab/$f");
while(<FR>)
{
chomp;
($c1, $c2, $c3) = split(" ");
if($count < 10 ) {
$syllableFileName = $a."-00".$count;
}
elsif($count <100) {
$syllableFileName = $a."-0".$count;
}
else {
$syllableFileName = $a."-".$count;
}
if($c1 eq "#")
{
}
else
{
$sample1=$prevTime*8000;
$sample2=$c1*8000;
$prevTime=$c1;
system("ch_wave wav_8KHz/$a.wav -o splice_wav/$syllableFileName.wav -from $sample1 -to $sample2");
$count = $count + 1;
print FW "splice_wav/$syllableFileName $c3\n";
}
}
}
