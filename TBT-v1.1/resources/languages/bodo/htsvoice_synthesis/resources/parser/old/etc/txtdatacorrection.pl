open($in, "<txt.done.data");
open($out, ">txt.done.data_new");
while( my $line = <$in>){
	my @part = split(/"/, $line);
	print $out "$part[0] \" ";
	my $new = trim($part[1]);
	$new =~ s/\?//g;
	$new =~ s/,//g;
	$new = trim($new);
	$new =~ s/\./ /g;
	$new =~ s/ - /-/g;
	$new =~ s/ / * /g;

		print $out "$new.\"";
		print $out $part[2];

} 

sub  trim { my $s = shift; $s =~ s/^\s+|\s+$//g; return $s };
