#! bin/bash

###############################################

#Designed and Developed by Atish and Rachana
#Script for environmental setup for hybrid segmentation

###############################################

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
