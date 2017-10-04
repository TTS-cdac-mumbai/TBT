# *****************************************************************************************
# Project:              WordsAfterSilenceRemoval
# Subtree:              /home2/hema/front-end-dsp/src
# Filename:             Makefile for feature Extraction
# Programmer:           Hema A Murthy
# Host System:          Linux
# Date Created:         25/March/2008
# Project:              Extract Segment boundaries
# *****************************************************************************************


SRCDIR = ./src
DSPLIB = ../src
OBJDIR = ./obj
BINDIR = ./bin
CFLAGS = -g -c -Wall  
CC = gcc

all:    $(BINDIR)/WordsWithSilenceRemoval $(BINDIR)/SegmentationFlux $(BINDIR)/WordSegmentWithSilenceRemoval 

$(BINDIR)/WordsWithSilenceRemoval : $(OBJDIR)/WordsWithSilenceRemoval.o \
	$(OBJDIR)/DspLibrary.o \
	$(OBJDIR)/SphereInterface.o \
	$(OBJDIR)/BatchProcessWaveform.o \
	$(OBJDIR)/InitAsdf.o \
        $(OBJDIR)/FeatureMappingToFunctions.o\
        $(OBJDIR)/QuickSort.o \
	$(OBJDIR)/PthLibrary.o \
	$(OBJDIR)/FmtsLibrary.o \
        $(OBJDIR)/HashTable.o 
	$(CC) -g -o $(BINDIR)/WordsWithSilenceRemoval \
	-I$(DSPLIB)\
	-I$(NISTINC) \
	-L$(NISTLIB) \
	$(OBJDIR)/WordsWithSilenceRemoval.o \
	$(OBJDIR)/DspLibrary.o \
	$(OBJDIR)/SphereInterface.o \
	$(OBJDIR)/BatchProcessWaveform.o \
	$(OBJDIR)/InitAsdf.o \
        $(OBJDIR)/PthLibrary.o \
	$(OBJDIR)/FmtsLibrary.o \
        $(OBJDIR)/FeatureMappingToFunctions.o\
        $(OBJDIR)/HashTable.o \
        $(OBJDIR)/QuickSort.o \
	-lsp \
	-lutil \
	-lm

$(BINDIR)/SegmentationFlux : $(OBJDIR)/SegmentationFlux.o \
	$(OBJDIR)/DspLibrary.o \
	$(OBJDIR)/SphereInterface.o \
	$(OBJDIR)/BatchProcessWaveform.o \
	$(OBJDIR)/InitAsdf.o \
        $(OBJDIR)/FeatureMappingToFunctions.o\
        $(OBJDIR)/QuickSort.o\
	$(OBJDIR)/PthLibrary.o \
	$(OBJDIR)/FmtsLibrary.o \
	$(OBJDIR)/HashTable.o
	$(CC) -g -o $(BINDIR)/SegmentationFlux \
	-I$(DSPLIB)\
	-I$(NISTINC) \
	-L$(NISTLIB) \
	$(OBJDIR)/SegmentationFlux.o \
	$(OBJDIR)/DspLibrary.o \
	$(OBJDIR)/SphereInterface.o \
	$(OBJDIR)/BatchProcessWaveform.o \
	$(OBJDIR)/InitAsdf.o \
        $(OBJDIR)/PthLibrary.o \
	$(OBJDIR)/FmtsLibrary.o \
        $(OBJDIR)/FeatureMappingToFunctions.o\
        $(OBJDIR)/HashTable.o \
        $(OBJDIR)/QuickSort.o \
	-lsp \
	-lutil \
	-lm

$(BINDIR)/WordSegmentWithSilenceRemoval : $(OBJDIR)/WordSegmentWithSilenceRemoval.o \
	$(OBJDIR)/DspLibrary.o \
	$(OBJDIR)/SphereInterface.o \
	$(OBJDIR)/BatchProcessWaveform.o \
	$(OBJDIR)/InitAsdf.o \
        $(OBJDIR)/FeatureMappingToFunctions.o\
        $(OBJDIR)/QuickSort.o\
       	$(OBJDIR)/PthLibrary.o \
	$(OBJDIR)/FmtsLibrary.o \
	$(OBJDIR)/HashTable.o 
	$(CC) -g -o $(BINDIR)/WordSegmentWithSilenceRemoval \
	-I$(DSPLIB)\
	-I$(NISTINC) \
	-L$(NISTLIB) \
	$(OBJDIR)/WordSegmentWithSilenceRemoval.o \
	$(OBJDIR)/DspLibrary.o \
	$(OBJDIR)/SphereInterface.o \
	$(OBJDIR)/BatchProcessWaveform.o \
	$(OBJDIR)/InitAsdf.o \
	$(OBJDIR)/PthLibrary.o \
	$(OBJDIR)/FmtsLibrary.o \
	$(OBJDIR)/FeatureMappingToFunctions.o\
        $(OBJDIR)/HashTable.o \
        $(OBJDIR)/QuickSort.o \
	-lsp \
	-lutil \
	-lm

