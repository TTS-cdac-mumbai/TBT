# awk and perl
AWK  = mawk
PERL = /usr/bin/perl
SHELL = /bin/bash


all: phone_syllable hybrid_segmentation build_htsvoice

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

