#!/bin/tcsh

# input --> vowel & consonant list

set count = 1
set len = `cat $2|wc -l`
while ($count <= $len)
set word = `cat $2|head -$count|tail -1`

HInit -m 1 -C config_files/config_feature -X lab -S flist -M models_hmm -l $word protos/proto_3s_2m

sed -i s/proto_3s_2m/$word/ models_hmm/proto_3s_2m
mv models_hmm/proto_3s_2m models_hmm/$word

HRest -m 1 -C config_files/config_feature -X lab -S flist -M models_hmm/reestimatedhmms/ -l $word models_hmm/$word
@ count++
end

set count = 1
set len = `cat $1|wc -l`
while ($count <= $len)
set word = `cat $1|head -$count|tail -1`

HInit -m 1 -C config_files/config_feature -X lab -S flist -M models_hmm -l $word protos/proto_5s_2m

sed -i s/proto_5s_2m/$word/ models_hmm/proto_5s_2m
mv models_hmm/proto_5s_2m models_hmm/$word

HRest -m 1 -C config_files/config_feature -X lab -S flist -M models_hmm/reestimatedhmms/ -l $word models_hmm/$word
@ count++
end
