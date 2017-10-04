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

echo "======train.scp started..======="


ls $PWD/gv/qst001/ver1/fal/*.lab > $PWD/data/scp/train.txt
sed -i 's/.lab/.cmp/g' $PWD/data/scp/train.txt
sed -i 's|'$PWD'\/gv\/qst001\/ver1\/fal\/||g' $PWD/data/scp/train.txt

echo "======train.scp building..======="

path_var=$(head -1 $PWD/data/scp/train.scp | rev | sed 's/^[^\/]*\//\//' | rev)
rm -rf $PWD/data/scp/train.scp
awk '{print "'$path_var'"$0}' $PWD/data/scp/train.txt > $PWD/data/scp/train.scp

echo "======train.scp completed..======="
