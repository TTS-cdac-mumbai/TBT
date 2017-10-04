#! bin/bash

###############################################

#Designed and Developed by Atish and Rachana
#Script to run phone level and syllable level lab generation parallaly

###############################################


rm -rf ../syllable_level_lab
rm -rf ../phone_level_lab

LNG=$1
cd ..
mkdir syllable_level_lab
mkdir phone_level_lab

cp resources/common/phoneme/phone_level_labs.c phone_level_lab/
cp resources/common/syllable/syllable_level_labs.c syllable_level_lab/

cd phone_level_lab/
gcc phone_level_labs.c -o phone_lab

cd ..
cd syllable_level_lab/
gcc syllable_level_labs.c -o syllable_lab

cd ..
np=$(nproc)
temp=$(expr "$np" / 2)
np_phone=$(expr "$temp" - 1)
cd phone_level_lab/
taskset -c 0-$np_phone ./phone_lab $LNG &

cd ..
np_syl1=$(expr "$np" / 2)
np_syl2=$(expr "$np" - 1)
cd syllable_level_lab/
taskset -c $np_syl1-$np_syl2 ./syllable_lab $LNG &

phone_lab_id=$(pidof phone_lab)
syllable_lab_id=$(pidof syllable_lab)

wait $phone_lab_id $syllable_lab_id

cd ../phone_level_lab/
pkill phone_lab
cd ../syllable_level_lab/
pkill syllable_lab


sed -e '1,2d' syldict > syldict1
sed -i -e '1i SIL SIL' syldict1
mv syldict1 syldict
cd ..

rm -rf temp/hybrid_seg/prompt-lab
cp -r syllable_level_lab/prompt-lab temp/hybrid_seg/
cp -r syllable_level_lab/syldict temp/hybrid_seg/

echo -e "\n\n$(tput setaf 2)==Done with Syldict Generation==$(tput sgr0) \n\n"
