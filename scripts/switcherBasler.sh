# Access BASLER ip camera with switcher

# outputs camera to
# /tmp/switcher_default_video_video_0



#!/bin/bash
source helpers.sh

killall switcher
switcher -d &
sleep 1

switcher-ctrl -C uridecodebin video
switcher-ctrl -i video to_shmdata 'http://192.168.2.102:9079/cgi-bin/mjpeg'