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

rm -rf ../source_hybrid_seg/$LNG/for_f0_values/f0_files
rm -rf ../source_hybrid_seg/$LNG/for_f0_values/f0_txt_files
rm -rf ../source_hybrid_seg/$LNG/for_f0_values/global_minmax
rm -rf ../source_hybrid_seg/$LNG/for_f0_values/list.txt
rm -rf ../source_hybrid_seg/$LNG/for_f0_values/list_f0_txtfiles
rm -rf ../source_hybrid_seg/$LNG/for_f0_values/list_of_f0
rm -rf ../source_hybrid_seg/$LNG/for_f0_values/local_minmax

rm -rf ../source_hybrid_seg/$LNG/prompt-lab
rm -rf ../source_hybrid_seg/$LNG/list
rm -rf ../source_hybrid_seg/$LNG/syldict

rm -rf ../source_htsvoice/$LNG/raw_files/raw
rm -rf ../source_htsvoice/$LNG/raw_files/wav_48KHz
rm -rf ../source_htsvoice/$LNG/utts	
rm -rf ../source_htsvoice/$LNG/raw_files/wav_list
