#! bin/bash

###########################################################################
##                                                                       ##
##                Centre for Development of Advanced Computing           ##
##                               Mumbai                                  ##
##                         Copyright (c) 2017                            ##
##                        All Rights Reserved.                           ##
##                                                                       ##
##  Permission is hereby granted, free of charge, to use and distribute  ##
##  this software and its documentation without restriction, including   ##
##  without limitation the rights to use, copy, modify, merge, publish,  ##
##  distribute, sublicense, and/or sell copies of this work, and to      ##
##  permit persons to whom this work is furnished to do so, subject to   ##
##  the following conditions:                                            ##
##   1. The code must retain the above copyright notice, this list of    ##
##      conditions and the following disclaimer.                         ##
##   2. Any modifications must be clearly marked as such.                ##
##   3. Original authors' names are not deleted.                         ##
##   4. The authors' names are not used to endorse or promote products   ##
##      derived from this software without specific prior written        ##
##      permission.                                                      ##
##                                                                       ##
##                                                                       ##
###########################################################################
##                                                                       ##
##                     TTS Building Toolkit                              ##
##                                                                       ##
##            Designed and Developed by Atish and Rachana                ##
##          		Date:  April 2017                                ##
##                                                                       ## 
###########################################################################

cd ..

rm -rf temp/htsvoice/raw_files
mkdir temp/htsvoice/raw_files
cp -r resources/common/htsvoice/raw_conversion.scr temp/htsvoice/raw_files

rm -rf temp/htsvoice/raw_files/wav_48KHz
mkdir temp/htsvoice/raw_files/wav_48KHz
cp -r temp/hybrid_seg/SIL_added_wav_48KHz/*.wav temp/htsvoice/raw_files/wav_48KHz

cd temp/htsvoice/raw_files

ls wav_48KHz/*.wav > wav_list   ### taking Sil added wavefiles.
./raw_conversion.scr wav_list
cd ../../..

echo -e "\n\n$(tput setaf 2)==Completed conversion of wav_48KHz to Raw==$(tput sgr0) \n\n"
