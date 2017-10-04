#! bin/bash

###############################################

#Designed and Developed by Atish and Rachana
#Script to add silence and downsample the 48KHz waves to 16KHz

###############################################


cd ..
rm -rf temp/hybrid_seg/wav_16KHz/
mkdir temp/hybrid_seg/wav_16KHz/
rm -rf temp/hybrid_seg/SIL_added_wav_48KHz/
mkdir temp/hybrid_seg/SIL_added_wav_48KHz/

cp -r resources/common/hybrid_seg/sil_detect_add.py temp/hybrid_seg
cp -r resources/common/hybrid_seg/downsample.py temp/hybrid_seg

cd temp/hybrid_seg
ls ../../input/wav_48KHz/*.wav > list
python sil_detect_add.py list 
python downsample.py
cd ../..
