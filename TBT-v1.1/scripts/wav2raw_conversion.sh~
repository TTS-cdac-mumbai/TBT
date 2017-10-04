# !bin/bash

# !bin/bash

###############################################

#Designed and Developed by Atish and Rachana
#Script to add silence in 48KHz wave files and to convert those into Raw files.

###############################################

cd ..

rm -rf temp/htsvoice/raw_files
mkdir temp/htsvoice/raw_files
cp -r resources/common/htsvoice/raw_conversion.scr temp/htsvoice/raw_files

rm -rf temp/htsvoice/raw_files/wav_48KHz
mkdir temp/htsvoice/raw_files/wav_48KHz
cp -r temp/hybrid_seg/SIL_added_wav_48KHz/*.wav temp/htsvoice/raw_files/wav_48KHz

cd temp/htsvoice/raw_files

ls wav_48KHz/*.wav > wav_list   ### taking Sil added wavefiles.
./raw_conversion.scr wav_list
cd ../../..

echo -e "\n\n$(tput setaf 2)==Completed conversion of wav_48KHz to Raw==$(tput sgr0) \n\n"
