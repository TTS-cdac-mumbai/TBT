#!/bin/bash

#cd resources$2
#for SOME_VAR in "$@"
#do
   # echo $SOME_VAR >> sentence.txt
#done;
echo "atish"
echo "param1=$1 param2=$2"

cd resources$2
echo $1 > sentence.txt

    sed -i s/\(/\ \(\ \,\ \,\ \ / sentence.txt
    sed -i s/\)/\ \)\ \,\ \,\ \ / sentence.txt
    sed -i s/\।/\;\ / sentence.txt
    #rm data/*/*.lab
    perl parser/rewrite.pl
 # export $FESTDIR=/home/boss/tts_install/festival
   echo $PATH >> pranaw.txt
export PATH=/usr/lib/lightdm/lightdm:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/home/boss/android_install/android-sdk-linux/tools:/home/boss/android_install/android-sdk-linux/platform-tools:/home/boss/android_install/android-ndk-r10e/prebuilt/linux-x86_64/arm-eabi-4.2.1/bin:/home/boss/installations/android/agcc-0.2
export ESTDIR=/home/boss/tts_install/speech_tools
export FESTDIR=/home/boss/tts_install/festival
export FESTVOXDIR=/home/boss/tts_install/festvox
echo $PATH >> pranaw.txt  
echo $SHELL >> pranaw.txt  
echo $FESTDIR >> pranaw.txt
    printenv >>pranaw.txt
    rm -f sentence.txt
    cd parser/
     #festival >> pranaw.txt
    #festival -b festvox/build_clunits.scm '(build_prompts "etc/txt.done.data")' >> pranaw.txt
   #cp $FESTDIR/bin/festival $FESTDIR/bin/festival$2
 #$FESTDIR/bin/festival$2 -b festvox/build_clunits.scm '(build_prompts "etc/txt.done.data")' >> pranaw.txt
  ../festival/bin/festival -b festvox/build_clunits.scm '(build_prompts "etc/txt.done.data")' >> pranaw.txt
 
    cd ../
    mv parser/prompt-utt/test.utt data/utts/
    cd data/
    make label
    find labels/full/ -name \*.lab -exec sed -i "s/  / /g" {} \;
    find labels/full/ -name \*.lab -exec sed -i "s/  / /g" {} \;
    find labels/full/ -name \*.lab -exec sed -i "s/  / /g" {} \;
    find labels/full/ -name \*.lab -exec sed -i "s/  / /g" {} \;
    cut -d " " -f4 labels/full/test.lab > labels/gen/test.lab
    cd ../
  #   ./hts_engine_API-1.10/bin/hts_engine -m voices/cmu_us_arctic_slt.htsvoice  -ow test.wav data/labels/gen/test.lab
   ./hts_engine_API-1.10/bin/hts_engine -m voices/iitm_unified_hindi.htsvoice  -ow test.wav data/labels/gen/test.lab
        
#perl scripts/synth.pl iitm_unified_hindi.htsvoice
#play gen/hts_engine/test.wav

