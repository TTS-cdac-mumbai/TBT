#!/bin/perl
system("rm -rf output_lab_hmm_festival_format");
system("mkdir output_lab_hmm_festival_format");
open(F, '<hmm/list');
while(<F>)
{
chomp;
($tre) = split(" ");
open(FW, ">output_lab_hmm_festival_format/$tre");
print FW "#\n";
open(FR, "<hmm/output_lab_hmm/$tre");
while(<FR>)
{
chomp;
($c1, $c2, $c3, $c4) = split(" ");
$col1 = $c2/10000000;
print FW "$col1 125 $c3\n";
}
}