$(OBJDIR)/SegmentationFlux.o: $(SRCDIR)/SegmentationFlux.c
	$(CC) $(CFLAGS) -o $(OBJDIR)/SegmentationFlux.o $(SRCDIR)/SegmentationFlux.c -I$(NISTINC) -I$(DSPLIB)

$(OBJDIR)/WordsWithSilenceRemoval.o: $(SRCDIR)/WordsWithSilenceRemoval.c
	$(CC) $(CFLAGS) -o $(OBJDIR)/WordsWithSilenceRemoval.o $(SRCDIR)/WordsWithSilenceRemoval.c -I$(NISTINC) -I$(DSPLIB)

$(OBJDIR)/WordSegmentWithSilenceRemoval.o: $(SRCDIR)/WordSegmentWithSilenceRemoval.c
	$(CC) $(CFLAGS) -o $(OBJDIR)/WordSegmentWithSilenceRemoval.o $(SRCDIR)/WordSegmentWithSilenceRemoval.c -I$(NISTINC) -I$(DSPLIB)

$(OBJDIR)/DspLibrary.o : $(DSPLIB)/DspLibrary.c 
	$(CC) $(CFLAGS) -o $(OBJDIR)/DspLibrary.o $(DSPLIB)/DspLibrary.c \
	-I$(NISTINC) -I$(DSPLIB)

$(OBJDIR)/BatchProcessWaveform.o : $(DSPLIB)/BatchProcessWaveform.c 
	$(CC) $(CFLAGS) -o $(OBJDIR)/BatchProcessWaveform.o $(DSPLIB)/BatchProcessWaveform.c -I$(NISTINC) -I$(DSPLIB)


$(OBJDIR)/PthLibrary.o : $(DSPLIB)/PthLibrary.c 
	$(CC) $(CFLAGS) -o $(OBJDIR)/PthLibrary.o $(DSPLIB)/PthLibrary.c \
	-I$(NISTINC) \
	-I$(DSPLIB)/fe

$(OBJDIR)/FmtsLibrary.o : $(DSPLIB)/FmtsLibrary.c 
	$(CC) $(CFLAGS) -o $(OBJDIR)/FmtsLibrary.o $(DSPLIB)/FmtsLibrary.c \
	-I$(NISTINC) \

$(OBJDIR)/SphereInterface.o : $(DSPLIB)/SphereInterface.c 
	$(CC) $(CFLAGS) -o $(OBJDIR)/SphereInterface.o $(DSPLIB)/SphereInterface.c -I$(NISTINC) -I$(DSPLIB)

$(OBJDIR)/InitAsdf.o : $(DSPLIB)/InitAsdf.c 
	$(CC) $(CFLAGS) -o $(OBJDIR)/InitAsdf.o $(DSPLIB)/InitAsdf.c -I$(NISTINC) -I$(DSPLIB)

$(OBJDIR)/HashTable.o : $(DSPLIB)/HashTable.c 
	$(CC) $(CFLAGS) -o $(OBJDIR)/HashTable.o $(DSPLIB)/HashTable.c \
	-I$(NISTINC) \
	-I$(DSPLIB)/fe 

$(OBJDIR)/QuickSort.o : $(DSPLIB)/QuickSort.c 
	$(CC) $(CFLAGS) -o $(OBJDIR)/QuickSort.o $(DSPLIB)/QuickSort.c \
	-I$(NISTINC) \
	-I$(DSPLIB)/fe 

$(OBJDIR)/FeatureMappingToFunctions.o : $(DSPLIB)/FeatureMappingToFunctions.c 
	$(CC) $(CFLAGS) -o $(OBJDIR)/FeatureMappingToFunctions.o $(DSPLIB)/FeatureMappingToFunctions.c \
	-I$(NISTINC) \
	-I$(DSPLIB)/fe 
clean: 
	rm -fr $(BINDIR)/WordsWithSilenceRemoval -fr $(BINDIR)/SegmentationFlux $(BINDIR)/WordSegmentWithSilenceRemoval $(OBJDIR)/DspLibrary.o $(OBJDIR)/BatchProcessWaveform.o $(OBJDIR)/InitAsdf.o ./obj/HashTable.o ../obj/FeatureMappingToFunctions $(OBJDIR)/SphereInterface.o $(OBJDIR)/WordsWithSilenceRemoval.o  $(OBJDIR)/WordSegmentWithSilenceRemoval.o $(OBJDIR)/SegmentationFlux.o $(OBJDIR)/PthLibrary.o $(OBJDIR)/FmtsLibrary.o


