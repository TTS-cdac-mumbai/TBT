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

LNG=$1

rm -rf wav_16KHz
rm -rf Phonelist_Description
rm -rf hmm/prompt-lab

cp -r ../resources/languages/$LNG/hybrid_seg/Phonelist_Description Phonelist_Description/
mv ../temp/hybrid_seg/wav_16KHz wav_16KHz/
cp -r ../temp/hybrid_seg/prompt-lab hmm/
cp -r ../temp/hybrid_seg/syldict hmm/

cp -r ../resources/common/hybrid_seg/scripts/run_1.sh ../resources/common/hybrid_seg/scripts/run_2.sh ../resources/common/hybrid_seg/scripts/run_3.sh ../resources/common/hybrid_seg/scripts/run_4.sh ./

sh run_1.sh
sh run_2.sh
sh run_3.sh
sh run_4.sh
