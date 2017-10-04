# **********************************************************
# Project:		ARG General libraries
# SubTree:		/home/rachana/TBT-Toolkit-master/TBT-v1.1/Hybrid_Segmentation/front-end-dsp/nist
# Filename:		Makefile
# Programmer:		J. Fiscus
# Organization:		NIST/NCSL/DIV 670/Auto Rec. Group
# Host System:		SUN 4 OS/4.1.1 (UNIX)
# Date Created:		05/28/92
#
# **********************************************************
# Makefile for Root Project Directory
# **********************************************************
PROJDIR = /home/rachana/TBT-Toolkit-master/TBT-v1.1/Hybrid_Segmentation/front-end-dsp/nist
SUBS = src
SHELL = /bin/sh

it install depend clean:
	@-X=`pwd`; \
	for i in $(SUBS); \
	do echo '<<<' $$i '>>>'; cd $$X/$$i; \
	make $@ PROJDIR=$(PROJDIR) ; done

bare: clean
	rm -f bin/* lib/packages/*.pas lib/lib*.a 
