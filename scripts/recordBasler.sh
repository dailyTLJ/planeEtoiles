# RECORD frames from the default video output
# 
# necessary steps: 
# ./switcherBasler.sh  ... to run the camera with switcher

# accesses:
# /tmp/switcher_default_video_video_0
#
# save output to:
# /tmp/default_recording/
# * output folder needs to be set to chmod 777 !

#!/bin/bash
source helpers.sh

# cd ../data
blobserver -C recordBasler.xml
killall blobserver
