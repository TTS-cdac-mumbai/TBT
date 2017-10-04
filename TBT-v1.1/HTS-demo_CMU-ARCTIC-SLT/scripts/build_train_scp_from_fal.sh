#! bin/bash
##############
#script to build train.scp

echo "======train.scp started..======="


ls $PWD/gv/qst001/ver1/fal/*.lab > $PWD/data/scp/train.txt
sed -i 's/.lab/.cmp/g' $PWD/data/scp/train.txt
sed -i 's|'$PWD'\/gv\/qst001\/ver1\/fal\/||g' $PWD/data/scp/train.txt
echo "======train.scp building..======="
path_var=$(head -1 $PWD/data/scp/train.scp | rev | sed 's/^[^\/]*\//\//' | rev)
rm -rf $PWD/data/scp/train.scp
awk '{print "'$path_var'"$0}' $PWD/data/scp/train.txt > $PWD/data/scp/train.scp
echo "======train.scp completed..======="
