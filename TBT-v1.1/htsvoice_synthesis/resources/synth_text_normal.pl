#!/usr/bin/perl
# ----------------------------------------------------------------- #
#Ministry Of Communications & Information Technology, Govt. Of India#
#          and Indian Institute Of Technology - Madras              #
#                    developed by TTS Group                         #
#                  http://lantana.tenet.res.in/                     #
#                    Copyright (c) 2009-2015                        #
# ----------------------------------------------------------------- #
#                                                                   #
#                                                                   #
# All rights reserved.                                              #
#                                                                   #
# Redistribution and use in source and binary forms, with or        #
# without modification, are permitted provided that the following   #
# conditions are met:                                               #
#                                                                   #
# - Redistributions of source code must retain the above copyright  #
#   notice, this list of conditions and the following disclaimer.   #
# - Redistributions in binary form must reproduce the above         #
#   copyright notice, this list of conditions and the following     #
#   disclaimer in the documentation and/or other materials provided #
#   with the distribution.                                          #
# - Neither the name of the HTS working group nor the names of its  #
#   contributors may be used to endorse or promote products derived #
#   from this software without specific prior written permission.   #
#                                                                   #
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND            #
# CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,       #
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF          #
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          #
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS #
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          #
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   #
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     #
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON #
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   #
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    #
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           #
# POSSIBILITY OF SUCH DAMAGE.                                       #
# ----------------------------------------------------------------- #


my $story=$ARGV[0];
if($story eq "") {
  print "Usage: perl $0 <inputstory>\n";
  print "Exiting at ". __FILE__ . ":". __LINE__ ."\n";
  exit -1;
}

$story =~ s/(^\s+|\s+$)//g;
$story =~ s/\।/\./g;
$story =~ s/\-/\ /g;
$story =~ s/\"//g;
$story =~ s/\//\ \/\ /g;
print "The story to be synthesized is: $story\n";
print "The output syllable file is: $out_word_file_name\n";

my @words = split(/\. /, $story);
print "The words are : \n";

$r=`rm out/*.wav`;
for(my $i=1; $i<=@words; $i++)
{
     $words[$i-1] =~ s/(^\s+|\s+$)//g;
        $words[$i-1] =~ s/[a-zA-Z]//g;
        $words[$i-1] =~ s/` |'//g;
     $r=`sh synthhindi.sh \"$words[$i-1]\"`;
     print $r;
     if($i < 10 ) {
        $FileName = "000".$i;
     }
     elsif($i <100) {
        $FileName = "00".$i;
     }
     elsif($i <1000) {
        $FileName = "0".$i;
     }
     else {
        $FileName = $i;
     }

   #  $r=`mv gen/hts_engine/test.wav out/$FileName.wav`;
  $r=`mv test.wav out/$FileName.wav`;

}
$r=`normalize-audio out/*.wav`; 
$r=`ch_wave out/*.wav -o out.wav`;
#system("play out.wav");

