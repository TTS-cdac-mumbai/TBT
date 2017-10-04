#!usr/bin/perl
my $file_path = $ARGV[0];
print "Processing $utf8FileIn ... $file_path \n";
open(FILE, "<$file_path");
binmode( FILE );
my $endln="\n";
my $line = ();
my $t;
while($line = <FILE>)
{		
	my @tempsent;
    $line =~ s/\. +/\.$endln/g;
	my @words = split(/\$/,$line);
	my $engl = 0;
	my $otherl = 0;
    my  $word=$words[0];
    my $filename=$words[1];
$word =~ s/\s+/\_/g;
print "\n word=$word filename=$filename";
system("perl synth_text_asr.pl $word  $filename");
	#foreach my $word (@words)
	#{
	#	print "The word is $word";
		#system("perl il_parser_pronun_dict_pranaw1_base.pl $word");
		#system("perl synth_text_asr.pl $word");
	#}
}
