# awk and perl

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

AWK  = mawk
PERL = /usr/bin/perl
SHELL = /bin/bash


all: phone_syllable hybrid_segmentation build_htsvoice test_htsvoice

phone_syllable:	
	@ (cd scripts; $(SHELL) process_phone_syllable.sh $(LNG)) 2> error_log_phone_syllable

wave_processing:
	@ (cd scripts; $(SHELL) processing_on_wave.sh)

wav2raw:
	@ (cd scripts; $(SHELL) wav2raw_conversion.sh)

f0_calculation:
	@ (cd scripts; $(SHELL) f0_calculation_master.sh $(GENDER)) 2> error_log_f0_calculation

hybrid_segmentation:
	@ (cd scripts; $(SHELL) process_hybrid_segmentation.sh $(GENDER) $(LNG)) 2> error_log_hybrid_segmentation

utt_generation:
	@ (cd scripts; $(SHELL) utt_generation.sh $(LNG)) 

build_htsvoice:
	@ (cd scripts; $(SHELL) process_htsvoice.sh $(LNG)) 2> error_log_htsvoice

test_htsvoice:
	@ (cd scripts; $(SHELL) load_testing.sh $(LNG))

.PHONY: clean

clean:
	@ (cd scripts; $(SHELL) clean_data.sh $(LNG))
