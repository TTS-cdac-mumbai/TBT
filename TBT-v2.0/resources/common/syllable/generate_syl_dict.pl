#!/usr/local/bin/perl 

open(F1, "<etc/txt.done.data");
#binmode (F1);

%dict={};
%hoh={};
$prevword='';
$countrow=1;$countword=1;
while(<F1>) {
	chomp;
        print "\n line=$_";
	@text =  split(/\"/, $_);
	$text[1] =~ s/[#%&\$*+()!?\.\,\']//g;
	$text[1] =~ s/-/ /g;
	@words = split(/ /, $text[1]);
	@monoWords;
	@syllWords;
	$countword=0;
	foreach $word(@words)
	{
		#print "\nrow $countrow word $countword";
		$word =trim($word);
		if($word ne NULL && $word ne "*" && $word ne ""){
			$dict{$word}=$dict{$word}+1;
			system("rm wordpronunciation");
			system("./unified-parser   $word    0 0 0 0");
                        print "\n word=$word";
			if(open(F2, "<wordpronunciation")){
				while(<F2>) {
					chomp;
					$_ =~ s/\(set\! wordstruct \'\(//g;
					$_ =~ s/\)//g;
					$_ =~ s/[0 ]//g;
                                        print "the first wordpronunciationsyldict1=$_\n";
                                        # print "\n monoword=$_";
					@monoWords =  split(/\(\(/, $_);
				}
				close(F2);
			}else{
				print "errormono $word\n";
			}
			system("./unified-parser   $word   0 1 0 0");
			if(open(F2, "<wordpronunciation")){
				my $count;
				while(<F2>) {
					chomp;
					$_ =~ s/\(set\! wordstruct \'\(//g;
					$_ =~ s/\)//g;
					$_ =~ s/[0 ]//g;
					#print "$_ \n";
                                        print "the second wordpronunciationsyldict1=$_\n";
					@syllWords =  split(/\(\(/, $_);
				}
				close(F2);
			}else{
				print "errorsyl $word\n";
			}
			for my $i (0 .. $#syllWords)
			{
				if($syllWords[$i] ne NULL &&  $syllWords[$i] ne ""){
					print "sylword=$syllWords[$i]\t monoword=$monoWords[$i]\n";
					$syllWords[$i] =~ s/"//g;
					$syllWords[$i] =~ s/_beg//g;
					$syllWords[$i] =~ s/_mid//g;
					$syllWords[$i] =~ s/_end//g;
					$monoWords[$i] =~ s/""/ /g;
					$monoWords[$i] =~ s/"//g;
					if($monoWords[$i] ne NULL &&  $monoWords[$i] ne ""){
						$hoh{$syllWords[$i]}{"beg-$monoWords[$i]_end"}+=1;		
					}
				}
			}
			close(F2);
		}
		$prevword =$word;
		$countword++;
	}
	$countrow++;
}
close(F1);
#system("rm wordpronunciationsyldict");
open(W, ">syldict");
print W "SIL SIL\n";
print W "sp sp\n";
foreach $item (sort keys %hoh){
  foreach $iteminitem (sort keys %{$hoh{$item}}){
       
	if (!($item =~ /^[a-zA-Z]+$/)){
		if (!($item =~ /^#+$/)){
			print W "$item $iteminitem\n";
                        print  "\nitem=$item  iteminitem=$iteminitem\n";
                       
		}
	}
	#print W "$item\t$iteminitem\t$hoh{$item}{$iteminitem}\n";
  }
}

close(W);



sub trim($)
{
  my $string = shift;
  $string =~ s/^\s+//;
  $string =~ s/\s+$//;
  return $string;
}
