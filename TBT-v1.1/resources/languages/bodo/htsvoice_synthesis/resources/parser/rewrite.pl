#!/bin/perl
open(FW, ">parser/etc/txt.done.data");
print FW "(test \" ";
open(F, '<sentence.txt');
while(<F>)
{
chomp;
($a) = split("\n");
print FW "$a ";
}
print FW "\")";

