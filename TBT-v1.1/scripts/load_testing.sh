#! /bin/bash

###############################################

#Designed and Developed by Atish and Rachana
#Main Script to build htsvoice
#script to load and test the built voice.

###############################################

LNG=$1

#tar -xvzf ../resources/languages/$LNG/htsvoice_synthesis.tar.gz -C ../

#cp ../HTS-demo_CMU-ARCTIC-SLT/voices/qst001/ver1/*.htsvoice ../htsvoice_synthesis/voices

cp $FESTDIR/src/main/festival ../htsvoice_synthesis/resources/bin/

sudo locate hts_engine_API-1.10 > hts_engine_list
hts_engine=$(head -1 hts_engine_list)
cp $hts_engine/bin/hts_engine ../htsvoice_synthesis/resources/bin/

cd ../htsvoice_synthesis/
perl synth_text.pl "छत्रपती शिवाजी महाराजांच्या शौर्याची, कर्तृत्वाची, पराक्रमाची खूप महान महती आहे."

echo "Done with HTS-VOICE tesing"
