##### master file for f0 calculation


cp ../resources/common/hybrid_seg/f0_calculation/processpitch.sh ../temp/hybrid_seg/
cp ../resources/common/hybrid_seg/f0_calculation/pitchextract.sh ../temp/hybrid_seg/

cd ../temp/hybrid_seg/
sh processpitch.sh $1
