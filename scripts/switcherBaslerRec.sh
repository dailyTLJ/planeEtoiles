# Play images sequence with switcher

# accesses video at
# /Documents/blobserver/_grabs/video.avi

# outputs recording to
# /tmp/switcher_default_video_gstsrc


#!/bin/bash
killall switcher
switcher -d &
sleep 1

# switcher-ctrl -C uridecodebin video
# switcher-ctrl -s video loop true
# switcher-ctrl -i video to_shmdata 'file:///home/radioair/Documents/blobserver/_grabs/video.avi'

switcher-ctrl -C uridecodebin video
switcher-ctrl -s video loop true
switcher-ctrl -s video uri 'file:///home/radioair/Documents/blobserver/_grabs/video.avi'
switcher-ctrl -s video started true