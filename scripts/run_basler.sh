# RUN HOG on Basler camera
# 
# necessary steps: 
# ./switcherBasler.sh  ... to run camera with switcher

# accesses:
# /tmp/switcher_default_video_video_0



#!/bin/bash
source helpers.sh

# cd ../data
blobserver -C basler.xml $@ &
sleep 5
shmvid /tmp/blobserver_1_hog_0
killall blobserver
killall gst-launch-0.10