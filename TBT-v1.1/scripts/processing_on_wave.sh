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
rm -rf temp/hybrid_seg/wav_16KHz/
mkdir temp/hybrid_seg/wav_16KHz/
rm -rf temp/hybrid_seg/SIL_added_wav_48KHz/
mkdir temp/hybrid_seg/SIL_added_wav_48KHz/

cp -r resources/common/hybrid_seg/sil_detect_add.py temp/hybrid_seg
cp -r resources/common/hybrid_seg/downsample.py temp/hybrid_seg

cd temp/hybrid_seg
ls ../../input/wav_48KHz/*.wav > list
python sil_detect_add.py list 
python downsample.py
cd ../..
