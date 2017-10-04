#!/bin/bash

cd resources$2
echo $1 > sentence.txt

    sed -i s/\(/\ \(\ \,\ \,\ \ / sentence.txt
    sed -i s/\)/\ \)\ \,\ \,\ \ / sentence.txt
    sed -i s/\।/\;\ / sentence.txt

    perl parser/rewrite.pl
    rm -f sentence.txt
    cd parser/
     
  ../bin/festival -b festvox/build_clunits.scm '(build_prompts "etc/txt.done.data")' 

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

   ./bin/hts_engine -m  ../voices/*.htsvoice -ow test.wav data/labels/gen/test.lab
 
        rm gen/hts_engine/*.raw
        rm pranaw.txt
