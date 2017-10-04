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
GENDER=$1
LNG=$2

cd ..
make wave_processing

rm -rf Hybrid_Segmentation
tar -xvzf resources/common/hybrid_seg/Hybrid_Segmentation.tar.gz 

mv Hybrid\ Segmentation\ Ver2.0 Hybrid_Segmentation
cp resources/common/hybrid_seg/env_setup.sh Hybrid_Segmentation/

np=$(nproc)
temp=$(expr "$np" / 2)
np_freq=$(expr "$temp" - 1)
np_hyb1=$(expr "$np" / 2)
np_hyb2=$(expr "$np" - 1)

cd scripts/
taskset -c $np_hyb1-$np_hyb2 sh f0_calculation_master.sh $GENDER &

cd ../Hybrid_Segmentation/
taskset -c 0-$np_freq sh env_setup.sh $LNG
cd ..

echo -e "\n\n$(tput setaf 2)==Completed Hybrid Segmentation==$(tput sgr0) \n\n"
