# *****************************************************************************************
# Project:              Compute Feature
# Subtree:              ./src
# Filename:             Makefile for feature Extraction
# Programmer:           Hema A Murthy
# Host System:          Linux
# Date Created:         11/Mar/99
# Project:              Extract Features from a speech data file
# *****************************************************************************************

# makefile for generating codebook

SRCDIR = ./src
DSPLIB = ./src
OBJDIR = ./obj
BINDIR = ./bin
CFLAGS = -g -c -Wall  
CC = gcc

all :   $(BINDIR)/ComputeFeature 

$(BINDIR)/ComputeFeature : $(OBJDIR)/ComputeFeature.o \
	$(OBJDIR)/DspLibrary.o \
	$(OBJDIR)/SphereInterface.o \
	$(OBJDIR)/BatchProcessWaveform.o \
	$(OBJDIR)/InitAsdf.o \
	$(OBJDIR)/PthLibrary.o \
	$(OBJDIR)/FmtsLibrary.o \
        $(OBJDIR)/FeatureMappingToFunctions.o\
        $(OBJDIR)/HashTable.o \
	$(OBJDIR)/QuickSort.o
	$(CC) -g -o $(BINDIR)/ComputeFeature \
	-I$(DSPLIB)/fe \
	-I$(NISTINC) \
	-L$(NISTLIB) \
	$(OBJDIR)/ComputeFeature.o \
	$(OBJDIR)/DspLibrary.o \
	$(OBJDIR)/SphereInterface.o \
	$(OBJDIR)/BatchProcessWaveform.o \
	$(OBJDIR)/InitAsdf.o \
	$(OBJDIR)/PthLibrary.o \
	$(OBJDIR)/FmtsLibrary.o \
	$(OBJDIR)/HashTable.o \
	$(OBJDIR)/QuickSort.o \
        $(OBJDIR)/FeatureMappingToFunctions.o \
	-lsp \
	-lutil \
	-lm

$(OBJDIR)/ComputeFeature.o: $(SRCDIR)/ComputeFeature.c
	$(CC) $(CFLAGS) -o $(OBJDIR)/ComputeFeature.o $(SRCDIR)/ComputeFeature.c \
	-I$(NISTINC) \
	-I$(DSPLIB)/fe 

$(OBJDIR)/DspLibrary.o : $(DSPLIB)/DspLibrary.c 
	$(CC) $(CFLAGS) -o $(OBJDIR)/DspLibrary.o $(DSPLIB)/DspLibrary.c \
	-I$(NISTINC) \
	-I$(DSPLIB)/fe 

$(OBJDIR)/BatchProcessWaveform.o : $(DSPLIB)/BatchProcessWaveform.c 
	$(CC) $(CFLAGS) -o $(OBJDIR)/BatchProcessWaveform.o $(DSPLIB)/BatchProcessWaveform.c \
	-I$(NISTINC) \
	-I$(DSPLIB)/fe

$(OBJDIR)/PthLibrary.o : $(DSPLIB)/PthLibrary.c 
	$(CC) $(CFLAGS) -o $(OBJDIR)/PthLibrary.o $(DSPLIB)/PthLibrary.c \
	-I$(NISTINC) \
	-I$(DSPLIB)/fe

$(OBJDIR)/FmtsLibrary.o : $(DSPLIB)/FmtsLibrary.c 
	$(CC) $(CFLAGS) -o $(OBJDIR)/FmtsLibrary.o $(DSPLIB)/FmtsLibrary.c \
	-I$(NISTINC) \
	-I$(DSPLIB)/fe

$(OBJDIR)/SphereInterface.o : $(DSPLIB)/SphereInterface.c 
	$(CC) $(CFLAGS) -o $(OBJDIR)/SphereInterface.o $(DSPLIB)/SphereInterface.c \
	-I$(NISTINC) \
	-I$(DSPLIB)/fe 

$(OBJDIR)/InitAsdf.o : $(DSPLIB)/InitAsdf.c 
	$(CC) $(CFLAGS) -o $(OBJDIR)/InitAsdf.o $(DSPLIB)/InitAsdf.c -I$(NISTINC) -I$(DSPLIB)/fe 

$(OBJDIR)/HashTable.o : $(DSPLIB)/HashTable.c 
	$(CC) $(CFLAGS) -o $(OBJDIR)/HashTable.o $(DSPLIB)/HashTable.c \
	-I$(NISTINC) \
	-I$(DSPLIB)/fe 

$(OBJDIR)/FeatureMappingToFunctions.o : $(DSPLIB)/FeatureMappingToFunctions.c 
	$(CC) $(CFLAGS) -o $(OBJDIR)/FeatureMappingToFunctions.o $(DSPLIB)/FeatureMappingToFunctions.c \
	-I$(NISTINC) \
	-I$(DSPLIB)/fe 


$(OBJDIR)/QuickSort.o : $(DSPLIB)/QuickSort.c 
	$(CC) $(CFLAGS) -o $(OBJDIR)/QuickSort.o $(DSPLIB)/QuickSort.c \
	-I$(NISTINC) \
	-I$(DSPLIB)/fe 

clean: 
	rm -fr $(BINDIR)/ComputeFeature $(OBJDIR)/DspLibrary.o $(OBJDIR)/BatchProcessWaveform.o $(OBJDIR)/InitAsdf.o $(OBJDIR)/HashTable.o $(OBJDIR)/FeatureMappingToFunctions $(OBJDIR)/SphereInterface.o $(OBJDIR)/ComputeFeature.o $(OBJDIR)/QuickSort.o \
	$(OBJDIR)/PthLibrary.o $(OBJDIR)/FmtsLibrary.o $(OBJDIR)/QuickSort.o $(OBJDIR)/HashTable.o	
