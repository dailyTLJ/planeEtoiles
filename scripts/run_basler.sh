# RUN HOG on Basler camera
# 
# necessary steps: 
# ./switcherBasler.sh  ... to run camera with switcher

# accesses:
# /tmp/switcher_default_video_video_0



#!/bin/bash
source helpers.sh

# cd ../data
blobserver -C basler_ip9.xml $@ &
sleep 5
shmvid /tmp/blobserver_output_1
killall blobserver
killall gst-launch-0.10