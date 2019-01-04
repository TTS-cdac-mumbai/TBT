#! /usr/bin/python

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

import wave
import audioop
import sys
import os

def downsampleWav(src, dst, inrate=48000, outrate=16000, inchannels=1, outchannels=1):
    if not os.path.exists(src):
        #print 'Source not found!'
        return False

    if not os.path.exists(os.path.dirname(dst)):
        os.makedirs(os.path.dirname(dst))

    try:
        s_read = wave.open(src, 'r')
        s_write = wave.open(dst, 'w')
    except:
        #print 'Failed to open files!'
        return False

    n_frames = s_read.getnframes()
    data = s_read.readframes(n_frames)

    try:
        converted = audioop.ratecv(data, 2, inchannels, inrate, outrate, None)
        if outchannels == 1 & inchannels != 1:
            converted[0] = audioop.tomono(converted[0], 2, 1, 0)
    except:
       # print 'Failed to downsample wav'
        return False

    try:
        s_write.setparams((outchannels, 2, outrate, 0, 'NONE', 'Uncompressed'))
        s_write.writeframes(converted[0])
    except:
        #print 'Failed to write wav'
        return False

    try:
        s_read.close()
        s_write.close()
    except:
        #print 'Failed to close wav files'
        return False

    return True

if __name__ == "__main__":
    path_in = "SIL_added_wav_48KHz/"
    path_out = "wav_16KHz/"

    for file in os.listdir(path_in):
        if file.endswith(".wav"):
            src = path_in + file 
            dst = path_out + file.split(".")[0] + ".wav"
	    downsampleWav(src, dst)
