#!/bin/perl
system("rm -f output_lab_splice_festival_format/*");
open(F, '<list');
while(<F>)
{
chomp;
($tre) = split(" ");
open(FW, ">output_lab_splice_festival_format/$tre");
print FW "#\n";
open(FR, "<output_lab_splice/$tre");
while(<FR>)
{
chomp;
($c1, $c2, $c3, $c4) = split(" ");
$col1 = $c2/10000000;
print FW "$col1 125 $c3\n";
}
}
