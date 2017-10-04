#!/bin/tcsh

# input --> vowel & consonant list
cp hmm_GMV/macros hmm0/
set count = 1
set len = `cat $1|wc -l`
while ($count <= $len)
set word = `cat $1|head -$count|tail -1`
cat hmm_GMV/proto_5s_2m >> hmm_GMV/hmmdefs
sed -i s/proto_5s_2m/$word/ hmm_GMV/hmmdefs
cat hmm_GMV/proto_5s_2m >> hmm_GMV/hmmdefs
sed -i s/proto_5s_2m/beg-$word/ hmm_GMV/hmmdefs
cat hmm_GMV/proto_5s_2m >> hmm_GMV/hmmdefs
sed -i s/proto_5s_2m/$word\_end/ hmm_GMV/hmmdefs
cat hmm_GMV/proto_5s_2m >> hmm_GMV/hmmdefs
sed -i s/proto_5s_2m/beg-$word\_end/ hmm_GMV/hmmdefs
@ count++
end

set count = 1
set len = `cat $2|wc -l`
while ($count <= $len)
set word = `cat $2|head -$count|tail -1`
cat hmm_GMV/proto_3s_2m >> hmm_GMV/hmmdefs
sed -i s/proto_3s_2m/$word/ hmm_GMV/hmmdefs
cat hmm_GMV/proto_3s_2m >> hmm_GMV/hmmdefs
sed -i s/proto_3s_2m/beg-$word/ hmm_GMV/hmmdefs
cat hmm_GMV/proto_3s_2m >> hmm_GMV/hmmdefs
sed -i s/proto_3s_2m/$word\_end/ hmm_GMV/hmmdefs
@ count++
end
