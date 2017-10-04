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

export NISTINC=$PWD/front-end-dsp/nist/include
export NISTLIB=$PWD/front-end-dsp/nist/lib
export DSPLIB=$PWD/front-end-dsp/src

###########To install NIST############

LNG=$1

echo "in env_setup====" $LNG

cd front-end-dsp/nist/
sh src/scripts/install.sh 

cd ..
make -B
cd Segmentation 
make -B
cd ../..
make -B

#cp ../source_hybrid_seg/$LNG/run_hybrid_segmentation.sh ./
cp ../resources/common/hybrid_seg/run_hybrid_segmentation.sh ./
sh run_hybrid_segmentation.sh $LNG  #It copies required input data and executes run.sh script part by part.
