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


# for ((i = 9; i <= 13; i += 2));
# do
#     for ((j = 12; j <= 16; j += 4));
#     do
#         blobtrainer -T -p ./models/positive -n ./models/negative --roi-size 80x160 --position 0x0 -C 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size ${j}x${j} --cell-step 1.3x1.3 --block-size 2x2 -o models/model_nov20k_cell${j}_bin${i}_block2.xml >> models/plane_training_NOV20.csv &
#         blobtrainer -T -p ./models/positive -n ./models/negative --roi-size 80x160 --position 0x0 -C 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size ${j}x${j} --cell-step 1.3x1.3 --block-size 3x3 -o models/model_nov20k_cell${j}_bin${i}_block3.xml >> models/plane_training_NOV20.csv
#     done
# done



## multiscale

# for ((i = 9; i <= 13; i += 2));
# do
#     for ((j = 12; j <= 16; j += 4));
#     do
#         blobtrainer -T -p ./models/positive -s 48 -n ./models/negative --roi-size 80x160 --position 0x0 -C 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size ${j}x${j} --cell-step 1.3x1.3 --block-size 2x2 --cell-max-size `expr ${j} '*' 2`x`expr ${j} '*' 2` --cell-step 1.3x1.3 -o models/model_nov29c_multi_cell${j}_bin${i}_block2.xml >> models/plane_training_NOV29.csv &
#         blobtrainer -T -p ./models/positive -s 48 -n ./models/negative --roi-size 80x160 --position 0x0 -C 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size ${j}x${j} --cell-step 1.3x1.3 --block-size 3x3 --cell-max-size `expr ${j} '*' 2`x`expr ${j} '*' 2` --cell-step 1.3x1.3 -o models/model_nov29c_multi_cell${j}_bin${i}_block3.xml >> models/plane_training_NOV29.csv
#     done
# done

# blobtrainer -T -p ./models/positive -s 32 -n ./models/negative --roi-size 80x160 --position 0x0 -C 0.7 -E 0.1 -c 0.01 -b 9 --cell-size 16x16 --cell-step 1.3x1.3 --block-size 3x3 --cell-max-size 32x32 --cell-step 1.3x1.3 -o models/model_nov29b_multi_cell16_bin9_block3.xml >> models/plane_training_NOV29.csv


## multiscale, always cellsize16, different max-size, with subdivision

# for ((i = 9; i <= 13; i += 2));
# do
#     for ((j = 2; j <= 3; j += 1));
#     do
#         blobtrainer -T -p ./models/positive -s 48 -n ./models/negative --roi-size 80x160 --position 0x0 -C 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size 16x16 --cell-step 1.3x1.3 --block-size 2x2 --cell-max-size `expr ${j} '*' 16`x`expr ${j} '*' 16` --cell-step 1.3x1.3 -o models/model_nov29c_multi_cell16_max`expr ${j} '*' 16`_bin${i}_block2.xml >> models/plane_training_NOV29.csv &
#         blobtrainer -T -p ./models/positive -s 48 -n ./models/negative --roi-size 80x160 --position 0x0 -C 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size 16x16 --cell-step 1.3x1.3 --block-size 3x3 --cell-max-size `expr ${j} '*' 16`x`expr ${j} '*' 16` --cell-step 1.3x1.3 -o models/model_nov29c_multi_cell16_max`expr ${j} '*' 16`_bin${i}_block3.xml >> models/plane_training_NOV29.csv
#     done
# done


for ((i = 9; i <= 9; i += 2));
do
    for ((j = 8; j <= 8; j += 4));
    do
        # blobtrainer -T -p ./models/positive -n ./models/negative -s 64 --cell-step 1.3x1.3 --roi-size 80x160 --position 0x0 -C 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size ${j}x${j} --block-size 1x1 -o models/model_nov29e_cell${j}_bin${i}_block1.xml >> models/plane_training_NOV29.csv &
        # blobtrainer -T -p ./models/positive -n ./models/negative -s 64 --cell-step 1.3x1.3 --roi-size 80x160 --position 0x0 -C 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size ${j}x${j} --block-size 2x2 -o models/model_nov29e_cell${j}_bin${i}_block2.xml >> models/plane_training_NOV29.csv &
        blobtrainer -T -p ./models/positive -n ./models/negative -s 64 --cell-step 1.3x1.3 --roi-size 80x160 --position 0x0 -C 0.7 -E 0.1 -c 0.01 -b ${i} --cell-size ${j}x${j} --block-size 3x3 -o models/model_nov29e_cell${j}_bin${i}_block3.xml >> models/plane_training_NOV29.csv
    done
done