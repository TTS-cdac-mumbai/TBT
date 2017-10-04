#!/bin/perl
system("rm -f Transcription.txt");
open(F, '<list');
while(<F>)
{
chomp;
($f) = split(" ");
$a = substr($f, 0, length($f) - 4);
open(FW, ">>Transcription.txt");
$count = 1;
print FW "wav_8KHz/$a\n";
open(FR, "<output_lab_temp/$f");
while(<FR>)
{
chomp;
($c1, $c2, $c3) = split(" ");
if($c1 eq "#")
{
}
else
{
$count = $count + 1;
print FW "$c3\n";
}
}
}
