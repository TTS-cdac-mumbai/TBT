#! bin/bash

###############################################

#Designed and Developed by Atish and Rachana
#Script to generate phoneme level utterance files

###############################################

cd ..
rm -rf phone_level_lab/lab
cp -r Hybrid_Segmentation/output_lab_phone phone_level_lab/
mv phone_level_lab/output_lab_phone phone_level_lab/lab

cp resources/common/htsvoice/build_utts.c phone_level_lab/
cd phone_level_lab/
gcc build_utts.c -o build_utts

./build_utts
cd ..

rm -rf temp/htsvoice/utts
cp -r phone_level_lab/festival/utts temp/htsvoice/utts

echo -e "\n\n$(tput setaf 2)==Completed phoneme level UTT file generation==$(tput sgr0) \n\n"
