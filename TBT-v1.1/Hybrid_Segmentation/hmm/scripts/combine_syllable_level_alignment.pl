#!/bin/perl
# ----------------------------------------------------------------- #
# 							            #
#                   Author - Aswin Shanmugam S		            #
#        DON Lab, Indian Institute of Technology - Madras           #
# 							            #
# ----------------------------------------------------------------- #
   
$ViterbiAlignment_Out_File = "align.mlf";
$phoneLabDir = "hybrid_phone_lab";
$labRemoveList = "list_remove_duplicates";

open(VITERBI_ALIGNMENT_OUTPUT, "<",$ViterbiAlignment_Out_File);
@syllableLvlAlignment = <VITERBI_ALIGNMENT_OUTPUT>;
close(VITERBI_ALIGNMENT_OUTPUT);

$keyLabDir = "keylab";

open(LAB_REMOVE_LIST, ">",$labRemoveList);

for ($lineCounter = 1; $lineCounter < @syllableLvlAlignment; ) { #Sentence loop begin
	$currentLine = $syllableLvlAlignment[$lineCounter];
	chomp($currentLine);
	$lineCounter++;
	($val1, $val2) = split(/-/,$currentLine,2);
	($val3, $val4) = split(/\./,$val2,2);
	$val1=~s/\"\*\///gs;
	$labFileName = $val1;
	$currentLabFileName = $labFileName;
	$keyLabFileName = $keyLabDir."/".$currentLabFileName.".lab".".keylab";

	open(KEY_LAB_FILE_DATA,"<",$keyLabFileName);
	@keyLab = <KEY_LAB_FILE_DATA>;
	close(KEY_LAB_FILE_DATA);

	open(PHONE_LAB_FILE,">",$phoneLabDir."/".$labFileName.".lab");
	print PHONE_LAB_FILE "#\n";
	
	$keyLablineCounter = 2;
	$prevEndTime = 0;

	while( $labFileName eq $currentLabFileName) { #Syllable loop begin
		$syllableInfo = @keyLab[$keyLablineCounter];
		chomp($syllableInfo);
		($endTimeSyllable, $labMiddleColumn, $syllable, $semicolon1, $dummy, $spliceSyllablePath, $semicolon2) = split(/ /,$syllableInfo,7);
		($val5, $val6) = split(/-/,$spliceSyllablePath,2);
		if($val3 ne $val6) {
			print LAB_REMOVE_LIST "$labFileName.lab\n"
		}

		$currentLine = $syllableLvlAlignment[$lineCounter];
		chomp($currentLine);
		$lineCounter++;
		($startTimePhone, $endTimePhone, $phone, $likelihood, $syllable) = split(/ /,$currentLine,5);
		
		while($currentLine ne ".") { #Phone loop begin
  			$absoluteEndTime = $prevEndTime + ( ($endTimePhone - $startTimePhone) / 10000000 );
  			$currentLine = $syllableLvlAlignment[$lineCounter];
			chomp($currentLine);
			$lineCounter++;
			if($currentLine eq ".") {
				print PHONE_LAB_FILE "$endTimeSyllable $labMiddleColumn $phone\n";
				$prevEndTime = $endTimeSyllable;
			}
			else {
				print PHONE_LAB_FILE "$absoluteEndTime $labMiddleColumn $phone\n";
				$prevEndTime = $absoluteEndTime;
			}
		($startTimePhone, $endTimePhone, $phone, $likelihood, $syllable) = split(/ /,$currentLine,5);
		} #Phone loop end
		
		$currentLine = $syllableLvlAlignment[$lineCounter];
		chomp($currentLine);
		$lineCounter++;
		($col1, $col2) = split(/-/,$currentLine,2);
		($val3, $val4) = split(/\./,$col2,2);
		$col1=~s/\"\*\///gs;
		$labFileName = $col1;
		$keyLablineCounter++;
		$previousSyllableNumber = $val4;
	} #Syllable loop end
	
	close(PHONE_LAB_FILE);
	$lineCounter--;
} #Sentence loop end
close(LAB_REMOVE_LIST);
