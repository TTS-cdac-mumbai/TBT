#! bin/bash

###############################################

#Designed and Developed by Atish and Rachana
#Main Script to build htsvoice

###############################################

LNG=$1
cd ..
tar -xjf resources/common/htsvoice/HTS-demo_CMU-ARCTIC-SLT.tar.bz2 

if [ $(find temp/htsvoice/utts | wc -l) -gt 0 ]
then
   echo "Found something"
else
   make utt_generation &
fi

if [ $(find temp/htsvoice/raw_files | wc -l) -gt 0 ]
then
   echo "Found something"
else
   make wav2raw
fi

cp resources/common/htsvoice/build_htsvoice.sh HTS-demo_CMU-ARCTIC-SLT/
cd HTS-demo_CMU-ARCTIC-SLT/
sh build_htsvoice.sh $LNG

echo -e "\n\n$(tput setaf 2)==Completed HTSVOICE Building==$(tput sgr0) \n\n"
