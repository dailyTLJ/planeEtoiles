# RUN HOG on recorded video
# 
# necessary steps: 
# ./switcherBaslerRec.sh  ...

# accesses:
# /tmp/switcher_default_video_video_0


#!/bin/bash
source helpers.sh

# cd ../data
blobserver -C basler_rec_eva.xml
# $@ &
#sleep 5

#gst-launch-0.10 shmsrc socket-path=/tmp/blobserver_output_2 ! \
#	gdpdepay ! decodebin2 ! ffmpegcolorspace ! \
#	fpsdisplaysink sync=false force-aspect-ratio=true &
#gst-launch-0.10 shmsrc socket-path=/tmp/blobserver_output_1 ! \
#	gdpdepay ! decodebin2 ! ffmpegcolorspace ! \
#	fpsdisplaysink sync=false force-aspect-ratio=true 
# disp-width=640 disp-height=480
# shmvid /tmp/blobserver_output_2 &
# shmvid /tmp/blobserver_output_1
killall blobserver
killall gst-launch-0.10
