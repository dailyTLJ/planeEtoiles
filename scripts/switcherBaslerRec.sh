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
# switcher-ctrl -s video uri 'file:///home/planeblob/Videos/random.avi'
# switcher-ctrl -s video uri 'file:///home/planeblob/Videos/snowVid30.avi'
# switcher-ctrl -s video uri 'file:///home/planeblob/Videos/snowVid15.avi'
# switcher-ctrl -s video uri 'file:///home/planeblob/Videos/snowJUMP15.avi'
switcher-ctrl -s video uri 'file:///home/planeblob/Videos/full.avi'
# switcher-ctrl -s video uri 'file:///home/planeblob/Videos/standingstill.avi'  ###
# switcher-ctrl -s video uri 'file:///home/planeblob/Videos/keepdistance.avi'  ### car
# switcher-ctrl -s video uri 'file:///home/planeblob/Videos/spin.avi'
# switcher-ctrl -s video uri 'file:///home/planeblob/Videos/hop_freeze.avi'  ### car
# switcher-ctrl -s video uri 'file:///home/planeblob/Videos/run_freeze.avi'
# switcher-ctrl -s video uri 'file:///home/planeblob/Videos/align.avi'
# switcher-ctrl -s video uri 'file:///home/planeblob/Videos/drop.avi'
# switcher-ctrl -s video uri 'file:///home/planeblob/Videos/groups.avi'  ### car
switcher-ctrl -s video started true