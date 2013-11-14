#!/bin/bash

#for ((j = 8; j <= 16; j += 4));
#do
#    blobtrainer -T -E 0.1 -c 0.01 --cell-size ${j}x${j} --block-size 1x1 -o model${j}.xml >> results.csv &
#    blobtrainer -T -E 0.1 -c 0.01 --cell-size ${j}x${j} --block-size 2x2 -o model`expr ${j} + 1`.xml >> results.csv &
#    blobtrainer -T -E 0.1 -c 0.01 --cell-size ${j}x${j} --block-size 3x3 -o model`expr ${j} + 2`.xml >> results.csv &
#    blobtrainer -T -E 0.1 -c 0.01 --cell-size ${j}x${j} --block-size 4x4 -o model`expr ${j} + 3`.xml >> results.csv &
#    blobtrainer -T -E 0.1 -c 0.01 --cell-size ${j}x${j} --block-size 1x2 -o model`expr ${j} + 4`.xml >> results.csv &
#    blobtrainer -T -E 0.1 -c 0.01 --cell-size ${j}x${j} --block-size 2x1 -o model`expr ${j} + 5`.xml >> results.csv
#done

#for ((i = 5; i <= 13; i += 1));
#do
#    for ((j = 8; j <= 16; j += 4));
#    do
#        blobtrainer -T -p positive_bw -n negative_bw --crossValidation 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size ${j}x${j} --cell-max-size `expr ${j} '*' 2`x`expr ${j} '*' 2` --cell-step 1.7x1.7 --block-size 2x2 >> results_bw.csv &
#        blobtrainer -T -p positive_bw -n negative_bw --crossValidation 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size ${j}x${j} --cell-max-size `expr ${j} '*' 4`x`expr ${j} '*' 4` --cell-step 1.7x1.7 --block-size 2x2 >> results_bw.csv &
#        blobtrainer -T -p positive_bw -n negative_bw --crossValidation 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size ${j}x${j} --cell-max-size `expr ${j} '*' 8`x`expr ${j} '*' 8` --cell-step 1.7x1.7 --block-size 2x2 >> results_bw.csv
#        blobtrainer -T -p positive_bw -n negative_bw --crossValidation 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size ${j}x${j} --cell-max-size `expr ${j} '*' 2`x`expr ${j} '*' 2` --cell-step 1.3x1.3 --block-size 2x2 >> results_bw.csv &
#        blobtrainer -T -p positive_bw -n negative_bw --crossValidation 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size ${j}x${j} --cell-max-size `expr ${j} '*' 4`x`expr ${j} '*' 4` --cell-step 1.3x1.3 --block-size 2x2 >> results_bw.csv &
#        blobtrainer -T -p positive_bw -n negative_bw --crossValidation 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size ${j}x${j} --cell-max-size `expr ${j} '*' 8`x`expr ${j} '*' 8` --cell-step 1.3x1.3 --block-size 2x2 >> results_bw.csv
#    done
#done

# for ((i = 5; i <= 13; i += 1));
# do
#     for ((j = 8; j <= 16; j += 4));
#     do
#         blobtrainer -T --roi-size 80x160 --position 0x0 --crossValidation 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size ${j}x${j} --cell-step 1.7x1.7 --block-size 1x1 -o model_wr.xml >> plane_training.csv &
#         blobtrainer -T --roi-size 80x160 --position 0x0 --crossValidation 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size ${j}x${j} --cell-step 1.7x1.7 --block-size 2x2 -o model_wr.xml >> plane_training.csv &
#         blobtrainer -T --roi-size 80x160 --position 0x0 --crossValidation 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size ${j}x${j} --cell-step 1.7x1.7 --block-size 3x3 -o model_wr.xml >> plane_training.csv &
#         blobtrainer -T --roi-size 80x160 --position 0x0 --crossValidation 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size ${j}x${j} --cell-step 1.3x1.3 --block-size 1x1 -o model_wr.xml >> plane_training.csv &
#         blobtrainer -T --roi-size 80x160 --position 0x0 --crossValidation 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size ${j}x${j} --cell-step 1.3x1.3 --block-size 2x2 -o model_wr.xml >> plane_training.csv &
#         blobtrainer -T --roi-size 80x160 --position 0x0 --crossValidation 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size ${j}x${j} --cell-step 1.3x1.3 --block-size 3x3 -o model_wr.xml >> plane_training.csv
#     done
# done


for ((i = 9; i <= 13; i += 2));
do
    for ((j = 12; j <= 16; j += 4));
    do
        blobtrainer -T --roi-size 80x160 --position 0x0 -C 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size ${j}x${j} --cell-step 1.3x1.3 --block-size 2x2 -o model_nov11.xml >> plane_training_NOV11.csv &
        blobtrainer -T --roi-size 80x160 --position 0x0 -C 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size ${j}x${j} --cell-step 1.3x1.3 --block-size 3x3 -o model_nov11.xml >> plane_training_NOV11.csv
    done
done