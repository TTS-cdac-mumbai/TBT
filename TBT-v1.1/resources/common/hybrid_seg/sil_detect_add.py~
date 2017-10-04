#! /usr/bin/python

###############################################

#Designed and Developed by Atish and Rachana
#Script to detect the silence in wave file and 
#add silence in the same

###############################################

import wave
import struct
import sys
import scipy.io.wavfile as wv   
from scipy.io import wavfile as wv                                                                                              
import numpy as np
import matplotlib.pyplot as plt
import shutil
import os.path
	
files = sys.argv[1]
filenames = open(files,'r')
dst = "SIL_added_wav_48KHz/"

filelines = filenames.readlines()

for name in filelines:
	name = name[:-1]
	w = wave.open(name)
	(fs,fname) = wv.read(name)
	astr = w.readframes(w.getnframes())
        # convert binary chunks to short 
	a = struct.unpack("%ih" % (w.getnframes()* w.getnchannels()), astr)
	signal = [float(val) / pow(2, 15) for val in a]

	dataindex=-1
	count1=0
	count2=0
	for index,element in enumerate(signal):
    		if element!=0:
			break
    		count1=count1+1
	indexrev=len(signal)-1
	
	for var in range(indexrev, 0, -1):
		if(signal[var])!=0:
			break
    		count2=count2+1	
	indexrev=indexrev-1

	wav=name[22:]
	#print wav
	dst_path = dst + wav

	if count1<4800 or count2<4800:
   		dur = fs/10 #100ms
   		temp1 = np.array([0]*dur)
   		temp2 = np.append(list(temp1),list(fname))
   		temp3 = np.append(list(temp2),([0]*dur))
   		data = np.array(temp3,np.int16)
        	wv.write(dst_path,fs,data)

	else:
		shutil.copy2(fname, "SIL_added_wav_48KHz/")
