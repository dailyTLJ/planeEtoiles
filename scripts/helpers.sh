#!/bin/bash

# display video from the shmdata
function shmvid {
    gst-launch-0.10 shmsrc socket-path=$1 ! gdpdepay ! decodebin2 ! ffmpegcolorspace ! fpsdisplaysink sync=false force-aspect-ratio=true
}

# play audio from the shmdata
function shmaudio {
    gst-launch-0.10 shmsrc socket-path=$1 ! gdpdepay ! decodebin2 ! pulsesink sync=false
}

# display metadata from the shmdata
function shmcaps {
    gst-launch-0.10 shmsrc -v socket-path=$1 ! gdpdepay ! fakesink silent=true
}

# display metadata of decoded buffers
function shmcapsdecoded {
    gst-launch-0.10 shmsrc -v socket-path=$1 ! gdpdepay ! decodebin2 ! fakesink silent=true
}

# display message for each buffers in the flow
function shmflow {
    gst-launch-0.10 shmsrc -v socket-path=$1 ! gdpdepay ! fakesink
}

# display message for each decoded buffers in the flow
function shmflowdecoded {
    gst-launch-0.10 shmsrc -v socket-path=$1 ! gdpdepay ! decodebin2 ! fakesink
}
