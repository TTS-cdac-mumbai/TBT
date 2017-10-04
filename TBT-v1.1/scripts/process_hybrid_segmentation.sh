#! bin/bash

###############################################

#Designed and Developed by Atish and Rachana
#Main Script for hybrid segmentation

###############################################

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
